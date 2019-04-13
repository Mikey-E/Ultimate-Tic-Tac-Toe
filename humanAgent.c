#include "core.h"

void playHumanMove(char** board, int* dest, int minRow, int maxRow, int minCol, int maxCol, char myChar){
#ifdef SLEEP
	sleep(SLEEP);
#endif
	char row;
	char col;
	printf("Enter numbers for row & col: ");
	row = fgetc(stdin);
	col = fgetc(stdin);
	fflush(stdin);

	//compensate for ascii
	row = row-48;
	col = col-48;

	if ((row < 1) ||
		(row > 9) ||
		(col < 1) ||
		(col > 9)
		){
		printf("numbers must be in the range of 1-9 inclusive\n");
		playHumanMove(board, dest, minRow, maxRow, minCol, maxCol, myChar);
	}else{
		dest[0] = row;
		dest[1] = col;
	}
}