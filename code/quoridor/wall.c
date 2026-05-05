#include "LPC17xx.h"
#include "../GLCD/GLCD.h" 
#include "../timer/timer.h"
#include "../RIT/RIT.h"
#include "game.h"
#include "draw.h"
#include "wall.h"
#include "npc.h"
#include <stdbool.h>

#define MAX_QUEUE_SIZE (ROWS * COLS)
extern Point p[2]; //posizione dei player
extern int lenght;
extern uint8_t wallP1;
extern uint8_t wallP2;
extern int mode; //modalità di gioco
extern uint32_t move;
extern int transfer(bool player, bool moveWall, bool direction, int y, int x);

typedef struct {
    Point position;
} QueueNode;

typedef struct {
    QueueNode array[MAX_QUEUE_SIZE];
    int front, rear;
} Queue;

extern uint16_t board[ROWS][COLS];
extern Wall sceltaW;

void clearWallBoard(int row,int col,int direction){
	drawWall(row,col,direction,Black);
	if(board[row][col]==1){
		//orizzontale
		if(board[row][col+1]==4&&board[row][col-1]==4){
			drawWall(row,col,0,Magenta);
		}else{
			//metà muro
			if(board[row][col+1]==4){ //destra
				drawWall(row,col+2,0,Magenta);
			}
			if(board[row][col-1]==4){ //sinistra
				drawWall(row,col-2,0,Magenta);
			}
		}
		//verticale
		if(board[row+1][col]==3&&board[row-1][col]==3){
			drawWall(row,col,1,Magenta);
		}else{
			//metà muro
			if(board[row-1][col]==3){ //sopra
				drawWall(row-2,col,1,Magenta);
			}
			if(board[row+1][col]==3){ //sotto
				drawWall(row+2,col,1,Magenta);
			}
		}
	}else{
		//metà muri
		if(board[row][col+1]==4){ //destra
				drawWall(row,col+2,0,Magenta);
			}
			if(board[row][col-1]==4){ //sinistra
				drawWall(row,col-2,0,Magenta);
			}
			if(board[row-1][col]==3){ //sopra
				drawWall(row-2,col,1,Magenta);
			}
			if(board[row+1][col]==3){ //sotto
				drawWall(row+2,col,1,Magenta);
			}
	}
}
bool isValidWall(int x, int y){
	return (x >= 1) && (x< ROWS) && (y >= 1) && (y < COLS);
}

void initWall(){
	clearTokenBoard();
	sceltaW.coord.row=5;
	sceltaW.coord.col=7;
	sceltaW.direction=0;
	drawWall(sceltaW.coord.row,sceltaW.coord.col,sceltaW.direction,Blue2);
	//draw wall dove c'è 1-3 in board (nelle coordinate/indici selezionati), per ripristinare i vecchi muri
}

