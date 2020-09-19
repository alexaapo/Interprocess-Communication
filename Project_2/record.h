#ifndef RECORD_H
#define RECORD_H

#include <stdio.h>
// #include "enum.h"

struct Record_Node
{
    char* recordID;
    char* patientFirstName;
    char* patientLastName;
    char* diseaseID;
    char* country;
    char* entryDate;
    char* exitDate;
    char* age;
    struct Record_Node *next;
};

typedef struct Record_Node *rec_ptr;

int Invalid_Date(char*);
int Invalid_Name(char*);
int Invalid_Disease(char*);
int Check_Validity_of_Record(rec_ptr* , char* , char* , char* , char*, char* , char* , char* , char* );
rec_ptr Insert_Record(rec_ptr*,char* , char* , char*, char* , char* , char* , char* , char* );
void Print_Error(int);
void Delete_Record_List(rec_ptr*);
void Print_Record(rec_ptr);
int Search_ID(rec_ptr, char*, char*);
int Size_of_List(rec_ptr);
int is_number(char*);
int Invalid_Record(char*);

#endif