//Author: Michael Elgin
//Game core for ultimate tic tac toe
//Contains functions to run the game, compute winner, ask agents for moves, etc

#include "core.h"
#include "moves.h"
#include "MCTS.h"

//A sector is 1-9 inclusive (left->right, top->down)

//initializes a board and returns the pointer to it
char** initBoard(int size){
	char** board;
	board = malloc(size * sizeof(char*));
	int i;
	for (i = 0; i < size; i++){
		board[i] = (char*) calloc(size, sizeof(char));
	}
	return board;
}

//creates a copy of an existing board and returns the pointer to it
char** copyBoard(char** board, int size){
	char** newBoard = initBoard(size);
	int i; int j;
	for (i = 0; i < size; i++){
		for (j = 0; j < size; j++){
			newBoard[i][j] = board[i][j];
		}
	}
	return newBoard;
}

//frees all dynamically allocated memory associated with a board
void freeBoard(char** board){
	int i;
	for (i = 0; i < 9; i++){
		free(board[i]);
	}
	free(board);
}

//frees all dynamically allocated memory associated with a smallSectorBoard
void freeSmallSectorBoard(char** board){
	int i;
	for (i = 0; i < 3; i++){
		free(board[i]);
	}
	free(board);
}

//Prints the board out in a "pretty" way
//-1 for maxRow and maxCol ensure a normal board
void printBoard(char** board, int maxRow, int maxCol){
	int i; int j;
	printf("\n   1--2--3---4--5--6---7--8--9");
	char normalLine[100] = "\n -------------------------------\n";
	char midLine[100];
	if ((maxRow == -1) && (maxCol == -1)){
		sprintf(midLine, "\n -^^^^^^^^^^^^^^^^^^^^^^^^^^^^^-\n");
	}else{
		sprintf(midLine, "\n -------------------------------\n");
	}
	char midLineWithHats[100];
	if (maxCol == 3){
		sprintf(midLineWithHats, "\n -^^^^^^^^^---------------------\n");
	}else if(maxCol == 6){
		sprintf(midLineWithHats, "\n -----------^^^^^^^^^-----------\n");
	}else if(maxCol == 9){
		sprintf(midLineWithHats, "\n ---------------------^^^^^^^^^-\n");
	}
	for (i = 0; i < 9; i++){
		if ((i % 3) == 0){
			if (i == 0){
				printf(normalLine);
			}else{
				printf(i == maxRow ? midLineWithHats : midLine);
			}
		} else {
			printf("\n");
		}
		printf("%d", i+1);
		for (j = 0; j < 9; j++){
			if ((j % 3) == 0){printf("|");}
			printf(" %c ", board[i][j]);
			if ((j + 1) == 9){printf("|");}
		}
	}
	printf(maxRow == 9 ? midLineWithHats : midLine);
}

void setBoard(char** board, char c, int row, int col){
	board[row-1][col-1] = c;
}

char queryBoard(char** board, int row, int col){
	return board[row-1][col-1];
}

//Sets rowIndex and colIndex pointers to starting positions of the sector
//Needed before calling some other functions
void convertSectorToIndexes(int sector, int* rowIndex, int* colIndex){
	if (sector == 1){*rowIndex = 0; *colIndex = 0;}
	else if (sector == 2){*rowIndex = 0; *colIndex = 3;}
	else if (sector == 3){*rowIndex = 0; *colIndex = 6;}
	else if (sector == 4){*rowIndex = 3; *colIndex = 0;}
	else if (sector == 5){*rowIndex = 3; *colIndex = 3;}
	else if (sector == 6){*rowIndex = 3; *colIndex = 6;}
	else if (sector == 7){*rowIndex = 6; *colIndex = 0;}
	else if (sector == 8){*rowIndex = 6; *colIndex = 3;}
	else if (sector == 9){*rowIndex = 6; *colIndex = 6;}
}

