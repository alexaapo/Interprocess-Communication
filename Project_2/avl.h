#ifndef AVL_H
#define AVL_H

#define max(A, B)  ((A) > (B) ? (A) : (B))
#include <time.h>
#include "record.h"
#include "statistics.h"

struct Same_Date_Node;
typedef struct Same_Date_Node* samedateptr;

struct AVL_Tree_Node
{
    char* date;
    struct AVL_Tree_Node *left;
    struct AVL_Tree_Node *right;
    samedateptr next_same_date;  //pointer to the list of the same dates (if exists)
    int height;
    rec_ptr rec;  //pointer to the record
};

typedef struct AVL_Tree_Node *avl_ptr;

avl_ptr AVL_Insert(avl_ptr, rec_ptr, char*);
void preOrder(avl_ptr);
void inOrder(avl_ptr ); 
void Delete_AVL_Tree(avl_ptr);
time_t date_to_seconds(char *);
time_t date_to_seconds1(const char *);
int Compare_Dates(const void *, const void *);
void AVL_Disease_Search(avl_ptr);
void AVL_Search_Outbreaks(avl_ptr);
void AVL_Search_Disease_Outbreaks(avl_ptr, char*, char*,char*);
void AVL_Num_of_Outbreaks_Disease(avl_ptr, char*);
void AVL_Num_of_Outbreaks_Countries(avl_ptr, char*);
void AVL_Search_Country_Outbreaks(sumptr*,avl_ptr, char*, char*, char*);

//List of the same dates
struct Same_Date_Node
{
    rec_ptr rec_node;
    struct Same_Date_Node* next;
};

int Print_Same_Date(avl_ptr);
int Destroy_Same_Keys(avl_ptr);
void Print_Same_Date_Disease(avl_ptr);
void AVL_Print_Disease(avl_ptr);
void Search_Same_Date_Disease(avl_ptr);
void Search_Same_Date_Outbreaks(avl_ptr);
void Search_Same_Date_Disease_Outbreaks(avl_ptr, char*, char*);
void Num_of_Same_Date_Outbreaks(avl_ptr, char*);
void Num_of_Same_Date_Outbreaks2(avl_ptr, char*);
void Search_Same_Date_Country_Outbreaks(sumptr*, avl_ptr, char*, char*);
void Statistics_AVL(avl_ptr, char*, char**);
void Same_Statistics_AVL(avl_ptr, sumptr*, char*, char**);
void AVL_Num_Patient_Admissions(sumptr*, avl_ptr, char*, char*, char*);
void AVL_Search_Disease_Discharges_Outbreaks(avl_ptr, char*, char*, char*);
void Search_Same_Date_Disease_Discharges_Outbreaks(avl_ptr, char*, char*, char*);
void AVL_Num_Patient_Discharges(sumptr*, avl_ptr, char*, char*, char*);
void Search_Same_Date_Num_Patient_Discharges(sumptr*,avl_ptr, char*, char*, char*);

#endif