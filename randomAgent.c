#include "core.h"

void playRandomMove(char** board, int* dest, int minRow, int maxRow, int minCol, int maxCol, char myChar){
	int row = rand()%9+1;
	int col = rand()%9+1;
	while ((row < minRow) || (row > maxRow) || (col < minCol) || (col > maxCol) || (board[row-1][col-1] != '\0')){
		row = rand()%9+1;
		col = rand()%9+1;
	}
	dest[0] = row;
	dest[1] = col;
}