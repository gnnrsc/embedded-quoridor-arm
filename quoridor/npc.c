#include "LPC17xx.h"
#include "../GLCD/GLCD.h" 
#include "../timer/timer.h"
#include "game.h"
#include "draw.h"
#include "wall.h"
#include "npc.h"
#include <stdbool.h>

#define MAX_QUEUE_SIZE (ROWS * COLS)
volatile int lenght;

typedef struct {
    Point position;
} QueueNode;

typedef struct {
    QueueNode array[MAX_QUEUE_SIZE];
    int front, rear;
} Queue;

extern uint16_t board[ROWS][COLS];
extern Wall sceltaW;

bool isValidWallNPC(int x, int y){
	return (x >= 1) && (x< ROWS) && (y >= 1) && (y < COLS);
}

void initQueueNPC(Queue* queue) {
    queue->front = queue->rear = -1;
}

bool isEmptyNPC(Queue* queue) {
    return queue->front == -1;
}

void enqueueNPC(Queue* queue, QueueNode item) {

    if (isEmptyNPC(queue))
        queue->front = 0;

    queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;
    queue->array[queue->rear] = item;
}

QueueNode dequeueNPC(Queue* queue) {
		QueueNode emptyNode;
	
		QueueNode item;
    if (isEmptyNPC(queue)) {
			// Invalid point
			emptyNode.position.row=-1;
			emptyNode.position.col=-1;
        return emptyNode;
    }

    item = queue->array[queue->front];
    if (queue->front == queue->rear)
        initQueueNPC(queue);
    else
        queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;

    return item;
}

bool isInFrontNPC(uint16_t board[ROWS][COLS], Point pos){
	return board[pos.row][pos.col] == 2;
}

bool isValidNPC(int row, int col) {
    return (row >= 0) && (row < ROWS) && (col >= 0) && (col < COLS);
}

bool isDestinationNPC(Point dest,bool player) {
	if(player){
		//player 2
		return dest.row==12;
	}else{
		//player 1
		return dest.row == 0;
	}
}

