#ifndef MCTS_H
#define MCTS_H

// -- SETTINGS --
#define MAX_DEPTH 6 //ALWAYS keep as at least 2. Zero wouldn't even make any sense, and also probably cause seg fault
					//1 leaves tree with nothing to compare with after opponent's move, also seg fault.

typedef struct Node node;

//function headers
void freeNode(node* node);
void freeTree(node* root);
int compareBoard(char** board1, char** board2, int* diffRowIndex, int* diffColIndex);
void getSuccessors(node* root);
void updateTree(node* root, int maxDepth);
char playRandomSimulation(node* root);

struct Node {
	char** board;//board state associated with this node
	char** smallSectorBoard;//for calculating winner, as usual
	node* parent;//only ever 1 parent in the tree
	node** children;
	int childCount;
	int sims;//total simulations down this node
	float wins;//total wins down this node. type float because a draw can be credited as a partial win

	//boundaries of next move, as usual
	int minRow;
	int maxRow;
	int minCol;
	int maxCol;

	char myChar;//who's turn it is to play next on the board associated with this node

	//most recent move played on the associated board
	int recentRow;
	int recentCol;
};

node* treeHashTable[256];//myChar will hash into this to give the root node pointer for that player.

#endif