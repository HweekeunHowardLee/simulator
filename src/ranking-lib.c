/*ranking-lib.c*/
/*lacks free*/
#include "ranking-lib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INITIAL_MAX_RANKS 16

void init_ranks(void* ranklist, int ranksize){
	struct Rankings* r = (struct Rankings*) ranklist;
	r->ranks = (int**) calloc(ranksize, sizeof(int*));
	r->async_selected = (int**) calloc(ranksize, sizeof(int*));
	r->rankno = (int*) calloc(ranksize, sizeof(int));
	r->maxrank = (int*) calloc(ranksize, sizeof(int));
	r->ranksize = ranksize;
	
	for (int i=0; i<ranksize; i++){
		r->ranks[i] = (int*) calloc(INITIAL_MAX_RANKS, sizeof(int));
		r->async_selected[i] = (int*) calloc(INITIAL_MAX_RANKS, sizeof(int));
		r->maxrank[i] = INITIAL_MAX_RANKS;
		r->rankno[i] = 0;
	}
	printf("rankings initialized.\n");
}

void addRanking(void* ranklist, int ruleNum, int rank, int async){
	int index;
	struct Rankings* r = (struct Rankings*) ranklist;
	if (r->rankno[rank] == r->maxrank[rank]){
		resizeRank(ranklist, rank);
	}
	index = r->rankno[rank];
	r->ranks[rank][index] = ruleNum;
	r->async_selected[rank][index] = async;
	r->rankno[rank]++;
}

int getNoRanks(void* ranklist){
	struct Rankings* r = (struct Rankings*) ranklist;
	for (int i=0; i<(r->ranksize); i++){
		if (r->rankno[i]==0){
			return i;
		}
	}
	return r->ranksize;
}

int getRankAmt(void* ranklist, int rank){
	struct Rankings* r = (struct Rankings*) ranklist;
	return r->rankno[rank];
}

int getRankRuleNum(void* ranklist, int randNum, int rank){
	struct Rankings* r = (struct Rankings*) ranklist;
	return r->ranks[rank][randNum];
}

int getRankAsyncBool(void* ranklist, int randNum, int rank){
	struct Rankings* r = (struct Rankings*) ranklist;
	return r->async_selected[rank][randNum];
}

void resizeRank(void* ranklist, int rank){
	int* temp;
	int* temp_async;
	struct Rankings* r = (struct Rankings*) ranklist;
	temp = (int*) calloc(r->maxrank[rank]*2, sizeof(int));
	temp_async = (int*) calloc(r->maxrank[rank]*2, sizeof(int));
	for (int i=0;i<r->maxrank[rank];i++){
		temp[i] = r->ranks[rank][i];
		temp_async[i] = r->async_selected[rank][i];
	}
	free(r->ranks[rank]);
	free(r->async_selected[rank]);
	r->ranks[rank] = temp;
	r->async_selected[rank] = temp_async;
	r->maxrank[rank]*=2;
}
