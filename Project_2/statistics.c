#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "statistics.h"

//Function to Insert or update the nodes of Statistics per disease.
void Insert_Statistics_List(sumptr *head, char* disease, int age) 
{
    sumptr current = *head, new_node;

    //In case of null list
    if(*head == NULL)
    {
        new_node = (sumptr)malloc(sizeof(struct Summary_Statistics));
        new_node->disease = (char*)malloc((strlen(disease)+1)*sizeof(char));
        strcpy(new_node->disease,disease);

        new_node->case_1 = 0;
        new_node->case_2 = 0;
        new_node->case_3 = 0;
        new_node->case_4 = 0;

        if (age <= 20)  new_node->case_1++;
        else if(age <= 40)  new_node->case_2++;
        else if(age <= 60)  new_node->case_3++;
        else  new_node->case_4++;

        new_node->next = NULL;

        *head = new_node;
        return;
    }

    //Check every node for finding the name of disease, so just update the cases
    while(current->next != NULL)
    { 
        if(strcmp(current->disease,disease) == 0)
        {  
            if (age <= 20)  current->case_1++;
            else if(age <= 40)  current->case_2++;
            else if(age <= 60)  current->case_3++;
            else  current->case_4++;
            return;
        }
        current = current->next;
    }

    if(strcmp(current->disease,disease) == 0)
    {  
        if (age <= 20)  current->case_1++;
        else if(age <= 40)  current->case_2++;
        else if(age <= 60)  current->case_3++;
        else  current->case_4++;
        return;
    }

    //If there is no node with disease name then create a new one 
    new_node = (sumptr)malloc(sizeof(struct Summary_Statistics));
    
    new_node->disease = (char*)malloc((strlen(disease)+1)*sizeof(char));
    strcpy(new_node->disease,disease);

    new_node->case_1 = 0;
    new_node->case_2 = 0;
    new_node->case_3 = 0;
    new_node->case_4 = 0;

    if (age <= 20)  new_node->case_1++;
    else if(age <= 40)  new_node->case_2++;
    else if(age <= 60)  new_node->case_3++;
    else  new_node->case_4++;

    new_node->next = NULL;

    current->next = new_node;
}

//Deallocate the memory of Statistics list
void Delete_Statistics_List(sumptr *head)
{
    sumptr current = *head;
    sumptr next; 

    if(current->next == NULL)
    {
        free(current->disease);
        free(current);
        *head = NULL; 
        return;
    }
  
    while(current != NULL)  
    { 
        next = current->next; 
        free(current->disease);
        free(current); 
        current = next; 
    } 
    
    *head = NULL; 
}

// int main(void) 
// {
//     sumptr head = NULL,temp;
    
//     Insert_Statistics_List(&head,"EVD",10);
//     Insert_Statistics_List(&head,"EVD",10);
//     Insert_Statistics_List(&head,"EVD",10);
//     Insert_Statistics_List(&head,"EVD",10);
//     Insert_Statistics_List(&head,"EVD",10);
//     Insert_Statistics_List(&head,"COVID-2019",10);
//     Insert_Statistics_List(&head,"PSORA",10);
//     Insert_Statistics_List(&head,"SARS",10);
//     Insert_Statistics_List(&head,"COVID-2018",10);

//     temp=head;
//     while(temp!=NULL)
//     {
//         printf("%s %d %d %d %d\n",temp->disease,temp->case_1,temp->case_2,temp->case_3,temp->case_4);
//         temp=temp->next;
//     }
//     Delete_Statistics_List(&head);
// }