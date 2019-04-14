#ifndef MOVES_H
#define MOVES_H

//Declare move functions for all agents
void playHumanMove(char** board, int* dest, int minRow, int maxRow, int minCol, int maxCol, char myChar);
void playRandomMove(char** board, int* dest, int minRow, int maxRow, int minCol, int maxCol, char myChar);
void playMCTSMove(char** board, int* dest, int minRow, int maxRow, int minCol, int maxCol, char myChar);

#endif