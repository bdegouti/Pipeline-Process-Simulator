#include "PipelineCalc.h"

long long int numByType[6] = {0, 0, 0, 0, 0 ,0};
boolean numByTypeComplete = False;
int pipelineLevels[4] = {1, 2, 3, 4};
Instruction* currBranch = NULL;
Node* InvalidDepend = NULL;

int main(int argc, char* argv[]){
    if(argc == 2){
        long long int sp[6] = {0, 5000000, 10000000, 15000000, 20000000, 25000000};
        double results[4];
        long long int numberOfRuns = 1000000;
        for(int p = 0; p < 4; p++){
            for(int k = 0; k < 6; k++){
                InstructionQueue* iq = getQueueListFromFile(argv[1], sp[k], numberOfRuns);
                long long int numOfCycles = Simulation(iq, pipelineLevels[p], numberOfRuns);
                results[p] += numOfCycles;
                InstructionQueue_Delete(iq);
                InvalidDepend = NULL;
                currBranch = NULL;
            }
            numByTypeComplete = True;
            results[p] = results[p]/6;
            printf("\n");
        }

        printf("1 pipe: %f, 2 pipes: %f, 3 pipes: %f, 4 pipes: %f\n", results[0], results[1], results[2], results[3]);
        printf("int:%lld flt:%lld brnch:%lld ld:%lld str:%lld\n", numByType[1], numByType[2], numByType[3], numByType[4], numByType[5]);
    }
    else if(argc == 5){
        long long int startingPoint = strtoull(argv[2], NULL, 0);
        long long int numberOfRuns = strtoull(argv[3], NULL, 0);
        InstructionQueue* iq = getQueueListFromFile(argv[1], startingPoint, numberOfRuns);
        int numberOfPipes = atoi(argv[4]);
        long long int numOfCycles = Simulation(iq, numberOfPipes, numberOfRuns);
        printf("there were %lld cycles for this run\n\n", numOfCycles);
        printHistogram();
        InstructionQueue_Delete(iq);
    }
    else{
        printf("Error: Invalid number of arguments recieved\n");
    }
}

InstructionQueue* getQueueListFromFile(char* arg, long long int startingPoint, long long int numberOfRuns){
    FILE* instructions = fopen(arg, "r");
    if(instructions == NULL){
        printf("Error: could not open File\n");
        exit(1);
    }
    
    ssize_t s;
    size_t len = 1024;
    char* line = (char*) malloc(sizeof(char)*len);
    char delim[] = ",\n";
    InstructionQueue* iq = InstructionQueue_Create();
    for(long long int i = 0; i < startingPoint; i++){
        getline(&line, &len, instructions);
        char* token = strtok(line, delim);
        char* addr = (char*) malloc(sizeof(char)*strlen(token));
        strcpy(addr, token);
        insert(addr, &InvalidDepend);
    }
    long long int lineNum = 0;
    while(lineNum < numberOfRuns && (s = getline(&line, &len, instructions))!= -1){
        char* addr;
        I_type i;
        char** depend = (char**) malloc(sizeof(char*)*MAX_DEPENDENCIES);
        char* token;
        int el = 1, numDep = 0;
        token = strtok(line, delim);
        addr = (char*) malloc(sizeof(char)*strlen(token));
        strcpy(addr, token);
        while((token = strtok(NULL, delim))!=NULL){
            if(el == 1){
                i = strtoull(token, NULL, 0);
            }
            else if(el < MAX_DEPENDENCIES){
                numDep++;
                depend[el-2] = (char*) malloc(sizeof(char)*strlen(token));
                strcpy(depend[el-2], token);
            }
            else{
                break;
            }
            el++;
        }
        Instruction* ni = Instruction_Create(addr, i, depend, numDep);
        InstructionQueue_Enqueue(iq, ni);
        lineNum++;
    }

    free(line);
    if(fclose(instructions)){
            printf("Error: could not close file\n");
            exit(1);
    }
    return iq;
}