bool wallNPC(int len0,int len1, Point position, bool player){
	//position dell'avversario p=0
	int i,max=-1,indice=-1;
	int len[8]={-1};
	coord c;
	//verticali
	//len[0] alto sx
	//len[1] alto dx
	//len[2] basso sx
	//len[3] basso dx
	//orizzontali
	//len[4] alto sx
	//len[5] alto dx
	//len[6] basso sx
	//len[7] basso dx
	if(!player){
		//player 2
		if(wallP2==0){
			return 0;
		}
		}else{
		//player 1
		if(wallP1==0){
			return 0;
		}
		}
	if(len0<len1){
		//provo a posizionare il muro
		//muro in alto sx
		if(isValidWall(position.row-1,position.col-1)){
			if(board[position.row-1][position.col-1]!=1){
				//se non c'è già un muro orizzontale 
				if(board[position.row-1][position.col-2]!=4 && board[position.row-1][position.col]!=4){
					board[position.row-1][position.col-2]=4;
					board[position.row-1][position.col-1]=1;
					board[position.row-1][position.col]=4;
					if(checkTrapWall(p[0],0) && checkTrapWall(p[1],1)){
						findFirstMove(p[player],player);
						len[4]=lenght;
					}
					board[position.row-1][position.col-2]=0;
					board[position.row-1][position.col-1]=0;
					board[position.row-1][position.col]=0;
				}
				//se non c'è già un muro verticale 
				if(board[position.row-2][position.col-1]!=3 && board[position.row][position.col-1]!=3){
					board[position.row-2][position.col-1]=3;
					board[position.row-1][position.col-1]=1;
					board[position.row][position.col-1]=3;
					if(checkTrapWall(p[0],0) && checkTrapWall(p[1],1)){
					findFirstMove(p[player],player);
					len[0]=lenght;
					}
					board[position.row-2][position.col-1]=0;
					board[position.row-1][position.col-1]=0;
					board[position.row][position.col-1]=0;
				}
			}
		}
		//muro in alto dx
		if(isValidWall(position.row-1,position.col+1)){
			if(board[position.row-1][position.col+1]!=1){
				//se non c'è già un muro orizzontale
				if(board[position.row-1][position.col+2]!=4 && board[position.row-1][position.col]!=4){
					board[position.row-1][position.col+2]=4;
					board[position.row-1][position.col+1]=1;
					board[position.row-1][position.col]=4;
					if(checkTrapWall(p[0],0) && checkTrapWall(p[1],1)){
					findFirstMove(p[player],player);
					len[5]=lenght;
					}
					board[position.row-1][position.col+2]=0;
					board[position.row-1][position.col+1]=0;
					board[position.row-1][position.col]=0;
				}
				//se non c'è già un muro verticale 
				if(board[position.row-2][position.col+1]!=3 && board[position.row][position.col+1]!=3){
					board[position.row-2][position.col+1]=3;
					board[position.row-1][position.col+1]=1;
					board[position.row][position.col+1]=3;
					if(checkTrapWall(p[0],0) && checkTrapWall(p[1],1)){
					findFirstMove(p[player],player);
					len[1]=lenght;
					}
					board[position.row-2][position.col+1]=0;
					board[position.row-1][position.col+1]=0;
					board[position.row][position.col+1]=0;
				}
			}
		}
		//muro in basso sx
		if(isValidWall(position.row+1,position.col-1)){
			if(board[position.row+1][position.col-1]!=1){
				//se non c'è già un muro orizzontale
				if(board[position.row+1][position.col-2]!=4 && board[position.row+1][position.col]!=4){
					board[position.row+1][position.col-2]=4;
					board[position.row+1][position.col-1]=1;
					board[position.row+1][position.col]=4;
					if(checkTrapWall(p[0],0) && checkTrapWall(p[1],1)){
					findFirstMove(p[player],player);
					len[6]=lenght;
					}
					board[position.row+1][position.col-2]=0;
					board[position.row+1][position.col-1]=0;
					board[position.row+1][position.col]=0;
				}
				//se non c'è già un muro verticale 
				if(board[position.row+2][position.col-1]!=3 && board[position.row][position.col-1]!=3){
					board[position.row+2][position.col-1]=3;
					board[position.row+1][position.col-1]=1;
					board[position.row][position.col-1]=3;
					if(checkTrapWall(p[0],0) && checkTrapWall(p[1],1)){
					findFirstMove(p[player],player);
					len[2]=lenght;
					}
					board[position.row+2][position.col-1]=0;
					board[position.row+1][position.col-1]=0;
					board[position.row][position.col-1]=0;
				}
			}
		}	
		//muro in basso dx
		if(isValidWall(position.row+1,position.col+1)){
			if(board[position.row+1][position.col+1]!=1){
				//se non c'è già un muro orizzontale
				if(board[position.row+1][position.col+2]!=4 && board[position.row+1][position.col]!=4){
					board[position.row+1][position.col+2]=4;
					board[position.row+1][position.col+1]=1;
					board[position.row+1][position.col]=4;
					if(checkTrapWall(p[0],0) && checkTrapWall(p[1],1)){
					findFirstMove(p[player],player);
					len[7]=lenght;
					}
					board[position.row+1][position.col+2]=0;
					board[position.row+1][position.col+1]=0;
					board[position.row+1][position.col]=0;
				}
				//se non c'è già un muro verticale 
				if(board[position.row+2][position.col+1]!=3 && board[position.row][position.col+1]!=3){
					board[position.row+2][position.col+1]=3;
					board[position.row+1][position.col+1]=1;
					board[position.row][position.col+1]=3;
					if(checkTrapWall(p[0],0) && checkTrapWall(p[1],1)){
					findFirstMove(p[player],player);
					len[3]=lenght;
					}
					board[position.row+2][position.col+1]=0;
					board[position.row+1][position.col+1]=0;
					board[position.row][position.col+1]=0;
				}
			}
		}
		//controllo lunghezza minore
		for(i=0;i<8;i++){
			if(len[i]!=-1){
				if (len[i] > max) {
            max = len[i];
					indice=i;
        }
			}
		}
		//verticali
	//len[0] alto sx
	//len[1] alto dx
	//len[2] basso sx
	//len[3] basso dx
	//orizzontali
	//len[4] alto sx
	//len[5] alto dx
	//len[6] basso sx
	//len[7] basso dx
		switch(indice){
			case 0:
				board[position.row-2][position.col-1]=3;
					board[position.row-1][position.col-1]=1;
					board[position.row][position.col-1]=3;
				if(checkTrapWall(p[0],0) && checkTrapWall(p[1],1)){
					c=convertCoordWall(position.row-1,position.col-1);
					move=transfer(!player,1,0,c.x,c.y);
				drawWall(position.row-1,position.col-1,1,Magenta);
				}else{
					board[position.row-2][position.col-1]=0;
					board[position.row-1][position.col-1]=0;
					board[position.row][position.col-1]=0;
					return 0;
				}
				break;
			case 1:
				board[position.row-2][position.col+1]=3;
					board[position.row-1][position.col+1]=1;
					board[position.row][position.col+1]=3;
				if(checkTrapWall(p[0],0) && checkTrapWall(p[1],1)){
					c=convertCoordWall(position.row-1,position.col+1);
					move=transfer(!player,1,0,c.x,c.y);
				drawWall(position.row-1,position.col+1,1,Magenta);
				}else{
					board[position.row-2][position.col+1]=0;
					board[position.row-1][position.col+1]=0;
					board[position.row][position.col+1]=0;
					return 0;
				}
				break;
			case 2:
				board[position.row+2][position.col-1]=3;
					board[position.row+1][position.col-1]=1;
					board[position.row][position.col-1]=3;
				if(checkTrapWall(p[0],0) && checkTrapWall(p[1],1)){
					c=convertCoordWall(position.row+1,position.col-1);
					move=transfer(!player,1,0,c.x,c.y);
				drawWall(position.row+1,position.col-1,1,Magenta);
				}else{
					board[position.row+2][position.col-1]=0;
					board[position.row+1][position.col-1]=0;
					board[position.row][position.col-1]=0;
					return 0;
				}
			
				break;
			case 3:
				board[position.row+2][position.col+1]=3;
					board[position.row+1][position.col+1]=1;
					board[position.row][position.col+1]=3;
				if(checkTrapWall(p[0],0) && checkTrapWall(p[1],1)){
					c=convertCoordWall(position.row+1,position.col+1);
					move=transfer(!player,1,0,c.x,c.y);
				drawWall(position.row+1,position.col+1,1,Magenta);
				}else{
					board[position.row+2][position.col+1]=0;
					board[position.row+1][position.col+1]=0;
					board[position.row][position.col+1]=0;
					return 0;
				}
			
				break;
			case 4:
				board[position.row-1][position.col-2]=4;
					board[position.row-1][position.col-1]=1;
					board[position.row-1][position.col]=4;
				if(checkTrapWall(p[0],0) && checkTrapWall(p[1],1)){
					c=convertCoordWall(position.row-1,position.col-1);
					move=transfer(!player,1,1,c.x,c.y);
				drawWall(position.row-1,position.col-1,0,Magenta);
				}else{
					board[position.row-1][position.col-2]=0;
					board[position.row-1][position.col-1]=0;
					board[position.row-1][position.col]=0;
					return 0;
				}
			
				break;
			case 5:
				board[position.row-1][position.col+2]=4;
					board[position.row-1][position.col+1]=1;
					board[position.row-1][position.col]=4;
				if(checkTrapWall(p[0],0) && checkTrapWall(p[1],1)){
					c=convertCoordWall(position.row-1,position.col+1);
					move=transfer(!player,1,1,c.x,c.y);
				drawWall(position.row-1,position.col+1,0,Magenta);
				}else{
					board[position.row-1][position.col+2]=0;
					board[position.row-1][position.col+1]=0;
					board[position.row-1][position.col]=0;
					return 0;
				}
			
				break;
			case 6:
				board[position.row+1][position.col-2]=4;
					board[position.row+1][position.col-1]=1;
					board[position.row+1][position.col]=4;
				if(checkTrapWall(p[0],0) && checkTrapWall(p[1],1)){
					c=convertCoordWall(position.row+1,position.col-1);
					move=transfer(!player,1,1,c.x,c.y);
				drawWall(position.row+1,position.col-1,0,Magenta);
				}else{
					board[position.row+1][position.col-2]=0;
					board[position.row+1][position.col-1]=0;
					board[position.row+1][position.col]=0;
					return 0;
				}
			
				break;
			case 7:
				board[position.row+1][position.col+2]=4;
					board[position.row+1][position.col+1]=1;
					board[position.row+1][position.col]=4;
				if(checkTrapWall(p[0],0) && checkTrapWall(p[1],1)){
					c=convertCoordWall(position.row+1,position.col+1);
					move=transfer(!player,1,1,c.x,c.y);
				drawWall(position.row+1,position.col+1,0,Magenta);
				}else{
					board[position.row+1][position.col+2]=0;
					board[position.row+1][position.col+1]=0;
					board[position.row+1][position.col]=0;
					return 0;
				}
			
				break;
			case -1:
				//non si può piazzare il muro
				return 0;
				break;
			default:
				break;
		}
		if(mode==24){
								//invio mossa
								trasmettiMossa();
								//disabilito mossa
								disable_RIT();
								reset_RIT();
								//reset timer
								disable_timer(0);
							}
		if(!player){
		//player 2
		wallP2--;
		writeWallP2();
		}else{
		//player 1
		wallP1--;
		writeWallP1();
		}
		return 1;
	}else{
		return 0;
	}
}

