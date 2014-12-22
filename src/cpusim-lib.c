/*cpusim-lib.c*/

#include <stdio.h>
#include <stdlib.h>
#include "cpusim-lib.h"
#include "hashtable-lib.h"
#include "booleval-lib.h"
#include "ranking-lib.h"
#include "ruleblock-lib.h"
#include "getline.h"
#include "string.h"
#include "CycleTimer.h"
#include "ctype.h"

#define INITIAL_MAXRULECOUNT 200
#define bool int
#define HASHSIZE 1024
#define false 0
#define true 1
#define PRINT_DETAILS 0
#define SHOW_RANDOM_ARR 0
#define DEBUG_RANK 1

#define MAX_RANKS 10

void* loadFile( char * infileName, int cyclesNo){
    FILE *infile;
    char *buffer;
    ModelDat *data;
	char* temprank;
	char* tempasync;
	char* temprule;
    data = (ModelDat*) malloc(sizeof(ModelDat));
    infile = fopen(infileName,"rt");
    
    if (infile == NULL) {
    	printf("Can't open %s for output.\n", infileName);
    	exit(EXIT_FAILURE);
    }
    printf("opened file..\n");

	data->CyclesNo = cyclesNo;
	data->MaxRuleCount = INITIAL_MAXRULECOUNT; //200
	data->RulesNo = 0;
	data->hashbooltable = (struct ht_nlist*) malloc(sizeof(struct ht_nlist));
	init_ht(data->hashbooltable, HASHSIZE); //HASHSIZE = 1024
	data->Rules = (char**) calloc(data->MaxRuleCount, sizeof(char*));
	data->ranks = (Rankings*) malloc(sizeof(Rankings));
	init_ranks(data->ranks, MAX_RANKS);
	data->ruleblocks = (AllRBlock*) malloc(sizeof(AllRBlock));
	init_ruleblock(data->ruleblocks);

	//initialize states
    while (fgetline_123(&buffer,infile)) {
	  if (strstr("Rules:",buffer) != NULL) 
	  	break;
      parseStates(buffer, data); //parse initial states
    }

	//initialize rules
    while (fgetline_123(&buffer,infile))
    {
      //check if there is any block rules
	  //reads block rule
	  if (strstr(buffer,"{") != NULL){
	    //fprintf(stderr, "buffer: %s\n", buffer);
		temprank = "0";
		tempasync = "0";
		if (strstr(buffer,":")!=NULL){ //false = no rankings
			//TODO EXTRACT ASYNC option
			temprank = strtok(buffer,":"); //gets the rank
			//fprintf(stderr, "temprank: %s\n", temprank);
			if (strstr(temprank, "*") != NULL){
				temprank[strlen(temprank) - 1] = '\0';
				tempasync = "1";
				//fprintf(stderr,"A block rule with async behavior is detected..\n");
			}
		}
		while (fgetline_123(&buffer,infile)){
		  if (strstr(buffer,"}")!=NULL){ 
			addRanking(data->ranks, data->RulesNo, atoi(temprank), atoi(tempasync) );
			data->RulesNo++; 
			break; 
		  }
		  //fprintf(stderr,"im in!!%s\n", buffer);
		  temprule = strdup(buffer);
		  //fprintf(stderr, "Test1\n");
		  searchReplace(temprule," ","");
		  //fprintf(stderr, "Test2\n");
		  addRuleToBlock(data->ruleblocks, data->RulesNo, temprule);
		  //fprintf(stderr, "Test3\n");
		  //parseRuleBlock(buffer, data); 
		}
		continue;
	  }
	  
	  parseRules(buffer, data); 
    }
    
	/*assumes all states and inputs are loaded*/
	//init_roundTogger(data->roundToggler, data->inputsNo);
	//printf("A total of %i rules are loaded.\n",data->RulesNo);
	init_averages(data->hashbooltable,cyclesNo,data->RulesNo);
    fclose(infile);
    printf("loaded file.\n");

    return data;
}

void freeData(void *data){
    ModelDat *m = (ModelDat*) data;
    printf("freeing data..\n"); 
	for(int i=0;i<m->MaxRuleCount;i++){
        free(m->Rules[i]);
    }
	hash_free(m->hashbooltable);
	free(m);
}

