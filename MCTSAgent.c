#include "core.h"
#include "MCTS.h"

//void createChildren()

int compareBoard(char** board1, char** board2){
	return 0;//@@@fix
}

void playMCTSMove(char** board, int* dest, int minRow, int maxRow, int minCol, int maxCol, char myChar){
	int i;
	node* root;
	if (treeHashTable[myChar] == 0){//create root based on current state of the board
		root = calloc(1, sizeof(node));
		root->board = board;
		root->isRoot = 1;
		root->isMyTurn = 1;
		root->parent = NULL;
		root->children = NULL;
		root->childCount = 0;
		root->sims = 0;
		root->wins = 0;
	}else{//set root node to where it was before
		root = treeHashTable[myChar];
		//see which move opponent made and update root to that.
		for (i = 0; i < root->childCount; i++){
			if (compareBoard(board, root->children[i]->board)){
				root = root->children[i]->parent;
				break;
			}
		}
	}
	
	//free memory
	free(root);
}