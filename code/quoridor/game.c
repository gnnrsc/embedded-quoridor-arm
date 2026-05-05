#include "LPC17xx.h"
#include "../GLCD/GLCD.h" 
#include "../timer/timer.h"
#include "../CAN/CAN.h"
#include "../RIT/RIT.h"
#include "game.h"
#include "draw.h"
#include "wall.h"
#include "npc.h"
#include <stdbool.h>

extern uint8_t round_timer; 
volatile uint8_t wallP1=8; 
volatile uint8_t wallP2=8; 

volatile coord token[7][7]; 
volatile coord wall[6][6]; 
volatile bool direction; //0=orizzontale 1=verticale

//1 muri 0 spazi liberi 2 pedine
volatile uint16_t board[ROWS][COLS];

volatile Point p[2]; //posizione dei player
volatile bool player; // 0=player1, 1=player2
volatile int playerMulti=2;
volatile Point nextPosition[6];
volatile int diagonal;

extern Point scelta;
extern uint8_t round_timer;

extern bool handshake;
extern CAN_msg       CAN_TxMsg;    /* CAN message for sending */
extern CAN_msg       CAN_RxMsg;    /* CAN message for receiving */
extern uint32_t move;

extern int lenght;
extern int transfer(bool player, bool moveWall, bool direction, int y, int x);

volatile int mode=-1; //modalità di gioco
// 0 - menu GAME MODE
// 1 - menu Single Board
// 2 - menu Two Board
// 13 - HUMAN Single Board
// 14 - NPC Single Board
// 23 - HUMAN Two Board
// 24 - NPC Two Board

void startGame(){
	//mode=0;
	LCD_Clear(Black);
	drawGrid();
	init_coord();
	initBoard();
	writeWallP1();
	writeTime();
	writeWallP2();
	enable_RIT();
	//writePressINT0(White,Blue);
	//enable KEY1
	NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
		LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 1 pin selection */
}

void init_coord(){
	//inizializza coordinate
	//token
	uint16_t i,j;
	for(i=0;i<7;i++){
		for(j=0;j<7;j++){
			token[i][j].x=7+(33*j);
			token[i][j].y=7+(33*i);
		}
	}
	//muri 
	for(i=0;i<6;i++){
		for(j=0;j<6;j++){
			wall[i][j].x=(7+27)+(j*33);
			wall[i][j].y=(7+27)+(i*33);
		}
	}
}

void initBoard(){
	//player 1 [0] Bianco - sotto (12,6)
	//player 2 [1] Red  -sopra (0,6)
	uint16_t i,j;
	for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            board[i][j] = 0;
        }
  }
	p[0].row=12;
  p[0].col=6;
	p[1].row=0;
	p[1].col=6;
	board[p[0].row][p[0].col]=2; 
	board[p[1].row][p[1].col]=2;
	drawPlayer(p[0].row,p[0].col,White);
	drawPlayer(p[1].row,p[1].col,Red);
	player=0; //inizia il player 1
}

void clearTokenBoard(){
	//player 1 [0] Bianco - sotto (12,6)
	//player 2 [1] Red  -sopra (0,6)
	uint16_t i;
	for (i = 0; i < 4; i++) {
			drawPlayer(nextPosition[i].row,nextPosition[i].col,Black);
  }
	if(nextPosition[4].row!=-1){
			drawPlayer(nextPosition[4].row,nextPosition[4].col,Black);
		}
		if(nextPosition[5].row!=-1){
			drawPlayer(nextPosition[5].row,nextPosition[5].col,Black);
		}
	scelta.row=-1;
}

bool isValid(Point nextP) {
    return (nextP.row >= 0) && (nextP.row < ROWS) && (nextP.col >= 0) && (nextP.col < COLS);
}

bool isDestination(Point p1, Point p2) {
    return (p1.row==0 || p2.row==12);
}
bool isBlocked(Point pos) {
    return (board[pos.row][pos.col] == 1 || board[pos.row][pos.col]==3 || board[pos.row][pos.col]==4); // 1 o 3 o 4 è il muro
}

bool isInFront(Point pos){
	return board[pos.row][pos.col] == 2;
}

coord diagonalIndex(int i){
	coord p; //p.x=sx    p.y=dx
		switch(i){
					 case 0:
						 p.x=3;
					 	 p.y=1;
						break;
					 case 1:
						 p.x=0;
					   p.y=2;
						break;
					 case 2:
						 p.x=1;
					   p.y=3;
					  break;
					 case 3:
						 p.x=2;
					   p.y=0;
						break;
					 default:
						 break;
				 }
		return p;
}

