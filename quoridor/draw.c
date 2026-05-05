
#include "LPC17xx.h"
#include "../GLCD/GLCD.h" 
#include "../timer/timer.h"
#include "game.h"
#include "draw.h"
#include "stdio.h"

extern uint8_t round_timer; 
extern uint8_t wallP1; 
extern uint8_t wallP2; 

extern coord token[7][7]; 
extern coord wall[6][6]; 

//1 muri 0 spazi liberi 2 pedine
extern uint16_t board[ROWS][COLS];

volatile uint8_t text;

coord convertCoordWall(int x, int y){
	//dagli indici della board 13x13 -> alle coordinate del disegno del wall 6x6
	coord c;
	c.x=(x-1)/2;
	c.y=(y-1)/2;
	return c;
}

//indici x e y dispari fino a 11 (compreso)
void drawWall(int x, int y,bool direction,uint16_t Color){
	int i,coordX,coordY;
	if(direction){
		//verticale
		//X=5px 	Y=60px	da colorare
		coord c=convertCoordWall(x,y);
		coordX=wall[c.x][c.y].x;
		coordY=wall[c.x][c.y].y-28;
	
		for(i=0;i<5;i++){
			LCD_DrawLine(coordX+i, coordY,coordX+i,coordY+60, Color);
		}
	}else{
		//orizzontale
		//X=60px 	Y=5px	da colorare
		coord c=convertCoordWall(x,y);
		coordX=wall[c.x][c.y].x-28;
		coordY=wall[c.x][c.y].y;
		
		for(i=0;i<5;i++){
			LCD_DrawLine(coordX, coordY+i,coordX+60,coordY+i, Color);
		}
	}
}

coord convertCoordToken(int x, int y){
	//dagli indici della board 13x13 -> alle coordinate del disegno del token 7x7
	coord c;
	c.x=x/2;
	c.y=y/2;
	return c;
}

coord convertTo13Token(int x, int y){
	//dagli indici del token 7x7  -> alle coordinate del disegno board 13x13
	coord c;
	c.x=x*2;
	c.y=y*2;
	return c;
}

coord convertTo13Wall(int x, int y){
	//dagli indici del token 6x6  -> alle coordinate del disegno board 13x13
	coord c;
	c.x=(x*2)+1;
	c.y=(y*2)+1;
	return c;
}

void writeSingleBoard(uint16_t ColorText){
	GUI_Text(72, 134, (uint8_t *) "Single Board", ColorText, Black);
}

void writeTwoBoard(uint16_t ColorText){
	GUI_Text(80, 214, (uint8_t *) "Two Board", ColorText, Black);
}

void writeHuman(uint16_t ColorText){
	GUI_Text(98, 134, (uint8_t *) "Human", ColorText, Black);
}

void writeNPC(uint16_t ColorText){
	GUI_Text(108, 214, (uint8_t *) "NPC", ColorText, Black);
}

void drawGameMode(){
	GUI_Text(33, 59, (uint8_t *) "Select the GAME MODE", White, Black);
	//box for text1
	drawBox(120,160,White);
	writeSingleBoard(White);
	//box for text2
	drawBox(200,240,White);
	writeTwoBoard(White);
}

void drawSingleBoard(){
	LCD_Clear(Black);
	GUI_Text(43, 59, (uint8_t *) "Single Board: select", White, Black);
	GUI_Text(45, 75, (uint8_t *) "the opposite player", White, Black);
	//box for text1
	drawBox(120,160,White);
	writeHuman(White);
	//box for text2
	drawBox(200,240,White);
	writeNPC(White);
}

void drawTwoBoard(){
	LCD_Clear(Black);
	GUI_Text(53, 59, (uint8_t *) "Two Board: select", White, Black);
	GUI_Text(70, 75, (uint8_t *) "your player", White, Black);
	//box for text1
	drawBox(120,160,White);
	writeHuman(White);
	//box for text2
	drawBox(200,240,White);
	writeNPC(White);
}

void drawBox(int y1,int y2,uint16_t Color){ //y->altezza
	//120 - 160 box1
	//200 - 240 box2
	//linee orizzontali
		LCD_DrawLine(60, y1, 180,y1 , Color);
		LCD_DrawLine(60, y2, 180,y2 , Color);
	//linee verticali
		LCD_DrawLine(60, y1,60,y2, Color);
		LCD_DrawLine(180, y1,180,y2 , Color);
}
void drawPlayer(int x, int y,uint16_t Color){
	//prima casella in x=7 e y=7
	//si avanza di 33
	int i,coordX,coordY;
	//26px da colorare
	coord c=convertCoordToken(x,y);
	coordX=token[c.x][c.y].x;
	coordY=token[c.x][c.y].y;
	
	for(i=0;i<26;i++){
		//LCD_DrawLine(x, y+i,x+25,y+i, Color);
		LCD_DrawLine(coordX, coordY+i,coordX+25,coordY+i, Color);
	}
}