//Examines a sector and determines if there is any winner
//Returns null terminator \0 for no winner or already won, or char of winner that needs to be updated
char examineSectorForWinner(char** board, int sector){
	//assign indexes based on sector
	int i;
	int rowIndex, colIndex;
	convertSectorToIndexes(sector, &rowIndex, &colIndex);
	//check if already won
	if (	(board[rowIndex][colIndex] == board[rowIndex][colIndex + 1])		&&
			(board[rowIndex][colIndex] == board[rowIndex][colIndex + 2])		&&
			(board[rowIndex][colIndex] == board[rowIndex + 1][colIndex])		&&
			(board[rowIndex][colIndex] == board[rowIndex + 1][colIndex + 1])	&&
			(board[rowIndex][colIndex] == board[rowIndex + 1][colIndex + 2])	&&
			(board[rowIndex][colIndex] == board[rowIndex + 2][colIndex])		&&
			(board[rowIndex][colIndex] == board[rowIndex + 2][colIndex + 1])	&&
			(board[rowIndex][colIndex] == board[rowIndex + 2][colIndex + 2])	){
		return '\0';
	}
	//check diagonal
	if (	(board[rowIndex][colIndex] == board[rowIndex + 1][colIndex + 1])	&&
			(board[rowIndex][colIndex] == board[rowIndex + 2][colIndex + 2])){
		return board[rowIndex][colIndex];
	}
	if (	(board[rowIndex + 2][colIndex] == board[rowIndex + 1][colIndex + 1])	&&
			(board[rowIndex + 2][colIndex] == board[rowIndex][colIndex + 2])){
		return board[rowIndex + 2][colIndex];
	}
	//check horizontal
	for (i = 0; i < 3; i++){
		if (	(board[rowIndex + i][colIndex] != '\0') &&
				(board[rowIndex + i][colIndex] == board[rowIndex + i][colIndex + 1])	&&
				(board[rowIndex + i][colIndex] == board[rowIndex + i][colIndex + 2])){
			return board[rowIndex + i][colIndex];
		}
	}
	//check vertical
	for (i = 0; i < 3; i++){
		if (	(board[rowIndex][colIndex + i] != '\0') &&
				(board[rowIndex][colIndex + i] == board[rowIndex + 1][colIndex + i])	&&
				(board[rowIndex][colIndex + i] == board[rowIndex + 2][colIndex + i])){
			return board[rowIndex][colIndex + i];
		}
	}	
	return '\0';//if none of the above returned, no winner
}

//Examines all 9 sectors, if the sector is won,
//all spots in that sector will be given to the winner
//Returns sector where that happened, or 0 if it did not happen
int updateBoardStatus(char** board){
	char sectorWinner;
	int rowIndex, colIndex;
	int i, j, k;
	for (i = 1; i <= 9; i++){
		sectorWinner = examineSectorForWinner(board, i);
		if (sectorWinner != '\0'){
			convertSectorToIndexes(i, &rowIndex, &colIndex);
			for (j = 0; j < 3; j++){
				for (k = 0; k < 3; k++){
					board[rowIndex + j][colIndex + k] = sectorWinner;
				}
			}
			return i;
		}
	}
	return 0;
}

int sumEmptySpaces(char** board){
	int sum = 0;
	int i, j;
	for (i = 0; i < 9; i++){
		for (j = 0; j < 9; j++){
			if (board[i][j] == '\0'){sum++;}
		}
	}
	return sum;
}

//sets the move function for an agent
//moveType == 0 means no move type yet, get user input.
//depth is for MCTS agents (3) only.
void setMoveFunction(void (**fPtrPtr)(char**, int*, int, int, int, int, char), char agentChar, int moveType, int depth){
	if (moveType){//already given move type
		agentTypeHashTable[agentChar] = moveType;
		if (moveType == 1){
			*fPtrPtr = &playHumanMove;
		}else if (moveType == 2){
			*fPtrPtr = &playRandomMove;
		}else if (moveType == 3){
			*fPtrPtr = &playMCTSMove;
			if (depth < 2){//should never happen, but just in case
				depth = 2;
			}
			if (depth > 9){//should never happen, but just in case
				depth = 9;
			}
			depthHashTable[agentChar] = (int) depth;//each MCTS agent's move function will retrieve its depth
		}
	}else{//get user input
		char agentType;
		do{
			agentType = fgetc(stdin);
			agentType = agentType - 48;//compensates for ascii
			fflush(stdin);
		}while ((agentType < 1) || (agentType > 3));//update if more agents are added in the future
			printf("Setting agent type as %d.\n", agentType);
			agentTypeHashTable[agentChar] = (int) agentType;
		if (agentType == 1){
			*fPtrPtr = &playHumanMove;
		}else if (agentType == 2){
			*fPtrPtr = &playRandomMove;
		}else if (agentType == 3){
			*fPtrPtr = &playMCTSMove;
			char searchDepth;
			printf("What search depth should this MCTS agent have (2 to 9)?\n");
			printf("2 = easiest (minimum)\n");
			printf("3 = easy\n");
			printf("4 = medium\n");
			printf("5 = challenging\n");
			printf("6+ = very difficult (and very slow to make moves)\n");
			printf("Enter search Depth: ");
			do{
				searchDepth = fgetc(stdin);
				searchDepth = searchDepth - 48;//compensates for ascii
				fflush(stdin);
			}while ((searchDepth < 2) || (searchDepth > 9));
			//ALWAYS keep depth as at least 2.
			//0 wouldn't make any sense, and also probably cause seg fault
			//1 leaves tree with nothing to compare with after opponent's move, also seg fault.
			printf("Setting search depth as %d.\n", searchDepth);
			depthHashTable[agentChar] = (int) searchDepth;//each MCTS agent's move function will retrieve this depth
		}
	}
}

