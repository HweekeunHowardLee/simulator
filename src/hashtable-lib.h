/* hashtable-lib.h */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define bool int

/* store the state */
struct nlist { /* table entry: */
    struct nlist *next; /* next entry in chain */
    char *name; /* name of molecule */
	bool state;
	int roundnum;
};

struct ht_nlist{
	struct nlist** ht;
	int hashsize;
	int currNoInputs;
	int maxInputs;
	char** inputs; //string array
	int* iStates;  //integer array
	int* iRounds;  //integer array
	int** averages;
	int currAvg;
};


void install(void* hashtabwrapper, char *name, bool currState, int num);
bool lookup_bool(void* hashtabwrapper, char *s);
int lookup_roundnum(void* hashtabwrapper, char* s);
void init_ht(void* hashtabwrapper, int hashsize);
void hash_free(void* hashtabwrapper);
void restoreIStates(void* hashtabwrapper);
void init_averages(void* hashtabwrapper, int cycles, int rulesno);
void incAverages(void* hashtabwrapper);
void printAverages(FILE *stream, void* hashtabwrapper, int cycles, int rulesno, int rtype);
void printAverages_short(FILE *stream, void* hashtabwrapper, int cycles, int rulesno, int rtype);
void update_roundStates(void* hashtabwrapper, int currRound);