void nextMove(Point player){
	//nextPosition[0] sopra
	//nextPosition[1] destra
	//nextPosition[2] sotto
	//nextPosition[3] sinistra
	//nextPosition[4] infront "sx"
	//nextPosition[5] infront "dx"
		uint16_t i,sx,dx;
		int row_moves[] = {-2, 0, 2, 0};
		int col_moves[] = {0, 2, 0, -2};

    int row_moves_w[] = {-1, 0, 1, 0};
    int col_moves_w[] = {0, 1, 0, -1};
		
		
		Point nextPositionWall[6]={0};
		diagonal=-1;
		nextPosition[4].col=-1;
		nextPosition[4].row=-1;
		nextPosition[5].col=-1;
		nextPosition[5].row=-1;
		
		for (i = 0; i < 4; i++) {
    nextPosition[i].row = player.row + row_moves[i];
    nextPosition[i].col = player.col+ col_moves[i];

    nextPositionWall[i].row =  player.row + row_moves_w[i];
    nextPositionWall[i].col =  player.col + col_moves_w[i];
            
     if(!isBlocked(nextPositionWall[i]) && isInFront(nextPosition[i])){
            	nextPosition[i].row += row_moves[i];
            	nextPosition[i].col += col_moves[i];

            	nextPositionWall[i].row += row_moves[i];
            	nextPositionWall[i].col += col_moves[i];
			 
				if(isValid(nextPosition[i])){
			 if(!isBlocked(nextPositionWall[i])){
			 drawPlayer(nextPosition[i].row,nextPosition[i].col,Yellow);
			 }else{
				 //bloccato -> diagonali
				sx=diagonalIndex(i).x;
				dx=diagonalIndex(i).y;
   				 nextPositionWall[4].row = nextPositionWall[i].row - row_moves_w[i]+row_moves_w[sx];
           nextPositionWall[4].col = nextPositionWall[i].col - col_moves_w[i]+col_moves_w[sx];
				 if(!isBlocked(nextPositionWall[4])){
					 nextPosition[4].row = nextPosition[i].row - row_moves[i] + row_moves[sx];
					 nextPosition[4].col = nextPosition[i].col - col_moves[i] + col_moves[sx];
					 drawPlayer(nextPosition[4].row,nextPosition[4].col,Yellow);
				 }else{
					 nextPosition[4].col=-1;
					 nextPosition[4].row=-1;
				 }
					 nextPositionWall[5].row = nextPositionWall[i].row - row_moves_w[i]+row_moves_w[dx];
					 nextPositionWall[5].col = nextPositionWall[i].col - col_moves_w[i]+col_moves_w[dx];
				 if(!isBlocked(nextPositionWall[5])){
					 nextPosition[5].row = nextPosition[i].row - row_moves[i] + row_moves[dx];
					 nextPosition[5].col = nextPosition[i].col - col_moves[i] + col_moves[dx];
					 drawPlayer(nextPosition[5].row,nextPosition[5].col,Yellow);
				 }else{
					 nextPosition[5].col=-1;
					 nextPosition[5].row=-1;
				 }
				 nextPosition[i].col=-1;
				 nextPosition[i].row=-1;
				 diagonal=i;
			 }
		 }else{
			 //spostamenti non consentiti
			 nextPosition[i].col=-1;
			 nextPosition[i].row=-1;
		 }
			}else{
				if(isValid(nextPosition[i])&&!isBlocked(nextPositionWall[i])){
			 drawPlayer(nextPosition[i].row,nextPosition[i].col,Yellow);
			}else{
				//spostamenti non consentiti
			 nextPosition[i].col=-1;
			 nextPosition[i].row=-1;
			}
		}
	}
}