//takes a board and most recent move and updates the values for boundaries
void updateBounds(int rowMove, int colMove, char** board, int* minRow, int* maxRow, int* minCol, int* maxCol, int* isFullSector){
	int rowMod = (rowMove-1) % 3;
	int colMod = (colMove-1) % 3;
	*minRow = 1 + (rowMod * 3);
	*maxRow = 3 + (rowMod * 3);
	*minCol = 1 + (colMod * 3);
	*maxCol = 3 + (colMod * 3);
	//check that there is an empty space to be played within the min/max bounds
	*isFullSector = 1;//assume full until a space is found
	int i; int j;
	for (i = *minRow; i <= *maxRow; i++){
		for (j = *minCol; j <= *maxCol; j++){
			if (queryBoard(board, i, j) == '\0'){
				*isFullSector = 0;
				break;
			}
		}
	}
	if (*isFullSector){//play anywhere
		*minRow = 1;
		*maxRow = 9;
		*minCol = 1;
		*maxCol = 9;
	}
}

void updateSmallSectorBoard(char** smallSectorBoard, int sector, char updateChar){
	int* rowIndex = malloc(sizeof(int));
	int* colIndex = malloc(sizeof(int));
	convertSectorToIndexes(sector, rowIndex, colIndex);
	//compensate for smaller board
	*rowIndex = *rowIndex / 3;
	*colIndex = *colIndex / 3;
	setBoard(smallSectorBoard, updateChar, *rowIndex + 1, *colIndex + 1);
	free(rowIndex);
	free(colIndex);
}

//writes agent info to file
//opening and closing FILE* is responsibility of caller. FILE* should already be open
void logAgentInfo(FILE* fp, char agentChar){
	char typeStr[18];//adjust size as needed
	if (agentTypeHashTable[agentChar] == 1){
		sprintf(typeStr, "Human");
	}else if (agentTypeHashTable[agentChar] == 2){
		sprintf(typeStr, "Random Move Maker");
	}else if (agentTypeHashTable[agentChar] == 3){
		sprintf(typeStr, "MCTS (depth %d)", depthHashTable[agentChar]);
	}
	fprintf(fp, "Player %c: Type: %s\n", agentChar, typeStr);
}

