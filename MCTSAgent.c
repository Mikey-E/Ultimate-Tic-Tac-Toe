#include "core.h"

#define SEARCH_DEPTH 1

typedef struct Node node;

struct Node {
	int isRoot;//0 or 1, false or true
	node* parent;//only ever 1 parent in the tree
	node** children;
	int sims;//total simulations down this node
	float wins;//total wins down this node. type float because a draw can be credited as a partial win
};

void playMCTSMove(char** board, int* dest, int minRow, int maxRow, int minCol, int maxCol, char myChar){
	
}