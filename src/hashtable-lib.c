/* hashtable-lib.c */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hashtable-lib.h"
#define bool int
#define INITIAL_SIZE 16;

//check toggle 
void update_roundStates(void* hashtabwrapper, int currRound){
	char* currinput;
	int num;
	bool currState;
	bool newState;
	struct ht_nlist* ht_wrapper = (struct ht_nlist*) hashtabwrapper;
	for (int i = 0; i < ht_wrapper->currNoInputs; i++) {
		currinput = ht_wrapper->inputs[i];
		num = lookup_roundnum(ht_wrapper, currinput); 
		if (currRound == num){
			//printf("%i detected..\n", currRound);
			currState = (lookup_bool(ht_wrapper, currinput));
			newState = (currState)? 0: 1;
			//printf("%i is now %i..\n", currState, newState);
			install(ht_wrapper, currinput, newState, num);
		}
	}		
}

void hash_free(void* hashtabwrapper){
	struct nlist* temp;
	printf("freeing hashtable..\n");
	struct ht_nlist* ht_wrapper = (struct ht_nlist*) hashtabwrapper;
	for (int i=0;i<ht_wrapper->hashsize;i++){
		struct nlist* start = ht_wrapper->ht[i];
		while (start!=NULL){
			temp = start->next;
			free(start);
			start = temp;
		}
	}
	free(ht_wrapper->ht);
	for (int i=0;i<ht_wrapper->maxInputs;i++){
		free(ht_wrapper->inputs[i]);
	}
	for (int i=0;i<ht_wrapper->currNoInputs;i++){
		free(ht_wrapper->averages[i]);
	}
	free(ht_wrapper->iStates);
	free(ht_wrapper->iRounds);
	free(ht_wrapper);
}

void init_ht(void* hashtabwrapper, int hashsizeno){
	//printf("creating hashtable..\n");
	struct ht_nlist* ht_wrapper = (struct ht_nlist*) hashtabwrapper;
	ht_wrapper->hashsize = hashsizeno;
	ht_wrapper->maxInputs = INITIAL_SIZE;
	ht_wrapper->currNoInputs = 0;
	ht_wrapper->currAvg = 0;
	ht_wrapper->inputs = (char**) calloc(ht_wrapper->maxInputs, sizeof(char*));
	ht_wrapper->iStates = (int*) calloc(ht_wrapper->maxInputs, sizeof(int));
	ht_wrapper->iRounds = (int*) calloc(ht_wrapper->maxInputs, sizeof(int));
	ht_wrapper->ht = (struct nlist**) calloc(hashsizeno,sizeof(struct nlist*));
	printf("hashtable initialized.\n");
}

void init_averages(void* hashtabwrapper, int cycles, int rulesno){
	struct ht_nlist* ht_wrapper = (struct ht_nlist*) hashtabwrapper;
	ht_wrapper->averages = (int**) calloc(ht_wrapper->currNoInputs, sizeof(int*));
	for (int i = 0; i < ht_wrapper->currNoInputs; i++) {
		ht_wrapper->averages[i] = (int*) calloc(cycles*rulesno+1,sizeof(int));
		for (int j = 0; j < cycles*rulesno+1; j++) {
			ht_wrapper->averages[i][j] = 0;
		}
	}
}

/* hash: form hash value for string s */
unsigned hash(char *s, int hashsize)
{
    unsigned hashval;
    for (hashval = 0; *s != '\0'; s++)
      hashval = *s + 31 * hashval;
    return hashval % hashsize;
}

/* lookup: look for s in hashtabwrapper */
struct nlist *lookup(void* hashtabwrapper, char *s)
{
    struct nlist *np;
	struct ht_nlist* ht_wrapper = (struct ht_nlist*) hashtabwrapper;
	struct nlist** ht = ht_wrapper->ht;
    for (np = ht[hash(s, ht_wrapper->hashsize)]; np != NULL; np = np->next) //collision
        if (strcmp(s, np->name) == 0)
          return np;
    return NULL;
}

/*checks the bool value of corresponding molecule s*/
bool lookup_bool(void* hashtabwrapper, char* s)
{
    struct nlist *np;
	struct ht_nlist* ht_wrapper = (struct ht_nlist*) hashtabwrapper;
	struct nlist** ht = ht_wrapper->ht;
    for (np = ht[hash(s, ht_wrapper->hashsize)]; np != NULL; np = np->next){
        if (strcmp(s, np->name) == 0){
          return np->state;
		}
	}
	printf("ERROR: %s not found, illegal operator detected!\n", s);
    return 0;
}

/*checks the toggle round of corresponding molecule s*/
int lookup_roundnum(void* hashtabwrapper, char* s)
{
    struct nlist *np;
	struct ht_nlist* ht_wrapper = (struct ht_nlist*) hashtabwrapper;
	struct nlist** ht = ht_wrapper->ht;
    for (np = ht[hash(s, ht_wrapper->hashsize)]; np != NULL; np = np->next){
        if (strcmp(s, np->name) == 0){
          return np->roundnum;
		}
	}
	printf("ERROR: %s not found, illegal operator detected!\n", s);
    return 0;
}