void trasmettiMossa(){
	//send from CAN2 to CAN1
	CAN_TxMsg.data[0] = move & 0xFF;
	CAN_TxMsg.data[1] = (move >> 8) & 0xFF;
	//data[2] avrà 4 bit verticale/orizzontale e i 4 dopo giocatore/muro -> da splittare ancora
	CAN_TxMsg.data[2] = (move >> 16) & 0xFF;
	CAN_TxMsg.data[3] = (move >> 24) & 0xFF;
	CAN_TxMsg.len = 4;
	CAN_TxMsg.id = 1;
	CAN_TxMsg.format = STANDARD_FORMAT;
	CAN_TxMsg.type = DATA_FRAME;
	CAN_wrMsg (2, &CAN_TxMsg);               /* transmit message */
	//send from CAN1 to CAN2
	CAN_TxMsg.data[0] = move & 0xFF;
	CAN_TxMsg.data[1] = (move >> 8) & 0xFF;
	//data[2] avrà 4 bit verticale/orizzontale e i 4 dopo giocatore/muro -> da splittare ancora
	CAN_TxMsg.data[2] = (move >> 16) & 0xFF;
	CAN_TxMsg.data[3] = (move >> 24) & 0xFF;
	CAN_TxMsg.len = 4;
	CAN_TxMsg.id = 2;
	CAN_TxMsg.format = STANDARD_FORMAT;
	CAN_TxMsg.type = DATA_FRAME;
	CAN_wrMsg (1, &CAN_TxMsg);               /* transmit message */
}
void trasmettiHandshake(){
	//send from CAN2 to CAN1
	CAN_TxMsg.data[3] = 0xFF;
	CAN_TxMsg.len = 4;
	CAN_TxMsg.id = 1;
	CAN_TxMsg.format = STANDARD_FORMAT;
	CAN_TxMsg.type = DATA_FRAME;
	CAN_wrMsg (2, &CAN_TxMsg);               /* transmit message */
	//send from CAN1 to CAN2
	CAN_TxMsg.data[3] = 0xFF;
	CAN_TxMsg.len = 4;
	CAN_TxMsg.id = 2;
	CAN_TxMsg.format = STANDARD_FORMAT;
	CAN_TxMsg.type = DATA_FRAME;
	CAN_wrMsg (1, &CAN_TxMsg);               /* transmit message */
}

void riceviMossa(CAN_msg CAN_RxMsg){
	//data[2]
	//01 -> 1 perso turno
	//10 -> 16 muro verticale
	//11 -> 17 muro orizzontale
	//00 -> 0 pedina
	
	//id Player
	//CAN_RxMsg.data[3]
	Wall wall;
	coord c;
	if(CAN_RxMsg.data[2]!=1){ //se non perde il turno
		//char number[5] = "";
		//sprintf(number,"%d",CAN_RxMsg.data[2]);
		//GUI_Text(70, 272, (uint8_t *) number, Red, Black);
		switch (CAN_RxMsg.data[2]){
			case 16:
				//muro verticale
				c=convertTo13Wall(CAN_RxMsg.data[1],CAN_RxMsg.data[0]);
				wall.coord.row=c.x;
				wall.coord.col=c.y;
				wall.direction=1;
				drawWall(wall.coord.row,wall.coord.col,wall.direction,Magenta);
				board[wall.coord.row-1][wall.coord.col]=3;
				board[wall.coord.row+1][wall.coord.col]=3;
				board[wall.coord.row][wall.coord.col]=1;
				if(CAN_RxMsg.data[3]){
				//player 2
				wallP2--;
				writeWallP2();
				}else{
				//player 1
				wallP1--;
				writeWallP1();
				}
				break;
			case 17:
				//muro orizzontale
				c=convertTo13Wall(CAN_RxMsg.data[1],CAN_RxMsg.data[0]);
				wall.coord.row=c.x;
				wall.coord.col=c.y;
				wall.direction=0;
				board[wall.coord.row][wall.coord.col-1]=4;
				board[wall.coord.row][wall.coord.col+1]=4;
				board[wall.coord.row][wall.coord.col]=1;
				drawWall(wall.coord.row,wall.coord.col,wall.direction,Magenta);
				if(CAN_RxMsg.data[3]){
				//player 2
				wallP2--;
				writeWallP2();
				}else{
				//player 1
				wallP1--;
				writeWallP1();
				}
				break;
			case 0:
				//pedina
				//elimina precedente posizione
				drawPlayer(p[CAN_RxMsg.data[3]].row,p[CAN_RxMsg.data[3]].col,Black);
				board[p[CAN_RxMsg.data[3]].row][p[CAN_RxMsg.data[3]].col]=0;
				//aggiungi la nuova posizione
				c=convertTo13Token(CAN_RxMsg.data[1],CAN_RxMsg.data[0]);
				p[CAN_RxMsg.data[3]].row=c.x;
				p[CAN_RxMsg.data[3]].col=c.y;
				board[p[CAN_RxMsg.data[3]].row][p[CAN_RxMsg.data[3]].col]=2;
				if(!CAN_RxMsg.data[3]){
				//player 1
					drawPlayerMultiplayer(CAN_RxMsg.data[1],CAN_RxMsg.data[0],White);
				}else{
					drawPlayerMultiplayer(CAN_RxMsg.data[1],CAN_RxMsg.data[0],Red);
				}
				if(isDestination(p[0],p[1])){
				//VINTO
				GUI_Text(87, 239, (uint8_t *) "YOU WON!", Red, Yellow);
				disable_RIT();
				disable_timer(0);
			}
				break;
			default:
				break;
		}
	}else{
		//perde il turno
		
	}
	//cambio turno
	if(!isDestination(p[0],p[1])){
				player=(player)?0:1;
	enable_RIT();
			}
}