void parseStates(char *word,void* data){	
	//ignore empty lines
	int rt;
	if (strcmp(word,"") == 0 || strstr(word, "Rules:") != NULL){
		return;
	}

    ModelDat *m = (ModelDat*) data;
    char* pch;
	char* valueName;
	int initState = -1;

	//assume only one init value per line
	pch = strtok (word," =");
	valueName = pch; // NAD = False --> valueName = NAD
	pch = strtok (NULL," ="); // NAD = False --> pch = False
	
	for (int i = 0; i < strlen(pch); i++)
		pch[i] = tolower(pch[i]);
	
	if (strstr(pch, "true") != NULL) {
		initState = 1;
	} else if (strstr(pch, "false") != NULL) {
		initState = 0;
	} else {
		printf("ERROR: unknown state detected for %s!\n", valueName);
	}

	//gets round toggler //TODO don't use this for now
	rt = -1;
	if ((pch = strtok (NULL," =")) != NULL){
		//round toggler found
		//supposed to be >0
		rt = atoi(pch)-1; 
		if (rt<0){
			printf("ERROR: toggle state for %s was found to be <=0, undefined behavior WILL occur!!\n", valueName);
		}
	}
	if (initState==-1){
		printf("ERROR in parsing states!!\n ..undefined behavior will be expected!\n");
	}

	install(m->hashbooltable, valueName, initState, rt);
	return;
}

void parseRules(char *word, void* data){
	char* pch;
	//ignore empty lines
	if (strcmp(word,"") == 0 || strstr(word,"Rules:") != NULL){
		return;
	}

    ModelDat *m = (ModelDat*) data;
	
	if (m->RulesNo >= m->MaxRuleCount){
		//realloc
		char** tempRules = (char**) calloc(m->MaxRuleCount*2, sizeof(char*));
		for (int i=0;i<m->MaxRuleCount;i++){
			tempRules[i] = m->Rules[i];
		}
		free(m->Rules);
		m->Rules = tempRules;
		m->MaxRuleCount*=2;
	}

	searchReplace(word," ","");

	if (strstr(word,":") != NULL) { //false = no rankings
		pch = strtok(word,":"); //gets the rank
		addRanking(m->ranks, m->RulesNo, atoi(pch), 0);
		pch = strtok (NULL, ":"); //gets the word
		word = pch;
	} else {
		addRanking(m->ranks, m->RulesNo, 0, 0); //adds everything to rank 0
	}
	
	(m->Rules)[m->RulesNo] = (char*) malloc(strlen(word)+1);
	strcpy((m->Rules)[m->RulesNo], word);
	m->RulesNo++;
	return;
}

