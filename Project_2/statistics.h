#ifndef STATISTICS_H
#define STATISTICS_H

//Node that keeps the disease name with the its number of outbreaks. 
struct Summary_Statistics
{
    char* disease; //name of disease
    int case_1; //ages: 0-20
    int case_2; //ages: 21-40
    int case_3; //ages: 40-60
    int case_4; //ages: 60+
    struct Summary_Statistics *next;
};

typedef struct Summary_Statistics *sumptr;

void Insert_Statistics_List(sumptr *, char*, int);
void Delete_Statistics_List(sumptr *);

#endif