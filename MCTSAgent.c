#include "core.h"
#include "MCTS.h"

//frees all dynamically allocated memory associated with a node
void freeNode(node* node){
	freeBoard(node->board);
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
//void createChildren()

//returns amount of positions that are different between 2 boards,
//like strcmp but for uttt boards.
//will update pointers to index of last compared different spot
int compareBoard(char** board1, char** board2, int* diffRowIndex, int* diffColIndex){

	printf("boards:\n");//@@@debug
	printBoard(board1, -1, -1);//@@@debug
	printBoard(board2, -1, -1);//@@@debug

	int differences = 0;
	int i; int j;
	for (i = 0; i < 9; i++){
		for (j = 0; j < 9; j++){
			if (board1[i][j] != board2[i][j]){
				differences++;
				*diffRowIndex = i;
				*diffColIndex = j;

				//debug@@@
				printf("setting dri to %d, dci to %d\n", i, j);
			}
		}
	}
	printf("differences: %d\n", differences);//@@@debug
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
	node* newChild;
	//calcuate space needed (worst case) for how many children there will be
	root->children = calloc(((root->maxRow - (root->minRow - 1))*(root->maxCol - (root->minCol - 1))), sizeof(node*));
	for (i = root->minRow; i <= root->maxRow; i++){
		for (j = root->minCol; j <= root->maxCol; j++){
			if (queryBoard(root->board, i, j) == '\0'){//then this is a potential move, so create a child node
				newChild = calloc(1, sizeof(node));
				newChild->board = copyBoard(root->board, 9);
				setBoard(newChild->board, root->myChar, i, j);
				newChild->recentRow = i;
				newChild->recentCol = j;
				updateBoardStatus(newChild->board);
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
}

void updateTree(node* root, int maxDepth){
	int i;
	if (root->childCount > 0){//if it already has children, move down the tree
		for (i = 0; i < root->childCount; i++){
			updateTree(root->children[i], maxDepth - 1);
		}
	}else{
		if (maxDepth <= 0){//if at depth limit, play random simulation & backpropagate data up tree
			
		}else{//create new nodes
			getSuccessors(root);
			for (i = 0; i < root->childCount; i++){
				updateTree(root->children[i], maxDepth - 1);
			}
		}
	}
}

void playMCTSMove(char** board, int* dest, int minRow, int maxRow, int minCol, int maxCol, char myChar){
	int i;
	node* root;
	node* newRoot;
	if (treeHashTable[myChar] == 0){//create root based on current state of the board
		root = calloc(1, sizeof(node));
		root->board = copyBoard(board, 9);
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
			if ((compareBoard(board, root->children[i]->board, dest, dest + sizeof(int)) == 0) &&
				(root->children[i]->minRow == minRow)	&&
				(root->children[i]->maxRow == maxRow)	&&
				(root->children[i]->minCol == minCol)	&&
				(root->children[i]->maxCol == maxCol)){
				//debug@@@
				printf("actual:\n");
				printBoard(board, -1, -1);
				printf("one of our children that might match up:\n");
				printBoard(root->children[i]->board, -1, -1);
				printf("values in that child:\n");
				printf("minRow: %d, maxRow: %d\n", root->children[i]->minRow, root->children[i]->maxRow);
				printf("minCol: %d, maxCol: %d\n", root->children[i]->minCol, root->children[i]->maxCol);

				newRoot = root->children[i];
			}else{
				freeTree(root->children[i]);
			}
		}
		freeNode(root);

		//test@@@
		//newRoot->sims = 7;
		//newRoot->parent = NULL;

		root = newRoot;
		root->parent = NULL;
		
		//debug@@@
		printf("our new updated root board\n");
		printBoard(root->board, -1, -1);
		printf("minRow: %d, maxRow: %d\n", root->minRow, root->maxRow);
		printf("minCol: %d, maxCol: %d\n", root->minCol, root->maxCol);
	}
	//update tree for choosing a good move
	updateTree(root, MAX_DEPTH);
	//select best move based on updated tree
	int indexOfBest = 0;
	float bestRatio = 1;
	float currRatio;
	for (i = 0; i < root->childCount; i++){
		//debug@@@
		printf("potential move:\n");
		printBoard(root->children[i]->board, -1, -1);
		printf("and it's values:\n");
		printf("minRow: %d, maxRow: %d\n", root->minRow, root->maxRow);
		printf("minCol: %d, maxCol: %d\n", root->minCol, root->maxCol);

		currRatio = root->children[i]->sims != 0 ? (root->children[i]->wins)/(root->children[i]->sims) : 0.5;//considered neither good nor bad
		if (currRatio < bestRatio){
			//debug@@@
			printf("choosing:\n");
			printBoard(root->children[i]->board, -1, -1);

			bestRatio = currRatio;
			indexOfBest = i;
		}else{
			freeTree(root->children[i]);
		}
	}	
	//set final move
	dest[0] = root->children[indexOfBest]->recentRow;
	dest[1] = root->children[indexOfBest]->recentCol;
	printf("Player %c trying to make move %d%d\n", myChar, dest[0], dest[1]);//@@@debug
	//store root in hashtable for accessing again later
	newRoot = root->children[indexOfBest];
	freeNode(root);
	newRoot->parent = NULL;
	treeHashTable[myChar] = newRoot;
	
	
	//playRandomMove(board, dest, minRow, maxRow, minCol, maxCol, myChar);//@@@ here until actual functionality
}