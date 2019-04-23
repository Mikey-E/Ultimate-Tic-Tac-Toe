#ifndef CORE_H
#define CORE_H

#include <stdio.h>
#include <unistd.h>
#include <windows.h>//not needed for compiling on linux

//function declarations
char** initBoard(int size);
char** copyBoard(char** board, int size);
void freeBoard(char** board);
void freeSmallSectorBoard(char** board);
void printBoard(char** board, int maxRow, int maxCol);
void setBoard(char** board, char c, int row, int col);
char queryBoard(char** board, int row, int col);
void convertSectorToIndexes(int sector, int* rowIndex, int* colIndex);
char examineSectorForWinner(char** board, int sector);
int updateBoardStatus(char** board);
int sumEmptySpaces(char** board);
void setMoveFunction(void (**fPtrPtr)(char**, int*, int, int, int, int, char));
void updateBounds(int rowMove, int colMove, char** board, int* minRow, int* maxRow, int* minCol, int* maxCol, int* isFullSector);
void updateSmallSectorBoard(char** smallSectorBoard, int sector, char updateChar);

// -- SETTINGS --
#define PLAYER1CHAR 'X'
#define PLAYER2CHAR 'O'
//#define DEBUG//optional
#define SRAND//optional, seeds random number generator based on current time. disable for testing with same RNG sequence

#endif