/* install: put (name, currState) in hashtable */
void install(void* hashtabwrapper, char *name, bool currState, int num)
{
    struct nlist *np;
	struct ht_nlist* ht_wrapper = (struct ht_nlist*) hashtabwrapper;
	struct nlist** ht = ht_wrapper->ht;
	int hashsizeno = ht_wrapper->hashsize;
    unsigned hashval;

	np = lookup(hashtabwrapper, name);
	/* create new item */
    if (np == NULL) {
        np = (struct nlist *) malloc(sizeof(struct nlist));
        hashval = hash(name, hashsizeno);
        //append to the head of the list
        np->next = ht[hashval];
		np->state = currState;
		np->name = name;
		np->roundnum = num;
        ht[hashval] = np;
		/*if full, realloc*/
		if (ht_wrapper->currNoInputs == ht_wrapper->maxInputs){
		printf("%s\n", "Start");
			char** tempInputs = (char**) calloc(ht_wrapper->maxInputs*2, sizeof(char*));
			int* tempIStates = (int*) calloc(ht_wrapper->maxInputs*2, sizeof(int));
			int* tempIRounds = (int*) calloc(ht_wrapper->maxInputs*2, sizeof(int));
			for (int i=0;i<ht_wrapper->currNoInputs;i++){
				tempInputs[i] = ht_wrapper->inputs[i];
				tempIStates[i] = ht_wrapper->iStates[i];
				tempIRounds[i] = ht_wrapper->iRounds[i];
			}
			free(ht_wrapper->inputs);
			free(ht_wrapper->iStates);
			free(ht_wrapper->iRounds);
			ht_wrapper->inputs = tempInputs;
			ht_wrapper->iStates = tempIStates;
			ht_wrapper->iRounds = tempIRounds;
			ht_wrapper->maxInputs*=2;
		}
		ht_wrapper->inputs[ht_wrapper->currNoInputs] = (char*) malloc(strlen(name)+1);
		strcpy(ht_wrapper->inputs[ht_wrapper->currNoInputs], name);
		ht_wrapper->iStates[ht_wrapper->currNoInputs] = currState; /*saves initial states*/
		ht_wrapper->iRounds[ht_wrapper->currNoInputs] = num; /*saves initial rounds*/
		ht_wrapper->currNoInputs++;
    } else { 
	/* already there and update state */
        np->state = currState;
		np->roundnum = num;
    }
}

void restoreIStates(void* hashtabwrapper){
	struct ht_nlist* ht_wrapper = (struct ht_nlist*) hashtabwrapper;
	for (int i=0;i<ht_wrapper->currNoInputs;i++){
		install(ht_wrapper, ht_wrapper->inputs[i], ht_wrapper->iStates[i], ht_wrapper->iRounds[i]);
	}
	ht_wrapper->currAvg = 0;
}

void incAverages(void* hashtabwrapper){
	int tempBool;
	struct ht_nlist* ht_wrapper = (struct ht_nlist*) hashtabwrapper;
	for (int i = 0; i < ht_wrapper->currNoInputs; i++) {
		tempBool = lookup_bool(ht_wrapper,ht_wrapper->inputs[i]);
		ht_wrapper->averages[i][ht_wrapper->currAvg] += tempBool;
	}
	ht_wrapper->currAvg++;
}

void printAverages(FILE *stream, void* hashtabwrapper, int cycles, int rulesno, int rtype){
	struct ht_nlist* ht_wrapper = (struct ht_nlist*) hashtabwrapper;
	char temp[32]; //shouldnt exceed this?
	/*rtype == 1 implies ca, otherwise ra*/
	int totalno = (rtype)? cycles*rulesno: cycles; // "+1" is because of the initial state
	fprintf(stream, "Frequency Summary:\n");
	for (int i = 0; i < ht_wrapper->currNoInputs; i++) {
		fprintf(stream, ht_wrapper->inputs[i]);
		for (int j = 0; j < totalno; j++) {
			sprintf(temp," %d",ht_wrapper->averages[i][j]);
			fprintf(stream,temp);
		}
		fprintf(stream, "\n");
	}
}

void printAverages_short(FILE *stream, void* hashtabwrapper, int cycles, int rulesno, int rtype){
	struct ht_nlist* ht_wrapper = (struct ht_nlist*) hashtabwrapper;
	char temp[32]; //shouldnt exceed this?
	/*rtype == 1 implies ca, otherwise ra*/
	//int totalno = (rtype)? cycles*rulesno+1: cycles+1;
	
	int temp1;
	fprintf(stream, "Frequency Summary:\n");
	for (int i = 0; i < ht_wrapper->currNoInputs; i++){
		fprintf(stream, ht_wrapper->inputs[i]);
		for (int j = 0; j < cycles; j++) {
		  temp1 = rulesno*j;
		  sprintf(temp," %d",ht_wrapper->averages[i][temp1]);
		  fprintf(stream,temp);
		}
		fprintf(stream, "\n");
	}
}
	