void cycle_async(void *data){
	ModelDat *m = (ModelDat*) data;
	int cycles = m->CyclesNo;
	//int totalRulesNo = m->RulesNo;
	//int alen = totalRulesNo;
	int currRuleNo;
	int currRuleAsync;
	int *rArray;
	//int *rArray = (int*) calloc(alen,sizeof(int));
	bool result;
	char* currRule;
	char* pch;
	char* pch1;
	char* output;
	char* expr;
	char* tempexpr;
	char* tempitoa;
	int test;
	int rankTotal;
	int rankamt;
	int roundnum;
	int rblocksize;
	char** csoutput;
	int* csroundnum;
	bool* csresult;
	tempitoa = (char*)malloc(sizeof(char)+1);
	rankTotal = getNoRanks(m->ranks);
	
	for (int i = 0; i < cycles; i++) {
		update_roundStates(m->hashbooltable, i);
		for (int q = 0; q < rankTotal; q++) {
			rankamt = getRankAmt(m->ranks, q); 
			rArray = (int*) calloc(rankamt,sizeof(int));
			uniqueRandomArray(rArray,rankamt); //get an array with random order

			for (int j = 0; j < rankamt; j++) { //iterate through the rankamt
				currRuleNo = getRankRuleNum(m->ranks, rArray[j], q);
				currRuleAsync = getRankAsyncBool(m->ranks, rArray[j], q);
	
				//check if current rule is a block rule //TODO need to debug this
				if (ruleNoToIndex(m->ruleblocks, currRuleNo) != -1) {
					//perform cs here
					rblocksize = getrblocksize(m->ruleblocks, currRuleNo);
					//fprintf(stderr,"rule block size: %d\n",rblocksize);
					csoutput = (char**) calloc(rblocksize,sizeof(char*));
					csresult = (bool*) calloc(rblocksize,sizeof(bool));
					csroundnum = (int*) calloc(rblocksize,sizeof(int));

					////fprintf(stderr,"Test cycle 2\n");
					for (int rs = 0; rs < rblocksize; rs++) {
						currRule = strdup(getRuleFromBlock(m->ruleblocks, currRuleNo, rs));
						//printf("Current rule: %s\n", currRule);
						/*assumes 1 equation per line*/
						pch = strtok(currRule," =");
						csoutput[rs] = strdup(pch);
						pch = strtok (NULL," =");
						tempexpr = strdup(pch);
						expr = strdup(pch);
						expr[strlen(expr)-1] = '\0';
						
						pch1 = strtok(tempexpr," !+*();");
						while(pch1!=NULL){
							test = lookup_bool(m->hashbooltable,pch1);
							sprintf(tempitoa, "%d", test);
							bSearchReplace(expr,pch1,tempitoa);
							pch1 = strtok(NULL," !+*();");
						}
						//fprintf(stderr, "rule block expression is %s\n",expr);
						csresult[rs] = evalBoolExpr(expr);
						//fprintf(stderr,"rule block expression result is %i\n",csresult[rs]);
						/*toggle round is the same*/
						csroundnum[rs] = lookup_roundnum(m->hashbooltable,csoutput[rs]);
						/*if blocks are async, we update each rule within the block each time it is ran, according to its natural order*/
						if (currRuleAsync){
						  //fprintf(stderr,"Test async 1\n");
							install(m->hashbooltable,csoutput[rs],csresult[rs], csroundnum[rs]);
							//fprintf(stderr,"Output: %s, Result: %d, Round number: %d\n",csoutput[rs],csresult[rs], csroundnum[rs]);
						}
					}
					/*if blocks are not async, we run entire block synchronously*/
					if (!currRuleAsync){
						for (int rs=0;rs<rblocksize;rs++){
							//printf("fromblock: %s is now %i.\n", csoutput[rs],csresult[rs]);
							install(m->hashbooltable,csoutput[rs],csresult[rs], csroundnum[rs]);
							//fprintf(stderr,"Output: %s, Result: %d, Round number: %d\n",csoutput[rs],csresult[rs], csroundnum[rs]);
						}
					}
				} else {
					/*async or sync makes no difference*/
					currRule = (char*) malloc(strlen(m->Rules[currRuleNo])+1);
					strcpy(currRule, m->Rules[currRuleNo]);

					/*assumes 1 equation per line*/
					pch = strtok (currRule," ="); //left value
					output = (char*) malloc(strlen(pch)+1);
					strcpy(output,pch);//left value
					pch = strtok (NULL," ="); //right value
					tempexpr = (char*) malloc(strlen(pch)+1);
					expr = (char*) malloc(strlen(pch)+1);//right value
					strcpy(tempexpr, pch);
					strcpy(expr, pch);
					expr[strlen(expr)-1] = '\0';

					pch1 = strtok(tempexpr," !+*();");
                  
					while(pch1!=NULL){
						test = lookup_bool(m->hashbooltable,pch1);
						sprintf(tempitoa, "%d", test);
						bSearchReplace(expr,pch1,tempitoa);
						pch1 = strtok(NULL," !+*();");
					}
					result = evalBoolExpr(expr);
					roundnum = lookup_roundnum(m->hashbooltable,output);
					install(m->hashbooltable,output,result, roundnum);//update the state
				}
				writeResults(m);
			}//end rank
			free(rArray);	
		}
		//printf("End cycle %d\n", i+1);
	}
}

