all: proj2.c Instruction.c proj2.h proj2 BST.cpp
	gcc -g -Wall -o proj2 proj2.c -lm

proj2.o: 

clean:
	rm proj2