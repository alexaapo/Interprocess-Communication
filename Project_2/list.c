#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"

void Print_List(listptr head) 
{
    listptr current = head;

    while (current != NULL) {
        printf("%s\n", current->data);
        current = current->next;
    }
}


void Insert_List(listptr *head, char* data) 
{
    listptr current = *head, new_node;

    if(current == NULL)
    {
        new_node = (listptr)malloc(sizeof(struct List_Node));
        new_node->data = (char*)malloc((strlen(data)+1)*sizeof(char));
        strcpy(new_node->data,data);
        new_node->next = NULL;

        *head = new_node;
        return;
    }

    if(current->next == NULL)
    {
        if(strcmp(current->data,data) == 0)  return;
    }
    
    while(current->next != NULL)
    { 
        if(strcmp(current->data,data) == 0)  return;
        current = current->next;
    }

    if(strcmp(current->data,data) == 0)  return;

    new_node = (listptr)malloc(sizeof(struct List_Node));
    new_node->data = (char*)malloc((strlen(data)+1)*sizeof(char));
    strcpy(new_node->data,data);
    new_node->next = NULL;

    current->next = new_node;
}

void Delete_List(listptr *head)
{
    listptr current = *head;
    listptr next; 

    if(current->next == NULL)
    {
        free(current->data);
        free(current);
        *head = NULL; 
        return;
    }
  
    while(current != NULL)  
    { 
        next = current->next; 
        free(current->data);
        free(current); 
        current = next; 
    } 
    
    *head = NULL; 
}

int List_Size(listptr head) 
{ 
    int count = 0;   
    listptr current = head;  
    while (current != NULL) 
    { 
        count++; 
        current = current->next; 
    } 
    return count; 
} 

