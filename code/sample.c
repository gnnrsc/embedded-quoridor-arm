/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "button_EINT/button.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "CAN/CAN.h"
#include "joystick/joystick.h"
#include "quoridor/game.h"
#include "quoridor/draw.h"

#define SIMULATOR 1

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif



int main(void)
{
	SystemInit();  												/* System Initialization (i.e., PLL)  */
	CAN_Init();
  LCD_Initialization();
	BUTTON_init();												/* BUTTON Initialization              */
	joystick_init();											/* Joystick Initialization            */
	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       	*/
	enable_RIT();													/* RIT enabled												*/

		//disabilita key 1
	NVIC_DisableIRQ(EINT1_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 22);     /* GPIO pin selection */
	LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt         */
	//disabilit key 2
	NVIC_DisableIRQ(EINT2_IRQn);
	LPC_PINCON->PINSEL4    &= ~(1 << 24);     /* GPIO pin selection */
	LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */
	
	//0x004C4B40 50ms
	LCD_Clear(Black);
	writePressINT0(White,Blue);
	//startGame();
	//menu();
	
	// 20 secondi 0x1DCD6500
	init_timer(0, 0x017D7840 ); //1 secondo
	init_timer(1, 0x047868C0 ); //3 secondi
	init_timer(2, 0x17D78400 ); //10 secondi
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
	
  while (1)	
  {
		__ASM("wfi");
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
