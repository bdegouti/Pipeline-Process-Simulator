#include<stdio.h>
#include<time.h>
#include<math.h>
#include<stdlib.h>
#include<unistd.h>
#include<assert.h>
#include<float.h>
#include <string.h>
#include "Instruction.c"
#include "BST.c"

#define INSTRUCTIONS_NUM 1000000
#define NUM_PER_RUN 5000000
#define NUMBER_OF_RUNS 6
#define MAX_DEPENDENCIES 10
#define NUM_PIPELINE_LEVELS 4
#define NUM_OF_STAGES 5

/*
this function generates a queue list from the file entered into it.
It will start at startingPoint and accept numberOfRuns lines. all instructions
prior to startingPoint are entered into a BST, which will be referenced in the case
of invalid dependencies.
*/
InstructionQueue* getQueueListFromFile(char* arg, long long int startingPoint, long long int numberOfRuns);

/*
this function is responsible for handling the core loop of the simulation, accepting new instructions and
ending when it reaches the end of the provided list.
*/
long long int Simulation(InstructionQueue* iq, int numOfPipes, long long int numberOfInstructions);

/*
this function performs the necessary actions for each instruction in the processor, and then moves each instruction forward.
it uses a BST to keep a hold of and track past instructions so as to allow instructions to check for dependencies.
*/
void processCurrentInstructions(Instruction** p[NUM_OF_STAGES], Node** is, int pipes, long long int* Departed);

/*
creates the matrix which serves as our "processor", with pipes determining the wdith. all values are initialized to NULL;
*/
Instruction*** createProcesser(int pipes);

/*delete processor*/
void deleteProcessor(Instruction*** p);

/*
empties processer
*/
void setProcessorToEmpty(Instruction*** p, int pipes);

/*
checks if processor empty, returns False if not
*/
boolean checkIfProcessorEmpty(Instruction*** p, int pipes);

/*
FOR DEBUGGING: prints stage in processor with instructions address and dependencies listed
*/
void printProcessor(Instruction*** p, int stage, int pipes);

/*
checks if processor empty at a given stage
*/
int checkIfProcessorEmptyAtStage(Instruction*** p, int stage, int pipes);

/*
prints histogram depicting number of types accepted as instructions throughout the simulation
*/
void printHistogram();