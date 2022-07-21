#include <stdlib.h>
#include <math.h>
#include <string.h>


typedef struct Node
{
  char* item;
  int ht;
  struct Node* left;
  struct Node* right;
} Node;

int height(Node* T);

Node* CreateNode(char* it, Node* lft, Node* rgt){
  Node* n = (Node*) malloc(sizeof(Node));
  n->item  = it;
  n->left  = lft;
  n->right = rgt;
  if(height(lft) > height(rgt)){
    n->ht = 1 + height(lft);
  }
  else{
    n->ht = 1 + height(rgt);
  }

  return n;
}

int height(Node* T)
{
  if(T == NULL)
  {
    return 0;
  }
  else
  {
    return T->ht;
  }
}

void getHeight(Node* T)
{
  if(height(T->left) > height(T->right)){
    T->ht = 1 + height(T->left);
  }
  else{
    T->ht = 1 + height(T->right);
  }
}

void singleRotateLeft(Node** T)
{
  Node* r   = (*T)->right;
  (*T)->right = r->left;
  getHeight(*T);

  r->left  = *T;
  *T        = r;
  getHeight(*T);
}

void singleRotateRight(Node** T)
{
  Node* L   = (*T)->left;
  (*T)->left  = L->right;
  getHeight(*T);

  L->right = *T;
  *T        = L;
  getHeight(*T);
}

void doubleRotateLeft(Node** T)
{
  singleRotateRight(&(*T)->right);
  singleRotateLeft(T);
}

void doubleRotateRight(Node** T)
{
  singleRotateLeft(&(*T)->left);
  singleRotateRight(T);
}

void rotateLeft(Node** T)
{
  Node* r   = (*T)->right;
  int  zag = height(r->left);
  int  zig = height(r->right);

  if(zig > zag)
  {
    singleRotateLeft(T);
  }
  else
  {
    doubleRotateLeft(T);
  }
}

void rotateRight(Node** T)
{
  Node* L  = (*T)->left;
  int  zig = height(L->left);
  int  zag = height(L->right);

  if(zig > zag)
  {
    singleRotateRight(T);
  }
  else
  {
    doubleRotateRight(T);
  }
}

void rebalance(Node** T)
{
  int hl = height((*T)->left);
  int hr = height((*T)->right);

  if(hr > hl + 1)
  {
    rotateLeft(T);
  }
  else if(hl > hr + 1)
  {
    rotateRight(T);
  }
  else
  {
    getHeight(*T);
  }
}

void insert(char* x, Node** T)
{
  if(*T == NULL)
  {
    //printf("tree created!\n");
    *T = CreateNode(x, NULL, NULL);
  }
  else if(strcmp(x, (*T)->item) > 0)
  {
    //printf("left! ");
    insert(x, &((*T)->left));
    rebalance(T);
  }
  else if(strcmp(x, (*T)->item) < 0)
  {
    //printf("right! ");
    insert(x, &((*T)->right));
    rebalance(T);
  }
  else if(strcmp(x, (*T)->item) == 0){
    free(x);
  }
}

int search(char* x, Node* T){
  if(T == NULL){
    //printf("no Tree!\n");
    return 0;
  }
  else if(strcmp(x, T->item) == 0){
    //printf("dep found!\n");
    return 1;
  }
  else if(strcmp(x, T->item) > 0){
    //printf("x: %s, node: %s\n", x, T->item);
    return search(x, T->left);
  }
  else{
    //printf("x: %s, node: %s\n", x, T->item);
    return search(x, T->right);
  }
}

void deleteTree(Node* T){
  if(T == NULL){
    return;
  }
  deleteTree(T->left);
  deleteTree(T->right);
  free(T->item);
  free(T);
}