void playNPC(){
	coord c;
	Point move1;
	Point move0;
	int len;
	if(mode==14){
		//single player NPC player 2
		move1=findFirstMove(p[1],1);
		len=lenght;
		move0=findFirstMove(p[0],0);
		//lenght<len
		if(!wallNPC(lenght,len,p[0],0)){ 
			//se il giocatore 1 è più vicino alla destinazione, piazza il muro (se si può)
			//isValidWall(sceltaW.coord.row+2,sceltaW.coord.col)
			drawPlayer(p[1].row,p[1].col,Black);
			board[p[1].row][p[1].col]=0;
			p[1].row=move1.row;
			p[1].col=move1.col;
			board[p[1].row][p[1].col]=2;
			drawPlayer(p[1].row,p[1].col,Red);
			if(isDestination(p[0],p[1])){
					//VINTO
					GUI_Text(87, 239, (uint8_t *) "YOU WON!", Red, Yellow);
					disable_RIT();
					disable_timer(0);
			}
		}
	}
	if(mode==24){
		//two player NPC
		move1=findFirstMove(p[playerMulti],playerMulti);
		len=lenght;
		move0=findFirstMove(p[!player],!player);
		//lenght<len
		if(!wallNPC(lenght,len,p[!player],!player)){ 
			//se il giocatore avversario è più vicino alla destinazione, piazza il muro (se si può)
			drawPlayer(p[playerMulti].row,p[playerMulti].col,Black);
			board[p[playerMulti].row][p[playerMulti].col]=0;
			p[playerMulti].row=move1.row;
			p[playerMulti].col=move1.col;
			board[p[playerMulti].row][p[playerMulti].col]=2;
			//trasferisci nel registro
					c=convertCoordToken(p[playerMulti].row,p[playerMulti].col);
					move=transfer(player,0,0,c.x,c.y);
					//invio mossa
					trasmettiMossa();
					//disabilito mossa
					disable_RIT();
					reset_RIT();
					disable_timer(0);
			if(!playerMulti){
				//player 1
				drawPlayer(p[playerMulti].row,p[playerMulti].col,White);
			}else{
				drawPlayer(p[playerMulti].row,p[playerMulti].col,Red);
			}
			if(isDestination(p[0],p[1])){
					//VINTO
					GUI_Text(87, 239, (uint8_t *) "YOU WON!", Red, Yellow);
					disable_RIT();
					disable_timer(0);
			}
		}
	}
}
void multiplayer(){
	trasmettiHandshake();
	if(handshake){
		//handshake=1
		playerMulti=1; //player 2
		startGame();
	  //player=playerMulti;
		disable_RIT();
		//play();
	}else{
		//handshake=0
		playerMulti=0; //player 1
		//timer enable
		enable_timer(2);
		//wait for connection
		waitForConnection(White,Red);
		disable_RIT();
	}
	
}
void changeGame(){
	if(!player){
		//se è il player 1
		if(mode==13 || mode==14 || (mode==23 && player==playerMulti)){
			nextMove(p[0]);
		}
	}else{
		//se è il player 2
		if(mode==13 || mode==14 || (mode==23 && player==playerMulti)){
			nextMove(p[1]);
		}
	}
	if(mode==24 && player==playerMulti){
			playNPC();
			player=(player)?0:1;
		}
}
void play(){
	if(!isDestination(p[0],p[1])){
			reset_timer(0);
			round_timer=TIME;
			enable_timer(0);
			writeTime();
			changeGame();
		}
}