/*
OF NOTE--due to an minor error in this function, the count of cycles in the report were off, by often. 
This error has since been fixed, but due to time constraints I will not be able to redo the report with the correct
*/
long long int Simulation(InstructionQueue* iq, int numOfPipes, long long int numberOfInstructions){
    Instruction*** processer = createProcesser(numOfPipes);
    Node* completedInstr = NULL;
    setProcessorToEmpty(processer, numOfPipes);
    long long int Arrived = 0, Departed = 0, numberOfCycles = 0;
    while(Departed < numberOfInstructions){
        if(Arrived < numberOfInstructions){
            //fetch
            for(int k = 0; k < numOfPipes && currBranch == NULL && Arrived < numberOfInstructions; k++){
                if(processer[0][k] == NULL){
                    Arrived++;
                    processer[0][k] = InstructionQueue_Dequeue(iq);
                    
                    if(!numByTypeComplete){
                        numByType[processer[0][k]->type]++;
                    }
                    if(processer[0][k]->type == Branch){
                        currBranch = processer[0][k];
                    }
                }
            }
            /*
            printf("\ncycle %lld\n", numberOfCycles);
            printf("\nIF\n");
            printProcessor(processer, 0, numOfPipes);
            printf("\nID\n");
            printProcessor(processer, 1, numOfPipes);
            printf("\nEX\n");
            printProcessor(processer, 2, numOfPipes);
            printf("\nMEM\n");
            printProcessor(processer, 3, numOfPipes);
            printf("\nWB\n");
            printProcessor(processer, 4, numOfPipes);
            */
        }
        //perform cycle
        processCurrentInstructions(processer, &completedInstr, numOfPipes, &Departed);
        numberOfCycles++;
    }
    //printf("Integer: %lld, Floating Point: %lld, Branch: %lld, Load: %lld, Save: %lld\n", numByType[1], numByType[2], numByType[3], numByType[4], numByType[5]);
    deleteTree(completedInstr);
    deleteTree(InvalidDepend);
    deleteProcessor(processer);
    return numberOfCycles;
}

void processCurrentInstructions(Instruction*** p, Node** is, int pipes, long long int* Departed){
    int EX[6] = {1, 1, 1, 1, 1, 1};
    for(int s = NUM_OF_STAGES-1; s >= 0; s--){
        for(int k = 0; k < pipes; k++){
            switch(s) //BYPASS OR NO?
            {   
                case 0: //IF
                    if(p[s][k] != NULL && p[s+1][k] == NULL){
                        if(p[s][k]->type == Branch){
                            currBranch = p[s][k];
                        }
                        p[s+1][k] = p[s][k];
                        p[s][k] = NULL;
                    }
                    break;
                case 1: //ID
                    if(p[s][k] != NULL && p[s+1][k] == NULL){
                        if(p[s][k]->numOfDependencies == 0){
                            p[s+1][k] = p[s][k];
                            p[s][k] = NULL;
                        }
                        else{
                            int satisfiedDeps = 0;
                            //checks if dependecies are satisfied
                            for(int l = 0; l < p[s][k]->numOfDependencies; l++){
                                satisfiedDeps += search(p[s][k]->dependencies[l], *is);
                            }
                            if(satisfiedDeps == p[s][k]->numOfDependencies){
                                p[s+1][k] = p[s][k];
                                p[s][k] = NULL;
                            }
                            else{
                                //if not, checks if any of those dependencies are out of execution scope
                                int invalidDeps = 0;
                                for(int l = 0; l < p[s][k]->numOfDependencies; l++){
                                    invalidDeps += search(p[s][k]->dependencies[l], InvalidDepend);
                                }
                                if(satisfiedDeps >= (p[s][k]->numOfDependencies-invalidDeps)){
                                    p[s+1][k] = p[s][k];
                                    p[s][k] = NULL;
                                }
                            }
                        }
                    }
                    break;
                case 2: //EX
                    if(p[s][k] != NULL && p[s+1][k] == NULL){
                        if(EX[p[s][k]->type] == True){
                            EX[p[s][k]->type] = False;
                            
                            if(currBranch != NULL && strcmp(p[s][k]->address, currBranch->address) == 0){
                                currBranch = NULL;
                            }
                            
                            p[s+1][k] = p[s][k];
                            p[s][k] = NULL;
                        }
                        else{
                            int highestEmpty = checkIfProcessorEmptyAtStage(p, 2, pipes);
                            if(highestEmpty != k){
                                p[s][highestEmpty] = p[s][k];
                                p[s][k] = NULL;
                            }
                        }
                    }
                    break;
                case 3: //MEM
                    if(p[s][k] != NULL && p[s+1][k] == NULL){
                        p[s+1][k] = p[s][k];
                        p[s][k] = NULL;
                    }
                    break;
                case 4: //WB
                    if(p[s][k] != NULL){
                        char* depend = (char*) malloc(sizeof(char)*strlen(p[s][k]->address));
                        strcpy(depend, p[s][k]->address);
                        insert(depend, is);
                        Instruction_Delete(p[s][k]);
                        *Departed = *Departed + 1;
                        p[s][k] = NULL;
                    }
                    break;
            }
        }
    }
    //printf("\n");
}

