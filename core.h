#ifndef CORE_H
#define CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#ifdef WINDOWS
#include <windows.h>//causes trouble for compiling on linux
#endif

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
void setMoveFunction(void (**fPtrPtr)(char**, int*, int, int, int, int, char), char agentChar, int moveType, int depth);
void updateBounds(int rowMove, int colMove, char** board, int* minRow, int* maxRow, int* minCol, int* maxCol, int* isFullSector);
void updateSmallSectorBoard(char** smallSectorBoard, int sector, char updateChar);
void logAgentInfo(FILE* fp, char agentChar);

#ifdef WINDOWS
//Causes linking errors on Linux to have this in the header file
int agentTypeHashTable[256];//for storing information about what type each agent is
#endif

// -- SETTINGS --
#define PLAYER1CHAR 'X'
#define PLAYER2CHAR 'O'
#define TIMESTAMP_SPACE 25
//#define DEBUG//optional

#endif