/*
void cycle_sync(void *data){
	ModelDat *m = (ModelDat*) data;
	int cycles = m->CyclesNo;
	int totalRulesNo = m->RulesNo;
	int currRuleNo;
	bool *result;
	int *roundnum;
	char* currRule;
	char* pch;
	char* pch1;
	char** output;
	char* expr;
	char* tempexpr;
	char* tempitoa;
	int test;
	int outputIndex;
	tempitoa = (char*)malloc(sizeof(char)+1);
	for (int i=0;i<cycles;i++){
		output = (char**) calloc(totalRulesNo,sizeof(char*));
		result = (bool*) calloc(totalRulesNo,sizeof(bool));
		roundnum = (bool*) calloc(totalRulesNo,sizeof(int));
		outputIndex = 0;
		for (int j=0;j<totalRulesNo;j++){
			currRuleNo = j;
			currRule = (char*) malloc(strlen(m->Rules[currRuleNo])+1);
			strcpy(currRule, m->Rules[currRuleNo]);
			strcpy(currRule, m->Rules[currRuleNo]);
			pch = strtok (currRule," =");
			output[j] = (char*) malloc(strlen(pch)+1);
			strcpy(output[j],pch);
			pch = strtok (NULL," =");
			tempexpr = (char*) malloc(strlen(pch)+1);
			expr = (char*) malloc(strlen(pch)+1);
			strcpy(tempexpr, pch);
			strcpy(expr, pch);
			expr[strlen(expr)-1] = '\0';
			
			pch1 = strtok(tempexpr," !+*();");
			while(pch1!=NULL){
				test = lookup_bool(m->hashbooltable,pch1);
				sprintf(tempitoa, "%d", test);
				bSearchReplace(expr,pch1,tempitoa);
				pch1 = strtok(NULL," !+*();");
			}
			result[j] = evalBoolExpr(expr);
			roundnum[j] = lookup_roundnum(m->hashbooltable,output[j]);
			if (SHOW_RANDOM_ARR){
				printf("%i ",currRuleNo);
			}
			if (PRINT_DETAILS){
				printf("given %s is %s = %i\n",m->Rules[currRuleNo],expr,result[j]);
				printStates(m);
			}
			//free(currRule);
			//free(tempexpr);
			//free(expr);
			//outputIndex++;
		}
		for (int j=0;j<totalRulesNo;j++){
			install(m->hashbooltable,output[j],result[j], roundnum[j]);
			//free(output[j]);
		}
		if (SHOW_RANDOM_ARR){ printf("\n"); }
		writeResults(m);
	}
	//free(tempitoa);
	//free(output);
	//free(result);
}
*/


void random_async(void *data){
    ModelDat *m = (ModelDat*) data;
	int cycles = m->CyclesNo;
	int totalRulesNo = m->RulesNo;
	int currRuleNo;
	bool result;
	char* currRule;
	char* pch;
	char* pch1;
	char* output;
	char* expr;
	char* tempexpr;
	char* tempitoa;
	int test;
	int roundnum;
	tempitoa = (char*)malloc(sizeof(char)+1);
	for (int i = 0; i < cycles; i++) {
		update_roundStates(m->hashbooltable, i);
		srand(CycleTimer::currentTicks());
		int temp = rand();
		currRuleNo = temp % totalRulesNo;
		
		currRule = (char*) malloc(strlen(m->Rules[currRuleNo])+1);
		strcpy(currRule, m->Rules[currRuleNo]);
		/*assumes 1 equation per line*/
		pch = strtok (currRule," =");
		output = (char*) malloc(strlen(pch)+1);
		strcpy(output,pch);
		pch = strtok (NULL," =");
		tempexpr = (char*) malloc(strlen(pch)+1);
		expr = (char*) malloc(strlen(pch)+1);
		strcpy(tempexpr, pch);
		strcpy(expr, pch);
		expr[strlen(expr)-1] = '\0';
		
		pch1 = strtok(tempexpr," !+*();");
		while(pch1!=NULL){
			test = lookup_bool(m->hashbooltable,pch1);
			sprintf(tempitoa, "%d", test);
			bSearchReplace(expr,pch1,tempitoa);
			pch1 = strtok(NULL," !+*();");
		}
		result = evalBoolExpr(expr);
		roundnum = lookup_roundnum(m->hashbooltable,output);
		install(m->hashbooltable,output,result,roundnum);		
		writeResults(m); /*each cycle in random_async adds one result*/
	}
	free(tempitoa);
}


//generic search and replace
void searchReplace(char* source, char* searchitem, char* replaceitem){
	//only valid if searchitem.length > replaceitem.length
	
	char* pch;
	char* temp;
	int searchlength = strlen(searchitem);
	temp = (char*) malloc(strlen(source));
    while ((pch = strstr(source,searchitem))!=NULL)
        {
			strcpy(temp,replaceitem);
			strcat(temp,pch+searchlength);
			strcpy(pch,temp);
        }
	free(temp);
}