Instruction*** createProcesser(int pipes){
    Instruction*** p = (Instruction***) malloc(sizeof(Instruction**)*NUM_OF_STAGES);
    for(int s = 0; s < NUM_OF_STAGES; s++){
        p[s] = (Instruction**) malloc(sizeof(Instruction*)*pipes);
    }
    return p;
}

void deleteProcessor(Instruction*** p){
    for(int s = 0; s < NUM_OF_STAGES; s++){
        free(p[s]);
    }
    free(p);
}
void setProcessorToEmpty(Instruction*** p, int pipes){
    for(int y = 0; y < pipes; y++){
        for(int x = 0; x < NUM_OF_STAGES; x++){
            p[x][y] = NULL;
        }
    }
}

boolean checkIfProcessorEmpty(Instruction*** p, int pipes){
    for(int y = 0; y < pipes; y++){
        for(int x = 0; x < NUM_OF_STAGES; x++){
            if(p[x][y] != NULL){
                return False;
            }
        }
    }
    return True;
}

int checkIfProcessorEmptyAtStage(Instruction*** p, int stage, int pipes){
    for(int i = 0; i < pipes; i++){
        if(p[stage][i] == NULL){
            return i;
        }
    }
    return -1;
}

void printProcessor(Instruction*** p, int stage, int pipes){
    for(int i = 0 ; i < pipes; i++){
        if(p[stage][i] == NULL){
            printf("NULL\n");
        }
        else{
            printf("addr: %s, dependencies ", p[stage][i]->address);
            for(int k = 0; k < p[stage][i]->numOfDependencies; k++){
                printf("%s ", p[stage][i]->dependencies[k]);
            }
            printf("\n");
        }
    }
}

void printHistogram(){
    const int HIST_SIZE = 10;
    int total = 0;
    for(int i = 0; i < 6; i++){
        total += numByType[i];
    }

    for(int i = HIST_SIZE; i > 0; i--){
        if(i == HIST_SIZE){
            printf("100%%");
        }
        else{
            printf("   ");
        }
        for(int k = 1; k < 6; k++){
            if(((((double)numByType[k])/total)*HIST_SIZE)>=i){
                printf(" #");
            }
            else{
                printf("  ");
            }
            if(k == 5){
                printf("\n");
            }
            else{
                printf("   ");
            }
        }
    }
    printf("0%% int  flt  brch  ld  str\n\n");
    printf("int: %.2f%%\nflt: %.2f%%\nbrnch: %.2f%%\nld: %.2f%%\nstr: %.2f%%\n", ((double)numByType[1]/total)*100, ((double)numByType[2]/total)*100, ((double)numByType[3]/total)*100, ((double)numByType[4]/total)*100, ((double)numByType[5]/total)*100);
    for(int i = 0; i < 6; i++){
        numByType[i] = 0;
    }
}