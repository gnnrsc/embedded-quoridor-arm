/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "../GLCD/GLCD.h"
#include "../timer/timer.h"
#include "../quoridor/game.h"
#include "../quoridor/draw.h"
#include "../quoridor/wall.h"
#include "../quoridor/npc.h"
#include <stdbool.h>
#include "stdio.h"
/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
volatile uint16_t down_0 = 0;
volatile uint16_t down_1 = 0;
volatile uint16_t down_2 = 0;
volatile uint32_t move;

extern Point nextPosition[6];
	//nextPosition[0] sopra
	//nextPosition[1] destra
	//nextPosition[2] sotto
	//nextPosition[3] sinistra
volatile Point scelta={-1};
volatile Wall sceltaW={-1};
//1 muri 0 spazi liberi 2 pedine
extern uint16_t board[ROWS][COLS];

extern Point p[2]; //posizione dei player
extern bool player; // 0=player1, 1=player2
volatile bool changeMode; // 0=token 1=wallab
extern int diagonal;

extern int mode; //modalità di gioco
volatile int sceltaMode=-1;

extern uint8_t wallP1; 
extern uint8_t wallP2;

extern int transfer(bool player, bool moveWall, bool direction, int y, int x);

extern bool handshake;
extern int playerMulti;


void checkScelta(int i){
	if(scelta.row!=-1){
					drawPlayer(scelta.row,scelta.col,Yellow);
				}
				if(nextPosition[i].row!=-1){
					drawPlayer(nextPosition[i].row,nextPosition[i].col,Green);
					scelta=nextPosition[i];
				}
}
void RIT_IRQHandler (void)
{					
	static uint16_t J_select=0;
	static uint16_t J_down = 0;
	static uint16_t J_left = 0;
	static uint16_t J_right = 0;
	static uint16_t J_up = 0;
	static uint16_t J_down_left=0;
	static uint16_t J_down_right=0;
	static uint16_t J_up_left=0;
	static uint16_t J_up_right=0;
	static uint16_t J_left_down=0;
	static uint16_t J_left_up=0;
	static uint16_t J_right_down=0;
	static uint16_t J_right_up=0;
		
	/* joystick management */
	// 0 - menu GAME MODE
// 1 - menu Single Board
// 2 - menu Two Board
// 13 - HUMAN Single Board
// 14 - NPC Single Board
// 23 - HUMAN Two Board
// 24 - NPC Two Board
	
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){	  //SELECT
		/* Joytick J_Select pressed p1.25*/
		uint16_t i;
		coord c;
		J_select++;
		switch(J_select){
			case 1:
				if(mode==24 && player==playerMulti){
					//mode=24 NPC two board
					//handshake player?
					//if(handshake){
						//invio move
					//playNPC();
					//player=(player)?0:1;
					//}else{
						//handshake
						//multiplayer();
					//}
					
					//player=playerMulti; //o in INT0?
					
					
				}
				if(mode==13 || (mode==14 && player==0) || mode==23){ //HUMAN single board (vecchio extrapoint)
					//mode=14 NPC single board switch player p[0]=normale VS player 2 p[1]=NPC
					//mode=23 HUMAN two board switch player dal handshake-> p[]?
					//elimina possibili scelte gialle
					/*
					if(mode==23 && player==playerMulti){ //è presente in riceviMossa()
						enable_RIT();
					}
					*/
			if(!changeMode){
					//TOKEN
				if(scelta.row==-1){
					//player rimane fermo
					//scelta=p[player];
					writeMove(White,Red);
				}else{
				for (i=0;i<6;i++){
					if(nextPosition[i].row!=-1){
						drawPlayer(nextPosition[i].row,nextPosition[i].col,Black);
					}
				}
				//elimina precedente posizione
				drawPlayer(p[player].row,p[player].col,Black);
				board[p[player].row][p[player].col]=0;
				//aggiungi la nuova posizione
				p[player].row=scelta.row;
				p[player].col=scelta.col;
				board[p[player].row][p[player].col]=2;
				//trasferisci nel registro
				//SE è TWO BOARD INVIO
				if(mode==23 && player==playerMulti){
					c=convertCoordToken(scelta.row,scelta.col);
					move=transfer(player,0,0,c.x,c.y);
					//invio mossa
					trasmettiMossa();
					//disabilito mossa
					disable_RIT();
					reset_RIT();
					disable_timer(0);
				}
			if(!player){
				//player 1
				drawPlayer(scelta.row,scelta.col,White);
			}else{
				drawPlayer(scelta.row,scelta.col,Red);
			}
			//reset scelta
			scelta.row=-1;
			//cambio turno se non ha vinto
			if(isDestination(p[0],p[1])){
				//VINTO
				GUI_Text(87, 239, (uint8_t *) "YOU WON!", Red, Yellow);
				disable_RIT();
				disable_timer(0);
			}else{
				player=(player)?0:1;
				if(mode==14 && player==1){
					//mode=14 NPC single board switch player p[1]=NPC player 2
					playNPC();
					player=(player)?0:1;
				}
				if(mode==13 || (mode==14 && player==0 && !isDestination(p[0],p[1])) || (mode==23 && player==playerMulti)){
					play();
				}
			}
		}
		}else{
			//MURI
			if(board[sceltaW.coord.row][sceltaW.coord.col]!=1){
				//se non c'è già un muro posizionato
				board[sceltaW.coord.row][sceltaW.coord.col]=1;
				if(sceltaW.direction){
					//verticale
					if(board[sceltaW.coord.row-1][sceltaW.coord.col]==3 ||board[sceltaW.coord.row+1][sceltaW.coord.col]==3){
						//messaggio muro già presente
						writeWallPresent(White,Red);
					}else{
						board[sceltaW.coord.row-1][sceltaW.coord.col]=3;
						board[sceltaW.coord.row+1][sceltaW.coord.col]=3;
						board[sceltaW.coord.row][sceltaW.coord.col]=1;
						//Controllo trappola
						if(checkTrapWall(p[0],0)&&checkTrapWall(p[1],1)){
							drawWall(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction,Magenta);
							//trasferisci nel registro
							if(mode==23 && player==playerMulti){
							c=convertCoordWall(sceltaW.coord.row,sceltaW.coord.col);
							move=transfer(player,1,!sceltaW.direction,c.x,c.y);
								//invio mossa
								trasmettiMossa();
								//disabilito mossa
								disable_RIT();
								reset_RIT();
								//reset timer
								disable_timer(0);
							}
							if(player){
								//player 2
								wallP2--;
								writeWallP2();
							}else{
								//player 1
								wallP1--;
								writeWallP1();
							}
							//cambio turno
							changeMode=0;
							player=(player)?0:1;
							if(mode==14 && player==1){
								//mode=14 NPC single board switch player p[1]=NPC player 2
								playNPC();
								player=(player)?0:1;
							}
							if(mode==13 || (mode==14 && player==0) || (mode==23 && player==playerMulti)){
								play();
							}
						}else{
							board[sceltaW.coord.row-1][sceltaW.coord.col]=0;
							board[sceltaW.coord.row+1][sceltaW.coord.col]=0;
							board[sceltaW.coord.row][sceltaW.coord.col]=0;
							//messaggio intrappolato
							writeWallTrap(White,Red);
						}
					}
				}else{
					//orizzontale
					if(board[sceltaW.coord.row][sceltaW.coord.col-1]==4 ||board[sceltaW.coord.row][sceltaW.coord.col+1]==4){
						//messaggio muro già presente
						writeWallPresent(White,Red);
					}else{
						board[sceltaW.coord.row][sceltaW.coord.col-1]=4;
						board[sceltaW.coord.row][sceltaW.coord.col+1]=4;
						board[sceltaW.coord.row][sceltaW.coord.col]=1;
						//Controllo trappola
						if(checkTrapWall(p[0],0) && checkTrapWall(p[1],1)){
							drawWall(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction,Magenta);
							if(mode==23 && player==playerMulti){
							c=convertCoordWall(sceltaW.coord.row,sceltaW.coord.col);
							move=transfer(player,1,!sceltaW.direction,c.x,c.y);
								//invio mossa
								trasmettiMossa();
								//disabilito mossa
								disable_RIT();
								reset_RIT();
								//reset timer
								disable_timer(0);
							}
							if(player){
								//player 2
								wallP2--;
								writeWallP2();
							}else{
								//player 1
								wallP1--;
								writeWallP1();
							}
							//cambio turno
							changeMode=0;
							player=(player)?0:1;
							if(mode==14 && player==1){
								//mode=14 NPC single board switch player p[1]=NPC player 2
								playNPC();
								player=(player)?0:1;
							}
							if(mode==13 || (mode==14 && player==0) || (mode==23 && player==playerMulti)){
								play();
							}
						}else{
							board[sceltaW.coord.row][sceltaW.coord.col-1]=0;
							board[sceltaW.coord.row][sceltaW.coord.col+1]=0;
							board[sceltaW.coord.row][sceltaW.coord.col]=0;
							//messaggio intrappolato
							writeWallTrap(White,Red);
						}
					}
				}
			}else{
				//messaggio muro già presente
				writeWallPresent(White,Red);
			}
	}

}
				if(mode==0){ //menu GAME MODE
					switch(sceltaMode){
						case 1: //scelta Single Board
							mode=sceltaMode;
							sceltaMode=-1;
						//disegno menu single board
						drawSingleBoard();
							break;
						case 2: //scelta Two Board
							//errore handshake
								//if(multiplayer())
							//box quadrati bianchi
							//messaggio errore
							//sceltaMode=-1
							//break;
							//else
							mode=sceltaMode;
							sceltaMode=-1;
						//chi preme INT0? primo giocatore
						//disegno menu two board
						drawTwoBoard();
							break;
						default:
							break;
					}
				}
				if(mode==2){  //Two board
					switch(sceltaMode){
						case 23: //HUMAM Two Board
							mode=sceltaMode;
							sceltaMode=-1;
							multiplayer();						
							break;
						case 24: //NPC Two Board
							mode=sceltaMode;
							sceltaMode=-1;
							multiplayer();	
							break;
						default:
							break;
					}
				}
				if(mode==1){
					switch(sceltaMode){
						case 13: //HUMAM Single Board
							mode=sceltaMode;
							sceltaMode=-1;
							changeMode=0;
							startGame();
						  play();
							break;
						case 14: //NPC Single Board
							mode=sceltaMode;
							sceltaMode=-1;
							changeMode=0;
							startGame();
						  play();
							break;
						default:
							break;
					}
				}
				break;
			default:
				break;
	}}
	else{
			J_select=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0 && (LPC_GPIO1->FIOPIN & (1<<27)) == 0 && diagonal==2){		//DOWN - LEFT
		/* Joytick J_Down pressed p1.26 --> using J_DOWN due to emulator issues*/
		//row+2
		J_down_left++;
		switch(J_down_left){
			case 1:
				if(!changeMode){
					//token
					checkScelta(5);
				}else{
					//wall
					if(isValidWall(sceltaW.coord.row+2,sceltaW.coord.col)){
						clearWallBoard(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction);
						sceltaW.coord.row+=2;
						drawWall(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction,Blue2);
					}
				}
				break;
			default:
				break;
		}
	}
	else{
			J_down_left=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0 && (LPC_GPIO1->FIOPIN & (1<<28)) == 0 && diagonal==2){		//DOWN - RIGHT
		/* Joytick J_Down pressed p1.26 --> using J_DOWN due to emulator issues*/
		//row+2
		J_down_right++;
		switch(J_down_right){
			case 1:
				if(!changeMode){
					//token
					checkScelta(4);
				}else{
					//wall
					if(isValidWall(sceltaW.coord.row+2,sceltaW.coord.col)){
						clearWallBoard(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction);
						sceltaW.coord.row+=2;
						drawWall(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction,Blue2);
					}
				}
				break;
			default:
				break;
		}
	}
	else{
			J_down_right=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0 ){		//DOWN
		/* Joytick J_Down pressed p1.26 --> using J_DOWN due to emulator issues*/
		//row+2
		J_down++;
		switch(J_down){
			case 2:
				if(mode==0){ //menu GAME MODE
					sceltaMode=2; //Two Board
					//box for text1
					drawBox(120,160,White);
					//box for text2
					drawBox(200,240,Red);
				}
				if(mode==1){ //menu Single Board
					sceltaMode=14; //NPC single Board
					//box for text1
					drawBox(120,160,White);
					//box for text2
					drawBox(200,240,Red);
				}
				if(mode==2){ //menu Two Board
					sceltaMode=24; //NPC Two Board
					//box for text1
					drawBox(120,160,White);
					//box for text2
					drawBox(200,240,Red);
				}
				if(mode==13 || mode==14 || mode==23 || mode==24){ //HUMAN single board
				if(!changeMode){
					//token
					if(J_left==0 && J_right==0){
						checkScelta(2);
					}
				}else{
					//wall
					if(isValidWall(sceltaW.coord.row+2,sceltaW.coord.col)){
						clearWallBoard(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction);
						sceltaW.coord.row+=2;
						drawWall(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction,Blue2);
					}
				}
			}
				break;
			default:
				break;
		}
	}
	else{
			J_down=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0 && (LPC_GPIO1->FIOPIN & (1<<26)) == 0 && diagonal==3){	  //LEFT - DOWN
		//col-2
		J_left_down++;
		switch(J_left_down){
			case 1:
				if(!changeMode){
					//token
					checkScelta(4);
				}else{
					//wall
					if(isValidWall(sceltaW.coord.row,sceltaW.coord.col-2)){
						clearWallBoard(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction);
						sceltaW.coord.col-=2;
						drawWall(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction,Blue2);
					}
				}
				break;
			default:
				break;
		}
	}
	else{
			J_left_down=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0 && (LPC_GPIO1->FIOPIN & (1<<29)) == 0 && diagonal==3){	  //LEFT - UP
		//col-2
		J_left_up++;
		switch(J_left_up){
			case 1:
				if(!changeMode){
					//token
					checkScelta(5);
				}else{
					//wall
					if(isValidWall(sceltaW.coord.row,sceltaW.coord.col-2)){
						clearWallBoard(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction);
						sceltaW.coord.col-=2;
						drawWall(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction,Blue2);
					}
				}
				break;
			default:
				break;
		}
	}
	else{
			J_left_up=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0 ){	  //LEFT
		//col-2
		J_left++;
		switch(J_left){
			case 2:
				if(!changeMode){
					//token
					if(J_down==0 && J_up==0){
						checkScelta(3);
					}
				}else{
					//wall
					if(isValidWall(sceltaW.coord.row,sceltaW.coord.col-2)){
						clearWallBoard(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction);
						sceltaW.coord.col-=2;
						drawWall(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction,Blue2);
					}
				}
				break;
			default:
				break;
		}
	}
	else{
			J_left=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0 && (LPC_GPIO1->FIOPIN & (1<<29)) == 0 && diagonal==1){	  //RIGHT - UP
		//col+2
		J_right_up++;
		switch(J_right_up){
			case 1:
				if(!changeMode){
					//token
					checkScelta(4);
				}else{
					//wall
					if(isValidWall(sceltaW.coord.row,sceltaW.coord.col+2)){
						clearWallBoard(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction);
						sceltaW.coord.col+=2;
						drawWall(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction,Blue2);
					}
				}
				break;
			default:
				break;
		}
	}
	else{
			J_right_up=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0 && (LPC_GPIO1->FIOPIN & (1<<26)) == 0 && diagonal==1){	  //RIGHT - DOWN
		//col+2
		J_right_down++;
		switch(J_right_down){
			case 1:
				if(!changeMode){
					//token
					checkScelta(5);
				}else{
					//wall
					if(isValidWall(sceltaW.coord.row,sceltaW.coord.col+2)){
						clearWallBoard(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction);
						sceltaW.coord.col+=2;
						drawWall(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction,Blue2);
					}
				}
				break;
			default:
				break;
		}
	}
	else{
			J_right_down=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	  //RIGHT
		//col+2
		J_right++;
		switch(J_right){
			case 2:
				if(!changeMode){
					//token
					if(J_down==0 && J_up==0){
						checkScelta(1);
					}
				}else{
					//wall
					if(isValidWall(sceltaW.coord.row,sceltaW.coord.col+2)){
						clearWallBoard(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction);
						sceltaW.coord.col+=2;
						drawWall(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction,Blue2);
					}
				}
				break;
			default:
				break;
		}
	}
	else{
			J_right=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0 && (LPC_GPIO1->FIOPIN & (1<<27)) == 0 && diagonal==0){	  //UP - LEFT
		//row-2
		J_up_left++;
		switch(J_up_left){
			case 1:
				if(!changeMode){
					//token
					checkScelta(4);
				}else{
					//wall
					if(isValidWall(sceltaW.coord.row-2,sceltaW.coord.col)){
						clearWallBoard(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction);
						sceltaW.coord.row-=2;
						drawWall(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction,Blue2);
					}
				}
				break;
			default:
				break;
		}
	}
	else{
			J_up_left=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0 && (LPC_GPIO1->FIOPIN & (1<<28)) == 0 && diagonal==0){	  //UP - RIGHT
		//row-2
		J_up_right++;
		switch(J_up_right){
			case 1:
				if(!changeMode){
					//token
					checkScelta(5);
				}else{
					//wall
					if(isValidWall(sceltaW.coord.row-2,sceltaW.coord.col)){
						clearWallBoard(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction);
						sceltaW.coord.row-=2;
						drawWall(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction,Blue2);
					}
				}
				break;
			default:
				break;
		}
	}
	else{
			J_up_right=0;
	}
	
