#include<stdlib.h>
#include<stdio.h>

typedef enum {None, Integer, Float, Branch, Load, Store} I_type;
typedef enum {False, True} boolean;

typedef struct Instruction{
    char* address;
    I_type type;
    char** dependencies;
    int numOfDependencies;
    struct Instruction* next;
} Instruction;

typedef struct InstructionQueue{
    Instruction* start;
    Instruction* end;
    long long int size;
} InstructionQueue;

typedef struct InstructionStack{
    Instruction* top;
    long long int size;
} InstructionStack;

Instruction* Instruction_Create(char* adddress, I_type type, char** dependencies, int numDep){
    Instruction* i = (Instruction*) malloc(sizeof(Instruction));
    i->address = adddress;
    i->dependencies = dependencies;
    i->numOfDependencies = numDep;
    i->type = type;
    return i;
}

void Instruction_Delete(Instruction* i){
    free(i->address);
    for(int k = 0; k < i->numOfDependencies; k++){
        free(i->dependencies[k]);
    }
    free(i->dependencies);
    free(i);
}

InstructionQueue* InstructionQueue_Create(){
    InstructionQueue *iq = (InstructionQueue*) malloc(sizeof(InstructionQueue));
    if(iq == NULL){
        printf("Error: malloc failed at InstructionQueue_Create");
        return NULL;
    }
    iq->end = NULL;
    iq->start = NULL;
    iq->size = 0;
    return iq;
}

void InstructionQueue_Delete(InstructionQueue* iq){
    if(iq->size > 0){
        Instruction *tmp, *e = iq->start;
        while(e != NULL){
            tmp = e;
            e = e->next;
            free(tmp);
        }
    }
    free(iq);
}

void InstructionQueue_Enqueue(InstructionQueue* iq, Instruction* i){
    if(iq->start == NULL){
        iq->start = i;
        iq->end = i;
        iq->size = 1;
        i->next = NULL;
        return;
    }
    iq->size++;
    iq->end->next = i;
    iq->end = i;
    i->next = NULL;
}

Instruction* InstructionQueue_Dequeue(InstructionQueue* iq){
    if(iq->start == NULL){
        return NULL;
    }
    if(iq->start == iq->end){
        Instruction* tmp = iq->start;
        iq->start = NULL;
        iq->end = NULL;
        iq->size = 0;
        return tmp;
    }
    iq->size--;
    Instruction* tmp = iq->start;
    iq->start = iq->start->next;
    return tmp;
}

InstructionStack* InstructionStack_Create(){
    InstructionStack *is = (InstructionStack*) malloc(sizeof(InstructionStack));
    if(is == NULL){
        printf("Error: malloc failed at InstructionStack_Create");
        return NULL;
    }
    is->top = NULL;
    is->size = 0;
    return is;
}

void InstructionStack_Add(InstructionStack* is, Instruction* i){
    if(is->top == NULL){
        is->top = i;
        i->next = NULL;
        is->size = 1;
        return;
    }
    i->next = is->top;
    is->top = i;
    is->size++;
}

Instruction* InstructionStack_Pop(InstructionStack* is){
    if(is->top == NULL){
        return NULL;
    }
    Instruction* tmp = is->top;
    is->top = is->top->next;
    is->size--;
    return tmp;
}

void InstructionStack_Delete(InstructionStack* is){
    Instruction* tmp;
    while((tmp = InstructionStack_Pop(is)) != NULL){
        Instruction_Delete(tmp);
    }
    free(is);
}

boolean InstructionStack_CheckDependencies(InstructionStack* is, Instruction* i){
    Instruction* tmp = is->top;
    int satisfiedDeps = 0;
    while(tmp != NULL && satisfiedDeps < i->numOfDependencies){
        for(int k = 0; k < i->numOfDependencies; k++){
            if(strcmp(i->dependencies[k], tmp->address) == 0){
                satisfiedDeps++;
            }
        }
        tmp = tmp->next;
    }
    if(satisfiedDeps >= i->numOfDependencies){
        return True;
    }
    return False;
}