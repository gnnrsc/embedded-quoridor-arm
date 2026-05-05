#ifndef __DRAW_H
#define __DRAW_H
#include "lpc17xx.h"
#include <stdbool.h>
#include "game.h"

extern void writeWallP1(void);
extern void writeTime(void);
extern void writeWallP2(void);
extern void drawGrid(void);
extern void drawPlayer(int x, int y, uint16_t Color);
extern void drawWall(int x, int y,bool direction,uint16_t Color);
extern void writePressINT0(uint16_t TextColor,uint16_t BackgroundColor);
extern void writeWallPresent(uint16_t TextColor,uint16_t BackgroundColor);
extern void writeWallTrap(uint16_t TextColor,uint16_t BackgroundColor);
extern void writeWallFinish(uint16_t TextColor,uint16_t BackgroundColor);
extern void writeMove(uint16_t TextColor,uint16_t BackgroundColor);
extern void retryConnection(uint16_t TextColor,uint16_t BackgroundColor);
extern void waitForConnection(uint16_t TextColor,uint16_t BackgroundColor);
coord convertCoordToken(int x, int y);
coord convertCoordWall(int x, int y);
extern void drawGameMode(void);
extern void drawSingleBoard(void);
extern void drawTwoBoard(void);
extern void drawBox(int y1,int y2,uint16_t Color);
extern void drawPlayerMultiplayer(int x, int y,uint16_t Color);
extern coord convertTo13Token(int x, int y);
extern coord convertTo13Wall(int x, int y);

#endif
