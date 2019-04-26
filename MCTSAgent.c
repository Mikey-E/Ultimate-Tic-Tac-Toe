#include "core.h"
#include "MCTS.h"
#include "moves.h"

//frees all dynamically allocated memory associated with a node
void freeNode(node* node){
	freeBoard(node->board);
	freeSmallSectorBoard(node->smallSectorBoard);
	free(node->children);
	free(node);
}

//recursively frees all dynamically allocated memory associated with a tree.
void freeTree(node* root){
	int i;
	for (i = 0; i < root->childCount; i++){
		freeTree(root->children[i]);
	}
	freeNode(root);
}

//returns amount of positions that are different between 2 boards,
//like strcmp but for uttt boards.
//will update pointers to index of last compared different spot
int compareBoard(char** board1, char** board2, int* diffRowIndex, int* diffColIndex){
	int differences = 0;
	int i; int j;
	for (i = 0; i < 9; i++){
		for (j = 0; j < 9; j++){
			if (board1[i][j] != board2[i][j]){
				differences++;
				*diffRowIndex = i;
				*diffColIndex = j;
			}
		}
	}
	return differences;
}

//generates children for a node. Should only be used on child-less nodes.
void getSuccessors(node* root){
	//generate a child for every empty space within the bounds
	int i; int j;
	int* newMinRow = malloc(sizeof(int));
	int* newMaxRow = malloc(sizeof(int));
	int* newMinCol = malloc(sizeof(int));
	int* newMaxCol = malloc(sizeof(int));
	int* isFullSector = malloc(sizeof(int));
	int* changedSectorRowIndex = malloc(sizeof(int));
	int* changedSectorColIndex = malloc(sizeof(int));
	int changedSector = 0;
	node* newChild;
	//calcuate space needed (worst case) for how many children there will be
	root->children = calloc(((root->maxRow - (root->minRow - 1))*(root->maxCol - (root->minCol - 1))), sizeof(node*));
	for (i = root->minRow; i <= root->maxRow; i++){
		for (j = root->minCol; j <= root->maxCol; j++){
			if (queryBoard(root->board, i, j) == '\0'){//then this is a potential move, so create a child node
				newChild = calloc(1, sizeof(node));
				newChild->board = copyBoard(root->board, 9);
				newChild->smallSectorBoard = copyBoard(root->smallSectorBoard, 3);
				setBoard(newChild->board, root->myChar, i, j);
				newChild->recentRow = i;
				newChild->recentCol = j;
				changedSector = updateBoardStatus(newChild->board);
				if (changedSector){
					updateSmallSectorBoard(newChild->smallSectorBoard, changedSector, root->myChar);
				}
				newChild->parent = root;
				newChild->children = NULL;
				newChild->childCount = 0;
				newChild->sims = 0;
				newChild->wins = 0;
				updateBounds(i, j, newChild->board, newMinRow, newMaxRow, newMinCol, newMaxCol, isFullSector);
				newChild->minRow = *newMinRow;
				newChild->maxRow = *newMaxRow;
				newChild->minCol = *newMinCol;
				newChild->maxCol = *newMaxCol;
				newChild->myChar = root->myChar == PLAYER1CHAR ? PLAYER2CHAR : PLAYER1CHAR;
				root->children[root->childCount] = newChild;
				root->childCount++;
			}
		}
	}
	free(newMinRow);
	free(newMaxRow);
	free(newMinCol);
	free(newMaxCol);
	free(isFullSector);
	free(changedSectorRowIndex);
	free(changedSectorColIndex);
}

void updateTree(node* root, int maxDepth){
	int i;
	if (root->childCount > 0){//if it already has children, move down the tree
		for (i = 0; i < root->childCount; i++){
			updateTree(root->children[i], maxDepth - 1);
		}
	}else{
		if (maxDepth <= 0){//if at depth limit, play random simulation & backpropagate data up tree
			char result = playRandomSimulation(root);
			do{
				if (result == root->myChar){//win
					root->wins++;
					root->sims++;
				}else if (result == '\0'){//draw
					root->wins += 0.5;
					root->sims++;
				}else{//loss
					root->sims++;
				}
				root = root->parent;
			}while (root != NULL);

		}else{//create new nodes
			getSuccessors(root);
			for (i = 0; i < root->childCount; i++){
				updateTree(root->children[i], maxDepth - 1);
			}
		}
	}
}

