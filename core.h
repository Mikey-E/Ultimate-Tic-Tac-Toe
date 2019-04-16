#ifndef CORE_H
#define CORE_H

#include <stdio.h>
#include <windows.h>//not needed for compiling on linux

//function declarations
void printBoard(char** board, int maxRow, int maxCol);
void setBoard(char** board, char c, int row, int col);
char queryBoard(char** board, int row, int col);
void convertSectorToIndexes(int sector, int* rowIndex, int* colIndex);
char examineSectorForWinner(char** board, int sector);
int updateBoardStatus(char** board);
int sumEmptySpaces(char** board);
void setMoveFunction(void (**fPtrPtr)(char**, int*, int, int, int, int, char));
void updateBounds(int rowMove, int colMove, char** board, int* minRow, int* maxRow, int* minCol, int* maxCol, int* isFullSector);
void freeSmallSectorBoard(char** board);
void freeBoard(char** board);
char** initBoard(int size);
char** copyBoard(char** board, int size);

// -- SETTINGS --
#define PLAYER1CHAR 'X'
#define PLAYER2CHAR 'O'
#define DEBUG//optional
#define SLEEP 0//optional (seconds) slows down the pace of the game
#define SRAND//optional, seeds random number generator based on current time
//#define SEED_RNG//disable for testing with same RNG sequence

#endif