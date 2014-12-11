/*ruleblock-lib.c*/
/*lacks free*/
#include "ruleblock-lib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*currently only supports 30 blockrules*/
#define ALL_RBLOCK_MAXSIZE 30
/*each block has an initial size of 16*/
#define INITIAL_RBLOCK_SIZE 40

void init_ruleblock(void* rblockwrapper){
	struct AllRBlock* rb = (struct AllRBlock*) rblockwrapper;
	rb->ruleno = (int*) calloc( ALL_RBLOCK_MAXSIZE, sizeof(int));
	rb->blockrules = (char***) calloc(ALL_RBLOCK_MAXSIZE, sizeof(char**)); 
	rb->currblocksize = (int*) calloc(ALL_RBLOCK_MAXSIZE, sizeof(int));
	rb->currmaxrblocksize = (int*) calloc(ALL_RBLOCK_MAXSIZE, sizeof(int));
	rb->allRBlockSize = 0;
	rb->maxAllRBlockSize = ALL_RBLOCK_MAXSIZE;
	for (int i=0;i<ALL_RBLOCK_MAXSIZE;i++){
		rb->blockrules[i] = (char**) calloc(INITIAL_RBLOCK_SIZE, sizeof(char*));
		rb->currblocksize[i] = 0;
		rb->currmaxrblocksize[i] = INITIAL_RBLOCK_SIZE;
	}
	printf("ruleblocks initialized.\n"); 
}

/*ruleno specifies the index of block via ruleNoToIndex()*/
void addRuleToBlock(void* rblockwrapper, int ruleno, char* rule){
	struct AllRBlock* rb = (struct AllRBlock*) rblockwrapper;
	int index = ruleNoToIndex(rb, ruleno);
	char** tempblockrules;
	if (index == -1){
		//init new block
		index = rb->allRBlockSize;
		rb->ruleno[rb->allRBlockSize] = ruleno;
		rb->allRBlockSize+=1;
	}
	if (rb->currblocksize[index] == rb->currmaxrblocksize[index]){
		tempblockrules = (char**) calloc(rb->currmaxrblocksize[index]*2, sizeof(char*));
		for (int j=0;j<rb->currblocksize[index];j++){
			tempblockrules[j] = rb->blockrules[index][j];
		}	
		free(rb->blockrules[index]);
		rb->blockrules[index] = tempblockrules;
		rb->currmaxrblocksize[index]*=2;
	}
	rb->blockrules[index][rb->currblocksize[index]] = strdup(rule);
	rb->currblocksize[index]++;
}

/*ruleno specifies the index of block via ruleNoToIndex()*/
int getrblocksize(void* rblockwrapper, int ruleno){
	struct AllRBlock* rb = (struct AllRBlock*) rblockwrapper;
	int index = ruleNoToIndex(rb, ruleno);
	return rb->currblocksize[index];
}

char* getRuleFromBlock(void* rblockwrapper, int ruleno, int i){
	struct AllRBlock* rb = (struct AllRBlock*) rblockwrapper;
	int index = ruleNoToIndex(rb, ruleno);
	return rb->blockrules[index][i];
}

int ruleNoToIndex(void* rblockwrapper, int ruleno){
	struct AllRBlock* rb = (struct AllRBlock*) rblockwrapper;
	for (int i=0;i<rb->allRBlockSize;i++){
		if (rb->ruleno[i] == ruleno){
			return i;
		}
	}
	return -1; //not found
}
