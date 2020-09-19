#ifndef BUCKET_H
#define BUCKET_H

#include "diseases.h"

struct Bucket
{
    int entry_counter;
    bucket_entry* name;
    struct Bucket *next;
};

typedef struct Bucket *bucket_ptr;

bucket_ptr Create_Bucket(int);
void Insert_Bucket_Entry(bucket_ptr, rec_ptr, char*, char*, int);
void Destroy_Bucket_List(bucket_ptr*);
void Print_Bucket(bucket_ptr);
int Bucket_List_Size(bucket_ptr);
void Print_Hash_Tables(int, int , bucket_ptr* , bucket_ptr* );
int Search_Disease_Entry(bucket_ptr, char*);
int Num_of_Outbreaks(bucket_ptr, char*, char*, char*, char*,char**);
void Topk_Diseases(bucket_ptr*, int, int, char*, char*, char*);
void Topk_Countries(bucket_ptr*, int, int, char*, char*, char*);
void topk_AgeRanges(bucket_ptr*, int, int, char*, char*, char*, char*, char**);
int Num_of_Outbreaks(bucket_ptr, char*, char*, char*, char*, char**);
int Num_Patient_Admissions(bucket_ptr, char*, char*, char*, char*, char**);
int Num_Patient_Discharges(bucket_ptr, char*, char*, char*, char*, char**);

#endif