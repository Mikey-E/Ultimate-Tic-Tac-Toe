#ifndef MCTS_H
#define MCTS_H

// -- SETTINGS --
#define MAX_DEPTH 1

typedef struct Node node;

//function headers
void playMCTSMove(char** board, int* dest, int minRow, int maxRow, int minCol, int maxCol, char myChar);
int compareBoard(char** board1, char** board2);

struct Node {
	char** board;//board state associated with this node
	int isRoot;//0 or 1, false or true
	int isMyTurn;//0 or 1, false or true
	node* parent;//only ever 1 parent in the tree
	node** children;
	int childCount;
	int sims;//total simulations down this node
	float wins;//total wins down this node. type float because a draw can be credited as a partial win
};

node* treeHashTable[256];//myChar will hash into this to give the root node pointer for that player.

#endif