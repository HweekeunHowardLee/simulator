#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "cpusim-lib.h"
#include "booleval-lib.h"
#include "hashtable-lib.h"
#include "ranking-lib.h"
#include "getline.h"
#include "CycleTimer.h"

#define true 1
#define false 0

int main(int argc, char *argv[])
{

  if (argc <7)
  {
    printf("usage: %s <infileName> <type: ra | ca > <runs> <cycles> <outType: short> <outFilename>\nReview README.txt for optional modes. \n",argv[0]);
    return EXIT_FAILURE;
  }
  //parse the arguments
  char* infileName = argv[1];
  char* rtype = argv[2];
  int numRuns = atoi (argv[3]);
  int numCycles = atoi (argv[4]);
  char* outType = argv[5];
  char* outfileName = argv[6];


  ModelDat *data;
  FILE *outfile;
  double startTime;
  double endTime;
  int cprint = -1;

  //Error handling

  //type error
  if ((strlen(rtype)!=2)||(!(strcmp(rtype,"ra")==0 || strcmp(rtype,"ca")==0))){
    printf("ERROR: runtype not recognized!\n");
    printf("usage: %s <infileName> <type: ra | ca > <runs> <cycles> <outType: short> <outFilename>\nReview README.txt for optional modes. \n",argv[0]);
    return EXIT_FAILURE;
  } 

  //cannot open output file
  outfile = fopen(outfileName,"wt");
  if (NULL == outfile)
  {
    printf("Can't open %s for output.\n",outfileName);
    exit(EXIT_FAILURE);
  }
  fclose(outfile);
  outfile = fopen(outfileName,"at");  
  if (NULL == outfile)
  {
    printf("Can't open %s for output.\n",outfileName);
    exit(EXIT_FAILURE);
  }

  data = (ModelDat*) loadFile(infileName, numCycles);
  printf("Finished load of %s.\n",infileName);
  
  /*running the cycles*/
  printf("starting %i runs with %i cycles each..\n", numRuns, numCycles);
  printHeader(outfile, infileName, numRuns, numCycles);
  startTime = CycleTimer::currentSeconds();
  
  for (int i=0;i<numRuns;i++){
	  init_outfile(data);
	  //printf("init outfile %i!!\n",i);
	  if (strcmp(rtype,"ra") == 0){
      cprint = 0;
      random_async(data);
	  } else if (strcmp(rtype,"ca") == 0){
      cprint = 1;
      cycle_async(data);
	  } 

	  /*writing to file */
	  if (strcmp(outType,"short") == 0){
	    printFile_short(outfile, data, i, data->RulesNo, numCycles);
	  } else {
      printFile(outfile, data, i);
	  }

	  /*restore initial states and add to count avg*/
	  restoreIStates(data->hashbooltable);
	  printf("End run %d\n",i);
  }
  
  //print summary
  
  //differnet ways of generating output
  if (strcmp(outType,"short") == 0){
    printAverages_short(outfile, data->hashbooltable, numCycles, data->RulesNo, cprint);
  } else {
    printAverages(outfile, data->hashbooltable, numCycles, data->RulesNo, cprint);
  }

  endTime = CycleTimer::currentSeconds();
  printf("program ran for %f seconds\n",endTime-startTime);
  
  freeData(data);
  fclose(outfile);
  printf("success\n");
  return (EXIT_SUCCESS);
}
