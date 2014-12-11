typedef struct AllRBlock{
	int* ruleno;					//ruleno assigned in cpusim
	char*** blockrules;				//struct of rules stored
	int* currblocksize;			//blocksize of each ruleblock
	int* currmaxrblocksize;			//max blocksize of each ruleblock
	int allRBlockSize;				//this.size
	int maxAllRBlockSize;			//this.maxsize
} AllRBlock;

void init_ruleblock(void* rblockwrapper);
void addRuleToBlock(void* rblockwrapper, int ruleno, char* rule);
int getrblocksize(void* rblockwrapper, int ruleno);
char* getRuleFromBlock(void* rblockwrapper, int ruleno, int i);
int ruleNoToIndex(void* rblockwrapper, int ruleno);