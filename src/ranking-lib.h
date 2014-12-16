////ranking-lib.h

typedef struct Rankings{
	int** ranks;   // associated rule numbers in each rank // 2D array with rank as rows and rules number as columns
	int** async_selected;
	int* rankno;   // current items per rank
	int* maxrank;  // max items per rank. (unbounded)
	int ranksize;
}Rankings;


void init_ranks(void* ranklist, int ranksize);
void addRanking(void* ranklist, int ruleNum, int rank, int async);
int getRankAmt(void* ranklist, int rank);
int getRankRuleNum(void* ranklist, int randNum, int rank);
int getRankAsyncBool(void* ranklist, int randNum, int rank);
int getNoRanks(void* ranklist);
void resizeRank(void* ranklist, int rank);