//Runs the program, calls upon agents to give moves to the board
int main(int argc, char** argv){
	//variables to be possibly changed by command line flags, with default values
	int isQuiet = 0;//decides to whether to show board output or not
	int sleepSeconds = 1;
	int batchCount = 1;//amount of games to play in a batch
	time_t rngSeed = 0;//can set a specific random number seed [1, 2^32 - 1]. 0 means base on current time
	char* outputFileStr = malloc(12 * sizeof(char));//enough space for history.txt
	strcpy(outputFileStr, "history.txt");//where to append results of a game played
	void (*agentFunction1)(char**, int*, int, int, int, int, char) = NULL;//called for moves
	void (*agentFunction2)(char**, int*, int, int, int, int, char) = NULL;//"...
	
	int option;
	while ((option = getopt(argc, argv, "1:2:f:g:hqr:s:")) != -1){
		switch(option){
			case '1'://first agent type
				setMoveFunction(&agentFunction1, PLAYER1CHAR, optarg[0] - 48, optarg[1] - 48);//-48 for ascii
				break;
			case '2'://second agent type
				setMoveFunction(&agentFunction2, PLAYER2CHAR, optarg[0] - 48, optarg[1] - 48);//"...
				break;
			case 'f'://custom results file
				free(outputFileStr);
				outputFileStr = malloc(strlen(optarg) + sizeof(char));//+1 char for null terminator
				strcpy(outputFileStr, optarg);
				break;
			case 'g'://amount of games to play
				batchCount = atoi(optarg);
				break;
			case 'h'://help
				printf("Rules:\n");
				printf("Conquer 3 sectors just like in normal tic-tac-toe to achieve victory.\n");
				printf("Each sector is its own tic-tac-toe game.\n");
				printf("Where you play in each sector corresponds to the next sector that your\n");
				printf("opponent has to play in.\n");
				printf("If that sector is full or conquered, then your opponent may play anywhere.\n");
				printf("Enter row and col as numbers together. Ex: row 7, col 1 is \"71\", then enter.\n");
				return(0);
			case 'q'://quiet
				isQuiet = 1;//true
				break;
			case 'r'://specific random number seed
				rngSeed = atoi(optarg);
				break;
			case 's'://seconds of sleep between each round
				sleepSeconds = atoi(optarg);
				break;
			default:
				printf("See readme.txt for info on flags.\n");
				return(0);
		}
	}
	//other local variables
	time_t t;//used for file timestamps
	time_t startTime;
	time_t endTime;
	time_t batchStartTime;
	time_t batchEndTime;
	char startTimestamp[TIMESTAMP_SPACE];
	char endTimestamp[TIMESTAMP_SPACE];
	char batchStartTimestamp[TIMESTAMP_SPACE];
	char batchEndTimestamp[TIMESTAMP_SPACE];
	int i;
	char** board;
	char** smallSectorBoard;
	int rowIndex, colIndex;//for helper functions that need them
	int rowMove, colMove;//assigned from move info that an agent fills out
	int* moves;//where each agent sets its desired move
	int minRow; int maxRow; int minCol; int maxCol;//boundaries
	int turn;//turn of player 1 or player 2
	int isFullSector;
	int changedSector;
	char currChar;
	char potentialWinner;//for checking if overall game has been won each round
	FILE* resultsFilePtr;
	int totalP1Wins, totalP2Wins, totalTies;

	//start counts fresh
	totalP1Wins = 0; totalP2Wins = 0; totalTies = 0;

	printf("\nUltimate Tic-Tac-Toe\n");//initial message

	//Select what agent to play against
	if ((agentFunction1 == NULL) || (agentFunction2 == NULL)){
		printf("1 = Human\n");
		printf("2 = Random Move Maker (very easy)\n");
		printf("3 = Monte Carlo Tree Search\n");
	}
	if (agentFunction1 == NULL){
		printf("What type of player is player 1: ");
		setMoveFunction(&agentFunction1, PLAYER1CHAR, 0, 0);//no agent type given yet, n/a search depth
	}
	if (agentFunction2 == NULL){
		printf("What type of player is player 2: ");
		setMoveFunction(&agentFunction2, PLAYER2CHAR, 0, 0);//"...
	}

	moves = calloc(2, sizeof(int));//enough space for a row and col number

	resultsFilePtr = fopen(outputFileStr, "a");//append mode
	if (resultsFilePtr == NULL){
		printf("Unable to open %s\n", outputFileStr);
	}

	//log start of batch
	time(&batchStartTime);
	strcpy(batchStartTimestamp, ctime(&batchStartTime));
	fprintf(resultsFilePtr, "==============================\n");
	fprintf(resultsFilePtr, "Batch of %d games starting at %s", batchCount, batchStartTimestamp);
	logAgentInfo(resultsFilePtr, PLAYER1CHAR);
	logAgentInfo(resultsFilePtr, PLAYER2CHAR);

	//batch starts
	while (batchCount-- > 0){
		//individual game starts
		fprintf(resultsFilePtr, "---Game---\n");
		if (!rngSeed){//use current time
			rngSeed = time(&t);
		}
		srand(rngSeed);
		//appending some information early (such as rng seed) can help with debugging in gdb if there is a game crash.
		fprintf(resultsFilePtr, "Random Number Seed: %d\n", rngSeed);
		rngSeed++;//increment for potential next game

		//append some info for output file
		time(&startTime);//get time game started
		strcpy(startTimestamp, ctime(&startTime));
		fprintf(resultsFilePtr, "Started:\t%s", startTimestamp);
		fclose(resultsFilePtr);//write
		resultsFilePtr = fopen(outputFileStr, "a");//reopen

		//assign values needed at beginning of game
		board = initBoard(9);
		smallSectorBoard = initBoard(3);//for keeping track of won sectors, and therefore computing winner
		minRow = 1; maxRow = 9;
		minCol = 1; maxCol = 9;
		turn = 1;//player 1's turn
		potentialWinner = '\0';

		if (!isQuiet){
			printBoard(board, -1, -1);//initial empty board
		}

		while(1){//game begins
			turn == 1 ? (*agentFunction1)(board, moves, minRow, maxRow, minCol, maxCol, PLAYER1CHAR)://for true
						(*agentFunction2)(board, moves, minRow, maxRow, minCol, maxCol, PLAYER2CHAR);//for false
			rowMove = moves[0];
			colMove = moves[1];
			//check validity of move
			if (	(rowMove < minRow) ||
					(rowMove > maxRow) ||
					(colMove < minCol) ||
					(colMove > maxCol)
					){
				printf("For this move the row must be between %d and %d inclusive,\n", minRow, maxRow);
				printf("and the col must be between %d and %d inclusive.\n", minCol, maxCol);
			}else if (queryBoard(board, rowMove, colMove) != '\0'){
				printf("That spot is already taken.\n");
			}else{//acceptable move
				currChar = (turn == 1 ? PLAYER1CHAR : PLAYER2CHAR);
				setBoard(board, currChar, rowMove, colMove);
				changedSector = updateBoardStatus(board);
				if (changedSector){//update the small board
					updateSmallSectorBoard(smallSectorBoard, changedSector, currChar);
					potentialWinner = examineSectorForWinner(smallSectorBoard, 1);
					if (potentialWinner != '\0'){//we have a winner
						if (!isQuiet){
							printBoard(board, -1, -1);
						}
						printf("%c Wins!\n", potentialWinner);
						if (potentialWinner == PLAYER1CHAR){
							totalP1Wins++;
						}else{
							totalP2Wins++;
						}
						goto cleanUpMemory;
					}
				}
				if (sumEmptySpaces(board) == 0){//draw
					if (!isQuiet){
						printBoard(board, -1, -1);
					}
					printf("Tie!\n");
					totalTies++;
					goto cleanUpMemory;
				}
				updateBounds(rowMove, colMove, board, &minRow, &maxRow, &minCol, &maxCol, &isFullSector);
				if (!isQuiet){
					if (isFullSector){//play anywhere
						printBoard(board, -1, -1);
					}else{
						printBoard(board, maxRow, maxCol);
					}
				}
				
#ifdef DEBUG
				//see status of small sector board for won sectors
				printf("[%d][%d][%d]\n", smallSectorBoard[0][0], smallSectorBoard[0][1], smallSectorBoard[0][2]);
				printf("[%d][%d][%d]\n", smallSectorBoard[1][0], smallSectorBoard[1][1], smallSectorBoard[1][2]);
				printf("[%d][%d][%d]\n", smallSectorBoard[2][0], smallSectorBoard[2][1], smallSectorBoard[2][2]);
#endif
				if (turn == 1) {turn = 2;} else {turn = 1;}
			}
			sleep(sleepSeconds);
		}
cleanUpMemory:
		freeBoard(board);
		freeSmallSectorBoard(smallSectorBoard);
		if (treeHashTable[PLAYER1CHAR]){
			freeTree(treeHashTable[PLAYER1CHAR]);
			treeHashTable[PLAYER1CHAR] = 0;
		}
		if (treeHashTable[PLAYER2CHAR]){
			freeTree(treeHashTable[PLAYER2CHAR]);
			treeHashTable[PLAYER2CHAR] = 0;
		}

		//append more results to file
		time(&endTime);//get time game finished
		strcpy(endTimestamp, ctime(&endTime));	
		fprintf(resultsFilePtr, "Ended:\t\t%s", endTimestamp);//two tabs center start and end over each other
		fprintf(resultsFilePtr, "Game lasted: %0.0f seconds.\n", difftime(endTime, startTime));
		if (potentialWinner != '\0'){
			fprintf(resultsFilePtr, "Winner: %c\n", potentialWinner);
		}else{
			fprintf(resultsFilePtr, "Winner: tie\n");
		}
		fprintf(resultsFilePtr, "----------\n");
	}
	//log info of the conclusion of the batch
	time(&batchEndTime);
	strcpy(batchEndTimestamp, ctime(&batchEndTime));
	fprintf(resultsFilePtr, "Batch ended at %s", batchEndTimestamp);
	fprintf(resultsFilePtr, "Batch lasted: %0.0f seconds.\n", difftime(batchEndTime, batchStartTime));
	fprintf(resultsFilePtr, "Total %c Wins: %d\n", PLAYER1CHAR, totalP1Wins);
	fprintf(resultsFilePtr, "Total %c Wins: %d\n", PLAYER2CHAR, totalP2Wins);
	fprintf(resultsFilePtr, "Total ties: %d\n", totalTies);
	fprintf(resultsFilePtr, "==============================\n");

	//final odds and ends
	fclose(resultsFilePtr);
	free(outputFileStr);
	free(moves);
	return 0;//Success.
}