if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	  //UP
		//row-2
		J_up++;
		switch(J_up){
			case 2:
				if(mode==0){ //menu GAME MODE
					sceltaMode=1; //Single Board
					//box for text1
					drawBox(120,160,Red);
					//box for text2
					drawBox(200,240,White);
				}
				if(mode==1){ //menu Single Board
					sceltaMode=13; //HUMAN Single Board
					//box for text1
					drawBox(120,160,Red);
					//box for text2
					drawBox(200,240,White);
				}
				if(mode==2){ //menu Two Board
					sceltaMode=23; //HUMAN Two Board
					//box for text1
					drawBox(120,160,Red);
					//box for text2
					drawBox(200,240,White);
				}
				if(mode==13 || mode==14 || mode==23 || mode==24){
				if(!changeMode){
					//token
					if(J_left==0 && J_right==0){
						checkScelta(0);
					}
				}else{
					//wall
					if(isValidWall(sceltaW.coord.row-2,sceltaW.coord.col)){
						clearWallBoard(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction);
						sceltaW.coord.row-=2;
						drawWall(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction,Blue2);
					}
				}
			}
				break;
			default:
				break;
		}
	}
	else{
			J_up=0;
	}
	
	
	/* button management */ 
		if(down_0!=0){  
			down_0 ++;  
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){ /* INT0 pressed */

			switch(down_0){
			case 2:
				if(mode==-1){ //menu Game Mode
					writePressINT0(Black,Black);
					mode=0;
					drawGameMode();
					playerMulti=2;
					//disabilito?
				}else{
					changeMode=0;
					//player?
					/*
				writePressINT0(Black,Black);
			//abilita key 1
				down_1=0;			
				NVIC_EnableIRQ(EINT1_IRQn);							 //ENABLE
				LPC_PINCON->PINSEL4    |= (1 << 22);    
					
			//modalità posiziona token
				changeMode=0;
					if(mode==23){
						player=playerMulti;
					}
				play();
					*/
				break;
				}
			default:
				break;
		}
					
	}
	else {	/* button released */
		down_0=0;	
		NVIC_EnableIRQ(EINT0_IRQn);		/* enable Button interrupts			 */
		LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */	
	}
	}
	if(down_1!=0){  
			down_1 ++;  
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){ /* KEY1 pressed */

			switch(down_1){
			case 2:
				changeMode=(changeMode)?0:1;
				if(!changeMode){
					//inserimento token
					//clearWall ultima selezione del muro 
					clearWallBoard(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction);
					//disabilita key 2
					NVIC_DisableIRQ(EINT2_IRQn);
					LPC_PINCON->PINSEL4    &= ~(1 << 24);     /* GPIO pin selection */
					LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */
					changeGame();
				}else{
					//inserimento wall
					//abilita key 2
					down_2=0;			
					NVIC_EnableIRQ(EINT2_IRQn); 							 /* enable Button interrupts			*/
					LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 2 pin selection */
					if(player){
						//player 2
						if(wallP2>0){
							initWall();
						}else{
							//messaggio muri finiti
							writeWallFinish(White,Red);
							changeMode=0;
							changeGame();
						}
					}else{
						//player 1
						if(wallP1>0){
							initWall();
						}else{
							//messaggio muri finiti
							writeWallFinish(White,Red);
							changeMode=0;
							changeGame();
						}
					}
				}
				break;
			default:				
				break;
		}
	}
	else {	/* button released */
		down_1=0;			
		NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
		LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 1 pin selection */
	}
	}
	if(down_2!=0){  
			down_2 ++;  
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){ /* KEY2 pressed */
			switch(down_2){
			case 2:
				clearWallBoard(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction);
				sceltaW.direction=(sceltaW.direction)?0:1;
				drawWall(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction,Blue2);
				break;
			default:
				if(down_2 % 8 == 0)
					{
						
					}					
			
				break;
		}
	}
	else {	/* button released */
		down_2=0;
		
		NVIC_EnableIRQ(EINT2_IRQn); 							 /* enable Button interrupts			*/
		LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 2 pin selection */
	}
	}
	//disable_RIT();
	//reset_RIT(); 
	//?	(07 sample button debouncing 2)
	LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
