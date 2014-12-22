# Make file that builds executable file cpusim.exe
# FLAGS=  -pedantic -W -Wall -Wextra -O2

CC= g++
FLAGS=  -pedantic -W -Wall -Wextra -O2 -Wno-write-strings

cpusim.exe : cpusim-main.o cpusim-lib.o hashtable-lib.o ranking-lib.o ruleblock-lib.o getline.o booleval-lib.o
		$(CC) $(FLAGS) -o cpusim.exe cpusim-main.o cpusim-lib.o hashtable-lib.o ranking-lib.o ruleblock-lib.o getline.o booleval-lib.o

cpusim-main.o : cpusim-main.c cpusim-lib.h hashtable-lib.h ranking-lib.h ruleblock-lib.h getline.h booleval-lib.h
		$(CC) $(FLAGS) -c cpusim-main.c
	
cpusim-lib.o : cpusim-lib.c cpusim-lib.h hashtable-lib.h ranking-lib.h ruleblock-lib.h getline.h booleval-lib.h
		$(CC) $(FLAGS) -c cpusim-lib.c

hashtable-lib.o : hashtable-lib.c hashtable-lib.h
		$(CC) $(FLAGS) -c hashtable-lib.c

ranking-lib.o : ranking-lib.c ranking-lib.h
		$(CC) $(FLAGS) -c ranking-lib.c

ruleblock-lib.o : ruleblock-lib.c ruleblock-lib.h
		$(CC) $(FLAGS) -c ruleblock-lib.c
		
getline.o : getline.c getline.h
		$(CC) $(FLAGS) -c getline.c

booleval-lib.o : booleval-lib.c booleval-lib.h
		$(CC) $(FLAGS) -c booleval-lib.c
clean:
	rm -rf *.o cpusim.exe