void initQueue(Queue* queue) {
    queue->front = queue->rear = -1;
}

bool isEmpty(Queue* queue) {
    return queue->front == -1;
}

void enqueue(Queue* queue, QueueNode item) {

    if (isEmpty(queue))
        queue->front = 0;

    queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;
    queue->array[queue->rear] = item;
}

QueueNode dequeue(Queue* queue) {
		QueueNode emptyNode;
	
		QueueNode item;
    if (isEmpty(queue)) {
			// Invalid point
			emptyNode.position.row=-1;
			emptyNode.position.col=-1;
        return emptyNode;
    }

    item = queue->array[queue->front];
    if (queue->front == queue->rear)
        initQueue(queue);
    else
        queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;

    return item;
}

bool isInFrontW(uint16_t board[ROWS][COLS], Point pos){
	return board[pos.row][pos.col] == 2;
}

bool isValidW(int row, int col) {
    return (row >= 0) && (row < ROWS) && (col >= 0) && (col < COLS);
}

bool isDestinationW(Point dest,bool player) {
	if(player){
		//player 2
		return dest.row==12;
	}else{
		//player 1
		return dest.row == 0;
	}
}

bool isBlockedW(uint16_t board[ROWS][COLS], Point pos) {
    return (board[pos.row][pos.col] == 1 || board[pos.row][pos.col]==3 || board[pos.row][pos.col]==4); // 1 o 3 o 4 è il muro
}
coord diagonalIndexW(int i){
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
bool checkTrapWall(Point start,bool player){
	bool visited[ROWS][COLS];
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
	
    for (i = 0; i < ROWS; i++)
        for (j = 0; j < COLS; j++)
            visited[i][j] = false;

    visited[start.row][start.col] = true;

    
    initQueue(&queue);

		startNode.position.row=start.row;
		startNode.position.col=start.col;
    enqueue(&queue, startNode);

    while (!isEmpty(&queue)) {
        currentNode = dequeue(&queue);

        currentPos = currentNode.position;

        // Controlla se la destinazione è stata raggiunta
        if (isDestinationW(currentPos,player))
            return true;

        // Prova a muoversi in tutte le 4 direzioni (up, down, left, right)

        for (i = 0; i < 4; i++) {
            next.row = currentPos.row + row_moves[i];
            next.col = currentPos.col + col_moves[i];

            wallNext.row = currentPos.row + row_moves_w[i];
            wallNext.col = currentPos.col + col_moves_w[i];
            
            if(isValidWall(wallNext.row, wallNext.col) && isInFrontW((board),next)&& !isBlockedW(board, wallNext)){
            	next.row = next.row + row_moves[i];
            	next.col = next.col + col_moves[i];

            	wallNext.row += row_moves[i];
            	wallNext.col += col_moves[i];
							if (isValidW(next.row, next.col)){
								if(isValidWall(wallNext.row, wallNext.col) && !isBlockedW(board,wallNext)&& !visited[next.row][next.col]){
									visited[next.row][next.col] = true;
									adjacentNode.position.row=next.row;
									adjacentNode.position.col=next.col;
									enqueue(&queue, adjacentNode);
								}else{
									//bloccato->diagonali
									sx=diagonalIndexW(i).x;
									dx=diagonalIndexW(i).y;
									//faccio mosse a sx
									wallNext.row = wallNext.row - row_moves_w[i]+row_moves_w[sx];
									wallNext.col = wallNext.col - col_moves_w[i]+col_moves_w[sx];
									next.row = next.row - row_moves[i] + row_moves[sx];
									next.col = next.col - col_moves[i] + col_moves[sx];
									if(isValidW(next.row, next.col) && isValidWall(wallNext.row, wallNext.col) && !isBlockedW(board,wallNext)&& !visited[next.row][next.col]){
									visited[next.row][next.col] = true;
									adjacentNode.position.row=next.row;
									adjacentNode.position.col=next.col;
									enqueue(&queue, adjacentNode);
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
									if(isValidW(next.row, next.col) && isValidWall(wallNext.row, wallNext.col) && !isBlockedW(board,wallNext)&& !visited[next.row][next.col]){
									visited[next.row][next.col] = true;
									adjacentNode.position.row=next.row;
									adjacentNode.position.col=next.col;
									enqueue(&queue, adjacentNode);
									}
							}
						}
        }else{
					 if (isValidW(next.row, next.col) && !visited[next.row][next.col] && !isBlockedW(board, wallNext)) {
                visited[next.row][next.col] = true;
								adjacentNode.position.row=next.row;
								adjacentNode.position.col=next.col;
                enqueue(&queue, adjacentNode);
            }
				}
    }
	}
    return false; // La destinazione non può essere raggiunta
	
}
