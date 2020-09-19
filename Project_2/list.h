#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct List_Node {
   char* data;
   struct List_Node *next;
};

typedef struct List_Node *listptr;

void Print_List(listptr);
void Insert_List(listptr *, char*);
void Delete_List(listptr *);
int List_Size(listptr);
