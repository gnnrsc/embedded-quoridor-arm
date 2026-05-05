#ifndef __WALL_H
#define __WALL_H
#include "lpc17xx.h"
#include <stdbool.h>
#include "game.h"

typedef struct {
    Point coord;
    bool direction;
} Wall;

extern void clearWallBoard(int row,int col,int direction);
extern bool isValidWall(int x, int y);
extern void initWall(void);
extern bool checkTrapWall(Point start,bool player);
extern bool wallNPC(int len0,int len1, Point position, bool player);
#endif
