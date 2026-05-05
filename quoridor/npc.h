#ifndef __NPC_H
#define __NPC_H
#include "lpc17xx.h"
#include <stdbool.h>
#include "game.h"

//extern void clearWallBoard(int row,int col,int direction);
//extern bool isValidWall(int x, int y);
//extern void initWall(void);
extern Point findFirstMove(Point start,bool player);
#endif