#define valid_token(c)  (!((c >= 'A' && c <= 'Z') || c == '_'))
//a specific search and replace for symbol resolution of booleval.
void bSearchReplace(char* source, char* searchitem, char* replaceitem){
	//only valid if searchitem.length > replaceitem.length
	
	char* pch;
	char* temp;
	char fc;
	char bc;
	int searchlength = strlen(searchitem);
	temp = (char*) malloc(strlen(source));
    while ((pch = strstr(source,searchitem))!=NULL)
        {
		bc = *(pch-1);
		fc = *(pch+strlen(searchitem));
		//printf("%s\n",source);
		//printf("%c %c\n", bc, fc);
		if (valid_token(bc)&&valid_token(fc)) {
			strcpy(temp,replaceitem);
			strcat(temp,pch+searchlength);
			strcpy(pch,temp);
		}
		source = pch+1; //move forward to prevent infinite loop
        }
	free(temp);
}

void printStates(void *data){
	printf("printing states of results..\n");
    ModelDat *m = (ModelDat*) data;
	int noOfStates = m->hashbooltable->currNoInputs;
	for (int i=0;i<noOfStates;i++){
		printf("%s: %i\n", m->hashbooltable->inputs[i], lookup_bool(m->hashbooltable,m->hashbooltable->inputs[i]));
	}
	//printf("end of result.\n");
}

void init_outfile(void *data){
    ModelDat *m = (ModelDat*) data;
	m->inputsNo = m->hashbooltable->currNoInputs;
	m->results = (char**) calloc(m->inputsNo, sizeof(char*));
	for (int i=0;i<(m->inputsNo);i++){
		//name of input + 1 + (cycles + a space)*2*num_of_inputs
		m->results[i] = (char*) malloc(strlen(m->hashbooltable->inputs[i])+1+((m->CyclesNo)*2*(m->RulesNo)));
		strcpy(m->results[i],m->hashbooltable->inputs[i]);
	}
	writeResults(m);
}

void printResults(void *data){
	printf("printing results slip..\n");
	ModelDat *m = (ModelDat*) data;
	for (int i=0;i<(m->inputsNo);i++){
		printf("#%i: %s\n",i,m->results[i]);
	}
}

void writeResults(void *data){
	ModelDat *m = (ModelDat*) data;
	char temp[3];
	int tempBool;
	for (int i = 0; i < (m->inputsNo); i++) {
		tempBool = lookup_bool(m->hashbooltable,m->hashbooltable->inputs[i]);
		sprintf(temp, " %d", tempBool);
		strcat(m->results[i],temp);
	}
	incAverages(m->hashbooltable);
}

void printHeader(FILE *stream, char* infilename, int nrums, int ncycles){
	fprintf(stream,"%s succeeded with %d Runs of %d Cycles each.\n\n",infilename,nrums,ncycles);
}


void printFile(FILE *stream,void *data, int runNo){
	//printf("saving run #%i..\n",runNo);
	ModelDat *m = (ModelDat*) data;
	fprintf(stream,"Run #%i\n",runNo);
	for(int i = 0; i < (m->inputsNo); i++){
        fprintf(stream,m->results[i]);
        fprintf(stream,"\n");
	}
	fprintf(stream,"\n");
}

char** tokenize(char** strArr, char* str) {
	char* p = strtok (str, " ");
	int n_spaces = 0;

	while (p) {
  		strArr = (char**) realloc (strArr, sizeof(char*) * (++n_spaces));
  		strArr[n_spaces-1] = p;
  		p = strtok (NULL, " ");
	}
  	return strArr;
}


void printFile_short(FILE *stream,void *data, int runNo, int rulesNum, int cyclesNum, int isRa){

	char ** strArr  = NULL;
	ModelDat *m = (ModelDat*) data;
	char *values;
	fprintf(stream,"Run #%i\n",runNo);
	for (int i = 0; i < m->inputsNo; i++) {
		strArr = tokenize(strArr, m->results[i]);
		fprintf(stream, strArr[0]);
		for (int j = 0; j <= cyclesNum; j++) {
		  int temp1 = isRa == 1? j : rulesNum * j;
		  fprintf(stream," %s", strArr[temp1+1]);
		}
		fprintf(stream, "\n");
	}
}

void uniqueRandomArray(void *rArray, int alen){
	int *ra = (int*) rArray;
	int X, Y;
	//initialize array
	for (int i=0;i<alen;i++){
		ra[i] = i;
	}
	//random swaps
	for (int i=0;i<alen;i++){
		srand(CycleTimer::currentTicks());
		X = rand() % (alen);
		Y = rand() % (alen);
		swap(ra[X],ra[Y]);
	}
}

void swap(int& x, int& y){
	int temp = x;
	x = y;
	y = temp;
}