char playRandomSimulation(node* root){
	int* moves = calloc(2, sizeof(int));
	int turn = root->myChar == PLAYER1CHAR ? 1 : 2;
	char** board = copyBoard(root->board, 9);
	char** smallSectorBoard = copyBoard(root->smallSectorBoard, 3);
	int minRow = root->minRow;
	int maxRow = root->maxRow;
	int minCol = root->minCol;
	int maxCol = root->maxCol;
	int isFullSector;//only used for updateBounds call
	int changedSector;
	char currChar;
	char potentialWinner = '\0';//for checking if overall game has been won each round
	while (1){
		if (sumEmptySpaces(board) == 0){//draw
			//potentialWinner = '\0';//represents draw, this statement likely not needed anymore.
			break;
		}
		turn == 1 ? playRandomMove(board, moves, minRow, maxRow, minCol, maxCol, PLAYER1CHAR)://for true
					playRandomMove(board, moves, minRow, maxRow, minCol, maxCol, PLAYER2CHAR);
		currChar = (turn == 1 ? PLAYER1CHAR : PLAYER2CHAR);
		setBoard(board, currChar, moves[0], moves[1]);
		changedSector = updateBoardStatus(board);
		if (changedSector){
			updateSmallSectorBoard(smallSectorBoard, changedSector, currChar);
			potentialWinner = examineSectorForWinner(smallSectorBoard, 1);
			if (potentialWinner != '\0'){//we have a winner
				break;
			}
		}
		updateBounds(moves[0], moves[1], board, &minRow, &maxRow, &minCol, &maxCol, &isFullSector);
		if (turn == 1) {turn = 2;} else {turn = 1;}
	}
	freeBoard(board);
	freeSmallSectorBoard(smallSectorBoard);
	free(moves);
	return potentialWinner;
}

void playMCTSMove(char** board, int* dest, int minRow, int maxRow, int minCol, int maxCol, char myChar){
	int searchDepth = depthHashTable[myChar];//get this agent's depth
	if (searchDepth >= 5){//it will take a bit of time
		printf("Thinking...\n");
	}
	int i;
	node* root;
	node* newRoot;
	int* altDest = malloc(2 * sizeof(int));
	if (treeHashTable[myChar] == 0){//create root based on current state of the board
		root = calloc(1, sizeof(node));
		root->board = copyBoard(board, 9);
		root->smallSectorBoard = initBoard(3);
		root->parent = NULL;
		root->children = NULL;
		root->childCount = 0;
		root->sims = 0;
		root->wins = 0;
		root->minRow = minRow;
		root->maxRow = maxRow;
		root->minCol = minCol;
		root->maxCol = maxCol;
		root->myChar = myChar;
	}else{//set root node to where it was before
		root = treeHashTable[myChar];
		//see which move opponent made and update root to point to that.
		for (i = 0; i < root->childCount; i++){
			if ((compareBoard(board, root->children[i]->board, altDest, altDest) == 0) &&
				(root->children[i]->minRow == minRow)	&&
				(root->children[i]->maxRow == maxRow)	&&
				(root->children[i]->minCol == minCol)	&&
				(root->children[i]->maxCol == maxCol)){
				newRoot = root->children[i];
			}else{
				freeTree(root->children[i]);
			}
		}
		freeNode(root);
		root = newRoot;
		root->parent = NULL;
	}
	//update tree for choosing a good move
	updateTree(root, searchDepth);
	//select best move based on updated tree
	int indexOfBest = 0;
	float bestRatio = 1;
	float currRatio;
	for (i = 0; i < root->childCount; i++){
		currRatio = root->children[i]->sims != 0 ? (root->children[i]->wins)/(root->children[i]->sims) : 0.5;//considered neither good nor bad
		if (currRatio < bestRatio){
			bestRatio = currRatio;
			indexOfBest = i;
		}
	}
	//free branches that will not be used
	for (i = 0; i < root->childCount; i++){
		if (i != indexOfBest){
			freeTree(root->children[i]);
		}
	}
	//set final move
	dest[0] = root->children[indexOfBest]->recentRow;
	dest[1] = root->children[indexOfBest]->recentCol;
	//store root in hashtable for accessing again later
	newRoot = root->children[indexOfBest];
	freeNode(root);
	newRoot->parent = NULL;
	treeHashTable[myChar] = newRoot;
	free(altDest);
}