void drawPlayerMultiplayer(int x, int y,uint16_t Color){
	//da 7x7
	int i,coordX,coordY;
	coordX=token[x][y].x;
	coordY=token[x][y].y;
	
	for(i=0;i<26;i++){
		//LCD_DrawLine(x, y+i,x+25,y+i, Color);
		LCD_DrawLine(coordX, coordY+i,coordX+25,coordY+i, Color);
	}
}

void drawGrid(){
	//caselle da 27px spazio di 6px
	int i,j;
	//prime orizzontali
	for(j=0;j<7;j++){
		for(i=0;i<7;i++){
		LCD_DrawLine(6+(33*i), 6+(33*j), 33*(i+1),6+(33*j) , White);
		}
	}
	//seconde orizzontali
	for(j=0;j<7;j++){
		for(i=0;i<7;i++){
		LCD_DrawLine(6+(33*i), 33+(33*j), 33*(i+1),33+(33*j) , White);
		}
	}
	//prime verticali
	for(j=0;j<7;j++){
		for(i=0;i<7;i++){
		LCD_DrawLine(6+(33*j), 33*(i+1), 6+(33*j),6+(33*i) , White);
		}
	}
	//seconde verticali
	for(j=0;j<7;j++){
		for(i=0;i<7;i++){
		LCD_DrawLine(33*(j+1), 6+(33*i), 33*(j+1),33*(i+1) , White);
		}
	}
	//box for text
	//linee orizzontali
		for(i=0;i<3;i++){
		LCD_DrawLine(6+(i*78), 262, 78*(i+1),262 , White);
		LCD_DrawLine(6+(i*78), 310, 78*(i+1),310 , White);
		}
	
	//linee verticali
		for(i=0;i<3;i++){
			LCD_DrawLine(6+(i*78), 262,6+(i*78),310 , White);
			LCD_DrawLine(78*(i+1), 262,78*(i+1),310 , White);
		}
}
void writeWallP1(){
	char number[5] = "";
	sprintf(number,"%d",wallP1);
	GUI_Text(16, 272, (uint8_t *) "P1 Wall", White, Black);
	GUI_Text(36, 288, (uint8_t *) number, White, Black);
}
void writeTime(){
	char timer[5] = "";
	sprintf(timer,"%d s ",round_timer);
	GUI_Text(110, 280, (uint8_t *) timer, White, Black);
}

void writeWallP2(){
	char number[5] = "";
	sprintf(number,"%d",wallP2);
	GUI_Text(170, 272, (uint8_t *) "P2 Wall", White, Black);
	GUI_Text(190, 288, (uint8_t *) number, White, Black);
}
void writePressINT0(uint16_t TextColor,uint16_t BackgroundColor){
	GUI_Text(43, 239, (uint8_t *) "Press INT0 to start", TextColor, BackgroundColor);
}
void writeWallPresent(uint16_t TextColor,uint16_t BackgroundColor){
	enable_timer(1);
	text=1;
	GUI_Text(0, 239, (uint8_t *) "Wall already present,try again", TextColor, BackgroundColor);
}
void writeWallTrap(uint16_t TextColor,uint16_t BackgroundColor){
	enable_timer(1);
	text=2;
	GUI_Text(10, 239, (uint8_t *) "Position not permitted,trap", TextColor, BackgroundColor);
}
void writeWallFinish(uint16_t TextColor,uint16_t BackgroundColor){
	enable_timer(1);
	text=3;
	GUI_Text(4, 239, (uint8_t *) "Walls finished,move the token", TextColor, BackgroundColor);
}
void writeMove(uint16_t TextColor,uint16_t BackgroundColor){
	enable_timer(1);
	text=4;
	GUI_Text(0, 239, (uint8_t *) "Move the token or place a wall", TextColor, BackgroundColor);
}
void retryConnection(uint16_t TextColor,uint16_t BackgroundColor){
	enable_timer(1);
	text=5;
	GUI_Text(43, 250, (uint8_t *) ":( retry connection", TextColor, BackgroundColor);
}
void waitForConnection(uint16_t TextColor,uint16_t BackgroundColor){
	GUI_Text(35, 250, (uint8_t *) "Wait for connection...", TextColor, BackgroundColor);
}
