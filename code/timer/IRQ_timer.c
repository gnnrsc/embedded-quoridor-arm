/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include "lpc17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h"
#include "../RIT/RIT.h"
#include "../quoridor/game.h"
#include "../quoridor/draw.h"
#include "../quoridor/wall.h"
#include <stdbool.h>

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
volatile uint8_t round_timer=TIME;
extern bool player;
extern bool changeMode; // 0=token 1=wall
extern Wall sceltaW;
extern uint8_t text;

extern uint32_t move;
extern int transfer(bool player, bool moveWall, bool direction, int y, int x);

extern int mode; //modalità di gioco
extern bool handshake;
extern int playerMulti; // 0=player1, 1=player2

void TIMER0_IRQHandler (void)
{
	if(round_timer==0){
		round_timer=TIME;
		if(mode==23 && player==playerMulti){
			move=transfer(player,0,1,0,0);
			//invio mossa
			trasmettiMossa();
			//disabilito mossa
			disable_RIT();
			reset_RIT();
			//reset timer
			disable_timer(0);
			}
		player=(player)?0:1;
		if(changeMode){
			//modalità muri
			clearWallBoard(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction);
			changeMode=0;
		}
		clearTokenBoard();
		if(mode==13 || mode==14 ||(mode==23 && player==playerMulti)){
				play();
			}
	}else{
		round_timer--;
		writeTime();
	}
	LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
	//Ogni 3 secondi
	switch(text){
		case 1:
			writeWallPresent(Black,Black);
			break;
		case 2:
			writeWallTrap(Black,Black);
			break;
		case 3:
			writeWallFinish(Black,Black);
			break;
		case 4:
			writeMove(Black,Black);
			break;
		case 5:
			retryConnection(Black,Black);
			disable_timer(1);
		  reset_timer(1);
			break;
		default:
			break;
	}
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

void TIMER2_IRQHandler (void)
{
	//Ogni 10 secondi azzera la connessione
	mode=0;
	drawGameMode();
	handshake=0;
	playerMulti=2;
	waitForConnection(Black,Black);
	retryConnection(White,Red);
	disable_timer(2);
	reset_timer(2);
	enable_RIT();
  LPC_TIM2->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
