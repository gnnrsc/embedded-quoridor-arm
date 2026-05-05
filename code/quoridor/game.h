#ifndef __GAME_H
#define __GAME_H
#include <stdbool.h>
#include "LPC17xx.h"
#include "../CAN/CAN.h"

#define ROWS 13
#define COLS 13
#define TIME 20

typedef struct {
    int row;
    int col;
} Point;

typedef struct {
	int x,y;
}coord;

extern void startGame(void);
extern void init_coord(void);
extern void initBoard(void);
extern void changeGame(void);
extern void play(void);
extern void clearTokenBoard(void);
bool isDestination(Point p1, Point p2);
extern void playNPC(void);
extern void multiplayer(void);
extern void trasmettiHandshake(void);
extern void riceviMossa(CAN_msg CAN_RxMsg);
extern void trasmettiMossa(void);

#endif