bool isBlockedNPC(uint16_t board[ROWS][COLS], Point pos) {
    return (board[pos.row][pos.col] == 1 || board[pos.row][pos.col]==3 || board[pos.row][pos.col]==4); // 1 o 3 o 4 è il muro
}
coord diagonalIndexNPC(int i){
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
Point findFirstMove(Point start,bool player){
  bool visited[ROWS][COLS];
  Point predecessor[ROWS][COLS];
    uint16_t i, j,sx,dx;
    Point next;
    Point wallNext;
    QueueNode startNode;
    QueueNode currentNode;
    Point currentPos;
    QueueNode adjacentNode;
    int row_moves[] = {-2, 2, 0, 0};
    int col_moves[] = {0, 0, -2, 2};
    int row_moves_w[] = {-1, 1, 0, 0};
    int col_moves_w[] = {0, 0, -1, 1};
    Queue queue;
    next.row=-1;
    next.col=-1;
		lenght=0;
  
    for (i = 0; i < ROWS; i++)
        for (j = 0; j < COLS; j++)
            visited[i][j] = false;
    
    for (i = 0; i < ROWS; i++)
        for (j = 0; j < COLS; j++)
            predecessor[i][j] = next;

    visited[start.row][start.col] = true;
    

    
    initQueueNPC(&queue);

    startNode.position.row=start.row;
    startNode.position.col=start.col;
    enqueueNPC(&queue, startNode);

    while (!isEmptyNPC(&queue)) {
        currentNode = dequeueNPC(&queue);

        currentPos = currentNode.position;

        // Controlla se la destinazione è stata raggiunta
        if (isDestinationNPC(currentPos,player)){
          //start posizione iniziale
          //ritornare prima mossa migliore
          while(predecessor[currentPos.row][currentPos.col].row!=start.row || predecessor[currentPos.row][currentPos.col].col!=start.col){
            currentPos=predecessor[currentPos.row][currentPos.col];
						lenght++;
          }
          return currentPos;
        }
            

        // Prova a muoversi in tutte le 4 direzioni (up, down, left, right)

        for (i = 0; i < 4; i++) {
            next.row = currentPos.row + row_moves[i];
            next.col = currentPos.col + col_moves[i];

            wallNext.row = currentPos.row + row_moves_w[i];
            wallNext.col = currentPos.col + col_moves_w[i];
            
            if(isValidWall(wallNext.row, wallNext.col) && isInFrontNPC((board),next)&& !isBlockedNPC(board, wallNext)){
              next.row = next.row + row_moves[i];
              next.col = next.col + col_moves[i];

              wallNext.row += row_moves[i];
              wallNext.col += col_moves[i];
              if (isValidNPC(next.row, next.col)){
                if(isValidWall(wallNext.row, wallNext.col) && !isBlockedNPC(board,wallNext)&& !visited[next.row][next.col]){
                  visited[next.row][next.col] = true;
                  predecessor[next.row][next.col] = currentPos;
                  adjacentNode.position.row=next.row;
                  adjacentNode.position.col=next.col;
                  enqueueNPC(&queue, adjacentNode);
                }else{
                  //bloccato->diagonali
                  sx=diagonalIndexNPC(i).x;
                  dx=diagonalIndexNPC(i).y;
                  //faccio mosse a sx
                  wallNext.row = wallNext.row - row_moves_w[i]+row_moves_w[sx];
                  wallNext.col = wallNext.col - col_moves_w[i]+col_moves_w[sx];
                  next.row = next.row - row_moves[i] + row_moves[sx];
                  next.col = next.col - col_moves[i] + col_moves[sx];
                  if(isValidNPC(next.row, next.col) && isValidWall(wallNext.row, wallNext.col) && !isBlockedNPC(board,wallNext)&& !visited[next.row][next.col]){
                  visited[next.row][next.col] = true;
                  predecessor[next.row][next.col] = currentPos;
                  adjacentNode.position.row=next.row;
                  adjacentNode.position.col=next.col;
                  enqueueNPC(&queue, adjacentNode);
                  }
                  //cancello mosse a sx
                  wallNext.row = wallNext.row + row_moves_w[i]-row_moves_w[sx];
                  wallNext.col = wallNext.col + col_moves_w[i]-col_moves_w[sx];
                  next.row = next.row + row_moves[i] - row_moves[sx];
                  next.col = next.col + col_moves[i] - col_moves[sx];
                  //faccio mosse a dx
                  wallNext.row = wallNext.row - row_moves_w[i]+row_moves_w[dx];
                  wallNext.col = wallNext.col - col_moves_w[i]+col_moves_w[dx];
                  next.row = next.row - row_moves[i] + row_moves[dx];
                  next.col = next.col - col_moves[i] + col_moves[dx];

								if(isValidNPC(next.row, next.col) && isValidWall(wallNext.row, wallNext.col) && !isBlockedNPC(board,wallNext)&& !visited[next.row][next.col]){
                  visited[next.row][next.col] = true;
                  predecessor[next.row][next.col] = currentPos;
                  adjacentNode.position.row=next.row;
                  adjacentNode.position.col=next.col;
                  enqueueNPC(&queue, adjacentNode);
                  }
              }
            }
        }else{
           if (isValidNPC(next.row, next.col) && !visited[next.row][next.col] && !isBlockedNPC(board, wallNext)) {
                visited[next.row][next.col] = true;
                predecessor[next.row][next.col] = currentPos;
                adjacentNode.position.row=next.row;
                adjacentNode.position.col=next.col;
                enqueueNPC(&queue, adjacentNode);
            }
        }
    }
  }
    next.row=-1;
    next.col=-1;
    return next; // La destinazione non può essere raggiunta
}


