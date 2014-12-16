/* cpusim-lib.h */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>


typedef struct ModelDat{
	int RulesNo;
	int inputsNo;
	int CyclesNo;
	int MaxRuleCount;
	struct ht_nlist* hashbooltable;
	char** Rules;
	char** results;
	struct Rankings* ranks;
	struct AllRBlock* ruleblocks;
}ModelDat;

char** tokenize(char** strArr, char* str);
void freeData(void *data);
void parseStates(char*word, void *data);
void parseRules(char*word, void *data);
void printFile(FILE *stream, void *data, int runNo);
void printFile_short(FILE *stream, void *data, int runNo, int rulesNum, int cyclesNum, int isRa);
void printHeader(FILE *stream, char* infilename, int nrums, int ncycles);
void printStates(void *data);
void* loadFile(char *infileName, int cyclesNo);
void searchReplace(char* source, char* searchitem, char* replaceitem);
//for boolean eval
void bSearchReplace(char* source, char* searchitem, char* replaceitem);
void printResults(void *data);
void init_outfile(void *data);
void writeResults(void *data);

void random_async(void *data);
void cycle_async(void *data);
void cycle_sync(void *data);
void uniqueRandomArray(void *rArray, int alen);
void swap(int& x, int& y);
