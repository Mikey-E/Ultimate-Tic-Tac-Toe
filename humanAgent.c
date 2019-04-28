#include "core.h"

void playHumanMove(char** board, int* dest, int minRow, int maxRow, int minCol, int maxCol, char myChar){
	char row;
	char col;
	char currChar;

	printf("Enter numbers for row & col: ");
	while ((currChar = fgetc(stdin)) != '\n'){
		row = col;
		col = currChar;
	}
	
	//compensate for ascii
	row = row-48;
	col = col-48;

	if ((row < 1) ||
		(row > 9) ||
		(col < 1) ||
		(col > 9)
		){
		printf("The two numbers must each be in the range of 1-9 inclusive. Ex: \"53\" is row 5, col 3.\n");
		playHumanMove(board, dest, minRow, maxRow, minCol, maxCol, myChar);
	}else{
		dest[0] = row;
		dest[1] = col;
	}
}
