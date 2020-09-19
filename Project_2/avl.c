#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#define __USE_XOPEN 
#include <time.h>
#include "avl.h"

int counter;

//Print the list of same dates. 
int Print_Same_Date(avl_ptr root)
{
    int count = 1;
    samedateptr node = root->next_same_date;
    if(root->next_same_date != NULL)
    {
        node = root->next_same_date;
        count++;
        
        while(node != NULL)
        {
            printf("%s %s %s %s\n", node->rec_node->entryDate, node->rec_node->exitDate, node->rec_node->country, node->rec_node->patientFirstName);
            count++;
            node = node->next;
        }
    }
    return count;
}

//Deallocate the list of same dates (if exists).
int Destroy_Same_Keys(avl_ptr root)
{
    samedateptr node = root->next_same_date;
    samedateptr next;
    
    if(node == NULL) return 1;
    
    while(node != NULL)  
    {  
        next = node->next;  
        free(node);  
        node = next;  
    }  
      
    root->next_same_date = NULL;  
    return 0;    
}

//Convert the date to Unix timestamps (in seconds).
//Every timestamp represent a unique date.
time_t date_to_seconds(char *date)
{
        struct tm storage = {0,0,0,0,0,0,0,0,0};
        char *p = NULL;
        time_t seconds = 0;

        p = (char *)strptime(date,"%d-%m-%Y",&storage);
        if(p == NULL)
        {
                seconds = 0;
        }
        else
        {
                seconds = mktime(&storage);
        }
        return seconds;
}

time_t date_to_seconds1(const char *date)
{
        struct tm storage = {0,0,0,0,0,0,0,0,0};
        char *p = NULL;
        time_t seconds = 0;

        p = (char *)strptime(date,"%d-%m-%Y",&storage);
        if(p == NULL)
        {
                seconds = 0;
        }
        else
        {
                seconds = mktime(&storage);
        }
        return seconds;
}

int Compare_Dates(const void *date1, const void *date2)
{
        // const char *dt1 = (const char *)date1;
        // const char *dt2 = (const char *)date2;

        const char **dt1 = (const char **)date1;
        const char **dt2 = (const char **)date2;

        // printf("%s %s\n", *dt1,*dt2);

        if(date_to_seconds1(*dt1)>date_to_seconds1(*dt2))
                return 1;
        if(date_to_seconds1(*dt1)<date_to_seconds1(*dt2))
                return -1;
        else
                return 0;
}

//Returns the hight of the tree node.
int height(avl_ptr tree_node)
{
    if(tree_node == NULL)
        return 0;
    else 
        return tree_node->height;
}

//Create a new tree node with a given key.
avl_ptr Create_Tree_Node(char* date, rec_ptr rec_node)
{
    avl_ptr node = (avl_ptr)malloc(sizeof(struct AVL_Tree_Node));
    node->date = (char*)malloc(strlen(date)+1);
    strcpy(node->date,date);
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    node->rec = rec_node;
    node->next_same_date = NULL;
    return node;
}

//Do a right rotation.
avl_ptr Right_Rotation(avl_ptr root)
{
    avl_ptr left = root->left;
    avl_ptr right = left->right;

    //Do the rotation.
    left->right = root;
    root->left = right;

    //Update heights
    root->height = max(height(root->left), height(root->right))+1;
    left->height = max(height(left->left), height(left->right))+1;
    return left;
}

//Do a left rotation.
avl_ptr Left_Rotation(avl_ptr root)
{
    avl_ptr right = root->right;
    avl_ptr left = right->left;

    //Do the rotation.
    right->left = root;
    root->right = left;

    //Update heights
    root->height = max(height(root->left), height(root->right))+1;
    right->height = max(height(right->left), height(right->right))+1;
    return right;
}

//Find the balance factor of a given tree node.
int Get_Balance_Factor(avl_ptr node)
{
    if(node == NULL)
        return 0;
    else 
        return (height(node->left)-height(node->right));
}

//Create and insert a node with same date at the end of the list.
void Create_Same_Date_Node(avl_ptr root, rec_ptr rec_node)
{
    samedateptr temp = root->next_same_date;
    samedateptr new_node = (samedateptr)malloc(sizeof(struct Same_Date_Node));
    if(temp == NULL)
    {
        root->next_same_date = new_node;
        new_node->next = NULL;
        new_node->rec_node = rec_node;
        return;
    }
    else
    {   while(temp->next != NULL) 
            temp = temp->next;
    }
    temp->next = new_node;
    new_node->rec_node = rec_node;
    new_node->next = NULL;
}

//Insert a new date in the tree with recursive function and return the new root.
avl_ptr AVL_Insert(avl_ptr root, rec_ptr rec_node, char* date)
{
    //BST inertion.
    if(root == NULL)  
        return Create_Tree_Node(date,rec_node);
    if(date_to_seconds(date)<date_to_seconds(root->date))
        root->left = AVL_Insert(root->left,rec_node, date);
    else if(date_to_seconds(date)>date_to_seconds(root->date))
        root->right = AVL_Insert(root->right,rec_node,date);
    else 
    {
        Create_Same_Date_Node(root,rec_node);
        return root;
    }

    //Update height
    root->height = max(height(root->left),height(root->right))+1;

    //With the balance factor check if the balance of the tree is ok.
    int balance_factor = Get_Balance_Factor(root);

    //Case 1: Left Left case, y is left child of z and x is left child of y, so we need a simple right rotation.
    if(balance_factor > 1 && (date_to_seconds(date)<date_to_seconds(root->left->date)))
        return Right_Rotation(root);
  
    //Case 2: Right right case, y is right child of z and x is right child of y, so we need a simple lrft rotation.
    if(balance_factor < -1 && (date_to_seconds(date)>date_to_seconds(root->right->date)))
        return Left_Rotation(root);
    
    //Case 3: Left Right case, y is left child of z and x is right child of y, so we need a left rotation to y and right rotation to z.
    if(balance_factor > 1 && (date_to_seconds(date)>date_to_seconds(root->left->date)))
    {
        root->left = Left_Rotation(root->left);
        return Right_Rotation(root);
    }
    
    //Case 4: Right left case, y is right child of z and x is left child of y, so we need a right rotation to y and left rotation to z.
    if(balance_factor < -1 && (date_to_seconds(date)<date_to_seconds(root->right->date)))
    {
        root->right = Right_Rotation(root->right);
        return Left_Rotation(root);
    }
    return root;
}

void preOrder(avl_ptr root) 
{ 
    if(root != NULL) 
    { 
        printf("%s ", root->date); 
        preOrder(root->left); 
        preOrder(root->right); 
    } 
} 

void inOrder(avl_ptr root) 
{ 
    if(root != NULL) 
    { 
        inOrder(root->left); 
        printf("%s %s %s %s\n", root->date, root->rec->exitDate, root->rec->country, root->rec->patientFirstName);
        Print_Same_Date(root);
        inOrder(root->right);         
    } 
} 

//Deallocate all the AVL Tree and the list of the same keys (if there are exists).
void Delete_AVL_Tree(avl_ptr root)
{
    if (root == NULL)
        return;	
	Delete_AVL_Tree(root->left);
	Delete_AVL_Tree(root->right);
    while(!Destroy_Same_Keys(root));
    free(root->date);
    free(root);
}

void Print_Same_Date_Disease(avl_ptr root)
{
    samedateptr node = root->next_same_date;
    if(root->next_same_date != NULL)
    {
        node = root->next_same_date;
        
        while(node != NULL)
        {
            if(strcmp(node->rec_node->exitDate,"-") == 0)
                counter++;//printf("%s %s %s %s %s %s %s\n", node->rec_node->recordID, node->rec_node->patientFirstName, node->rec_node->patientLastName, node->rec_node->diseaseID, node->rec_node->country, node->rec_node->entryDate, node->rec_node->exitDate);
            node = node->next;
        }
    }
}

//For /numCurrentPatients returns the number of outbreaks who are still hospitallized. (if the disease is given)
void AVL_Print_Disease(avl_ptr root)
{
    if(root != NULL) 
    { 
        AVL_Print_Disease(root->left);
        if(strcmp(root->rec->exitDate,"-") == 0)
            counter++;
        Print_Same_Date_Disease(root);
        AVL_Print_Disease(root->right);         
    } 
}

void Search_Same_Date_Disease(avl_ptr root)
{
    samedateptr node = root->next_same_date;
    if(root->next_same_date != NULL)
    {
        node = root->next_same_date;
        
        while(node != NULL)
        {
            if(strcmp(node->rec_node->exitDate,"-") == 0)  counter++;
            node = node->next;
        }
    }
}

//For /numCurrentPatients returns the number of outbreaks who are still hospitallized. (if the disease is not given)
void AVL_Disease_Search(avl_ptr root)
{
    if(root != NULL) 
    { 
        AVL_Disease_Search(root->left);
        if(strcmp(root->rec->exitDate,"-") == 0)  counter++;
        Search_Same_Date_Disease(root);
        AVL_Disease_Search(root->right);         
    } 
}

void Search_Same_Date_Outbreaks(avl_ptr root)
{
    samedateptr node = root->next_same_date;
    if(root->next_same_date != NULL)
    {
        node = root->next_same_date;
        
        while(node != NULL)
        {
            counter++;
            node = node->next;
        }
    }
}

//For /globalDiseaseStats (if entry and exit date are not given)
void AVL_Search_Outbreaks(avl_ptr root)
{
    if(root != NULL) 
    { 
        AVL_Search_Outbreaks(root->left);
        counter++;
        Search_Same_Date_Outbreaks(root);
        AVL_Search_Outbreaks(root->right);         
    } 
}

//Returns the number of outbreaks who was diagnosed with the Disease in a specific date (and in specific country if exists) for the same date nodes (if exist)
void Search_Same_Date_Disease_Outbreaks(avl_ptr root, char* exit_date, char* country)
{
    samedateptr node = root->next_same_date;
    if(root->next_same_date != NULL)
    {
        node = root->next_same_date;
        
        while(node != NULL)
        {
            if(strcmp(exit_date,"--") !=0)
            {
                if(date_to_seconds(node->rec_node->entryDate) <= date_to_seconds(exit_date)) 
                {
                    if(country != NULL)  
                    {
                        if(strcmp(node->rec_node->country,country) == 0)  counter++;
                    }
                    else  
                    {
                        counter++;
                    }
                }
            }
            else
            {
                if(country != NULL)  
                {
                    if(strcmp(node->rec_node->country,country) == 0)  counter++;
                }
                else  
                {
                    counter++;
                }
            }
            
            node = node->next;
        }
    }
}

//Insert to the list in case of same date nodes
void Search_Same_Date_Country_Outbreaks(sumptr* head, avl_ptr root, char* exit_date, char* disease)
{
    samedateptr node = root->next_same_date;
    if(root->next_same_date != NULL)
    {
        node = root->next_same_date;
        
        while(node != NULL)
        {
            if(strcmp(exit_date,"--") !=0)
            {
                if(date_to_seconds(node->rec_node->entryDate) <= date_to_seconds(exit_date)) 
                {
                    if(disease != NULL)  
                    {
                        if(strcmp(node->rec_node->diseaseID,disease) == 0)  Insert_Statistics_List(head,disease,atoi(root->rec->age));
                    }
                    else  
                    {
                        counter++;
                    }
                }
            }
            else
            {
                if(disease != NULL)  
                {
                    if(strcmp(node->rec_node->diseaseID,disease) == 0)  counter++;
                }
                else  
                {
                    counter++;
                }
            }
            
            node = node->next;
        }
    }
}

//Returns the number of outbreaks who was diagnosed with the Disease in a specific date (and in specific country if exists)
void AVL_Search_Disease_Outbreaks(avl_ptr root, char* entry_date, char* exit_date, char* country)
{
    if(root == NULL) return;
    if(date_to_seconds(root->date) >= date_to_seconds(entry_date))    
    {
        if(strcmp(exit_date,"--") != 0)
        {
            if(date_to_seconds(root->date) <= date_to_seconds(exit_date))
            {
                if(country != NULL)  
                {
                    if(strcmp(root->rec->country,country) == 0)
                        counter++;
                }
                else  
                {
                    counter++;
                }
                Search_Same_Date_Disease_Outbreaks(root,exit_date,country);  //Check for same dates in the list(if they exist)
            }
        }
        else
        {
            if(country != NULL)  
            {
                if(strcmp(root->rec->country,country) == 0)  counter++;
            }
            else  
            {
                counter++;
            }
            Search_Same_Date_Disease_Outbreaks(root,exit_date,country);            
        }
        AVL_Search_Disease_Outbreaks(root->right,entry_date,exit_date,country);
        AVL_Search_Disease_Outbreaks(root->left,entry_date,exit_date,country);  
    }
    if(date_to_seconds(root->date) < date_to_seconds(entry_date))  AVL_Search_Disease_Outbreaks(root->right,entry_date,exit_date,country);
}

//Do the same for the same date nodes
void Search_Same_Date_Disease_Discharges_Outbreaks(avl_ptr root, char* entry_date, char* exit_date, char* country)
{
    samedateptr node = root->next_same_date;
    if(root->next_same_date != NULL)
    {
        node = root->next_same_date;
        
        while(node != NULL)
        {
            if(strcmp(node->rec_node->exitDate,"--") !=0)
            {
                if((date_to_seconds(entry_date) <= date_to_seconds(node->rec_node->exitDate)) && (date_to_seconds(node->rec_node->exitDate) <= date_to_seconds(exit_date))) 
                {
                    if(country != NULL)  
                    {
                        if(strcmp(node->rec_node->country,country) == 0)  counter++;
                    }
                }
            }
            
            node = node->next;
        }
    }
}

//Returns the number of outbreaks who was diagnosed with the Disease in a specific exit date (and in specific country if exists)
//Now I display the tree with in order, because i don't mind about the endtry date nodes of the tree anymore
void AVL_Search_Disease_Discharges_Outbreaks(avl_ptr root, char* entry_date, char* exit_date, char* country)
{
    if(root != NULL) 
    { 
        AVL_Search_Disease_Discharges_Outbreaks(root->left,entry_date,exit_date,country); 
        if(strcmp(root->rec->exitDate,"--") !=0)
        {
            if((date_to_seconds(entry_date) <= date_to_seconds(root->rec->exitDate)) && (date_to_seconds(root->rec->exitDate) <= date_to_seconds(exit_date)))
            {
                if(country != NULL)  
                {
                    if(strcmp(root->rec->country,country) == 0)
                        counter++;
                }
                Search_Same_Date_Disease_Discharges_Outbreaks(root,entry_date,exit_date,country);
            }
        }

        AVL_Search_Disease_Discharges_Outbreaks(root->right,entry_date,exit_date,country);         
    } 
}

//Insert to the summary statistics list the name of the country and the age of each record of same date nodes if exist
void Search_Same_Date_Num_Patient_Admissions(sumptr* head,avl_ptr root, char* exit_date, char* country)
{
    samedateptr node = root->next_same_date;
    if(root->next_same_date != NULL)
    {
        node = root->next_same_date;
        
        while(node != NULL)
        {
            if(strcmp(exit_date,"--") !=0)
            {
                if(date_to_seconds(node->rec_node->entryDate) <= date_to_seconds(exit_date)) 
                {
                    Insert_Statistics_List(head,node->rec_node->country,atoi(root->rec->age));
                }
            }
            
            node = node->next;
        }
    }
}

//Insert to the summary statistocs list the name of the country and the age of each record 
void AVL_Num_Patient_Admissions(sumptr* head, avl_ptr root, char* entry_date, char* exit_date, char* country)
{
    if(root == NULL) return;
    if(date_to_seconds(root->date) >= date_to_seconds(entry_date))    
    {
        if(strcmp(exit_date,"--") != 0)
        {
            if(date_to_seconds(root->date) <= date_to_seconds(exit_date))
            {
                Insert_Statistics_List(head,root->rec->country,atoi(root->rec->age));
                
                Search_Same_Date_Num_Patient_Admissions(head,root,exit_date,country); //Do the same for the same date nodes
            }
        }
        AVL_Num_Patient_Admissions(head,root->right,entry_date,exit_date,country);
        AVL_Num_Patient_Admissions(head,root->left,entry_date,exit_date,country);  
    }
    if(date_to_seconds(root->date) < date_to_seconds(entry_date))  AVL_Num_Patient_Admissions(head,root->right,entry_date,exit_date,country);
}

//Do the same for the same date nodes
void Search_Same_Date_Num_Patient_Discharges(sumptr* head,avl_ptr root, char* entry_date, char* exit_date, char* country)
{
    samedateptr node = root->next_same_date;
    if(root->next_same_date != NULL)
    {
        node = root->next_same_date;
        
        while(node != NULL)
        {
            if(strcmp(node->rec_node->exitDate,"--") !=0)
            {
                if((date_to_seconds(entry_date) <= date_to_seconds(node->rec_node->exitDate)) && (date_to_seconds(node->rec_node->exitDate) <= date_to_seconds(exit_date))) 
                {
                    Insert_Statistics_List(head,node->rec_node->country,atoi(root->rec->age));
                }
            }
            
            node = node->next;
        }
    }
}

//Returns the number of outbreaks who was diagnosed with the Disease in a specific exit date (and in specific country if exists)
//Now I display the tree with in order, because i don't mind about the endtry date nodes of the tree anymore
void AVL_Num_Patient_Discharges(sumptr* head, avl_ptr root, char* entry_date, char* exit_date, char* country)
{
    if(root != NULL) 
    { 
        AVL_Num_Patient_Discharges(head,root->left,entry_date,exit_date,country); 
        if(strcmp(root->rec->exitDate,"--") !=0)
        {
            if((date_to_seconds(entry_date) <= date_to_seconds(root->rec->exitDate)) && (date_to_seconds(root->rec->exitDate) <= date_to_seconds(exit_date)))
            {
                Insert_Statistics_List(head,root->rec->country,atoi(root->rec->age));
                Search_Same_Date_Num_Patient_Discharges(head,root,entry_date,exit_date,country);
            }
        }

        AVL_Num_Patient_Discharges(head,root->right,entry_date,exit_date,country);         
    }
}

//Insert into the list of summary statistics the records that have the specific disease in the specific country and in [date1,date2]
//So returns that list (of one node because the disease is the same)
void AVL_Search_Country_Outbreaks(sumptr* head,avl_ptr root, char* entry_date, char* exit_date, char* disease)
{
    if(root == NULL) return;
    if(date_to_seconds(root->date) >= date_to_seconds(entry_date))    
    {
        if(strcmp(exit_date,"--") != 0)
        {
            if(date_to_seconds(root->date) <= date_to_seconds(exit_date))
            {
                if(disease != NULL)  
                {
                    if(strcmp(root->rec->diseaseID,disease) == 0)
                        Insert_Statistics_List(head,disease,atoi(root->rec->age));
                }
                else  
                {
                    counter++;
                }
                Search_Same_Date_Country_Outbreaks(head,root,exit_date,disease);
            }
        }
        else
        {
            if(disease != NULL)  
            {
                if(strcmp(root->rec->diseaseID,disease) == 0)  counter++;
            }
            else  
            {
                counter++;
            }
            Search_Same_Date_Country_Outbreaks(head,root,exit_date,disease);            
        }
        AVL_Search_Country_Outbreaks(head,root->right,entry_date,exit_date,disease);
        AVL_Search_Country_Outbreaks(head,root->left,entry_date,exit_date,disease);  
    }
    if(date_to_seconds(root->date) < date_to_seconds(entry_date))  AVL_Search_Country_Outbreaks(head,root->right,entry_date,exit_date,disease);
}

void Num_of_Same_Date_Outbreaks(avl_ptr root, char* country)
{
    samedateptr node = root->next_same_date;
    if(root->next_same_date != NULL)
    {
        node = root->next_same_date;
        
        while(node != NULL)
        {
            if(strcmp(root->rec->country,country) == 0)  counter++;
            node = node->next;
        }
    }
}

//Returns the number of outbreaks for a given country.
void AVL_Num_of_Outbreaks_Disease(avl_ptr root, char* country)
{
    if(root != NULL) 
    { 
        AVL_Num_of_Outbreaks_Disease(root->left,country);
        if(strcmp(root->rec->country,country) == 0) counter++;
        Num_of_Same_Date_Outbreaks(root,country);
        AVL_Num_of_Outbreaks_Disease(root->right,country);         
    } 
}

void Num_of_Same_Date_Outbreaks2(avl_ptr root, char* disease)
{
    samedateptr node = root->next_same_date;
    if(root->next_same_date != NULL)
    {
        node = root->next_same_date;
        
        while(node != NULL)
        {
            if(strcmp(root->rec->diseaseID,disease) == 0)  counter++;
            node = node->next;
        }
    }
}

//Returns the number of outbreaks for a given disease.
void AVL_Num_of_Outbreaks_Countries(avl_ptr root, char* disease)
{
    if(root != NULL) 
    { 
        AVL_Num_of_Outbreaks_Countries(root->left,disease);
        if(strcmp(root->rec->diseaseID,disease) == 0) counter++;
        Num_of_Same_Date_Outbreaks2(root,disease);
        AVL_Num_of_Outbreaks_Countries(root->right,disease);         
    } 
}

//THis is a function that creates the statistics of each date file of each country
void Statistics_AVL(avl_ptr root, char* country, char** statistics)
{
    char case1[50],case2[50],case3[50],case4[50];

    if(root != NULL) 
    { 
        Statistics_AVL(root->left,country,statistics); 
        
        sumptr head = NULL;
        Insert_Statistics_List(&head,root->rec->diseaseID,atoi(root->rec->age)); //Insert or update to the statistics list the disease and the age 
        Same_Statistics_AVL(root,&head,country,statistics);  //Do the same work if exist same date nodes

        sumptr current = head;

        //If the array is empty allocate memory with malloc, else with realloc.
        if(*statistics==NULL)
        {
            //Save the date
            (*statistics) = (char*)malloc((strlen(root->rec->entryDate)+1)*sizeof(char));
            strcpy((*statistics),root->rec->entryDate);
            (*statistics) = (char*)realloc((*statistics), (strlen((*statistics))+1)+3);
            strcat((*statistics),"\n");
        }
        else
        {
            //Save the date
            (*statistics) = (char*)realloc((*statistics), (strlen((*statistics))+1 ) + (strlen(root->rec->entryDate)+1));
            strcat((*statistics),root->rec->entryDate); 

            (*statistics) = (char*)realloc((*statistics), (strlen((*statistics))+1)+3);
            strcat((*statistics),"\n");
        }

        //Save the country
        (*statistics) = (char*)realloc((*statistics), (strlen((*statistics))+1 ) + (strlen(country)+1));
        strcat((*statistics),country);

        (*statistics) = (char*)realloc((*statistics), (strlen((*statistics))+1)+3);
        strcat((*statistics),"\n");

        while(current != NULL)
        {
            //Save the disease name
            (*statistics) = (char*)realloc((*statistics), (strlen((*statistics))+1 ) + (strlen(current->disease)+1));
            strcat((*statistics),current->disease);

            (*statistics) = (char*)realloc((*statistics), (strlen((*statistics))+1)+3);
            strcat((*statistics),"\n");

            //Save the case 1 of ages
            (*statistics) = (char*)realloc((*statistics), (strlen((*statistics))+1 ) + (strlen("Age range 0-20 years: ")+1));
            strcat((*statistics),"Age range 0-20 years: ");  

            sprintf(case1, "%d", current->case_1);            
            (*statistics) = (char*)realloc((*statistics), (strlen((*statistics))+1 ) + (strlen(case1)+1));
            strcat((*statistics),case1);

            (*statistics) = (char*)realloc((*statistics), (strlen((*statistics))+1 ) + (strlen(" cases")+1));
            strcat((*statistics)," cases"); 

            (*statistics) = (char*)realloc((*statistics), (strlen((*statistics))+1)+3);
            strcat((*statistics),"\n");

            //Save the case 2 of ages
            (*statistics) = (char*)realloc((*statistics), (strlen((*statistics))+1 ) + (strlen("Age range 21-40 years: ")+1));
            strcat((*statistics),"Age range 21-40 years: ");  

            sprintf(case2, "%d", current->case_2);            
            (*statistics) = (char*)realloc((*statistics), (strlen((*statistics))+1 ) + (strlen(case2)+1));
            strcat((*statistics),case2);

            (*statistics) = (char*)realloc((*statistics), (strlen((*statistics))+1 ) + (strlen(" cases")+1));
            strcat((*statistics)," cases"); 

            (*statistics) = (char*)realloc((*statistics), (strlen((*statistics))+1)+3);
            strcat((*statistics),"\n");

            //Save the case 3 of ages
            (*statistics) = (char*)realloc((*statistics), (strlen((*statistics))+1 ) + (strlen("Age range 41-60 years: ")+1));
            strcat((*statistics),"Age range 41-60 years: ");  

            sprintf(case3, "%d", current->case_3);            
            (*statistics) = (char*)realloc((*statistics), (strlen((*statistics))+1 ) + (strlen(case3)+1));
            strcat((*statistics),case3);

            (*statistics) = (char*)realloc((*statistics), (strlen((*statistics))+1 ) + (strlen(" cases")+1));
            strcat((*statistics)," cases"); 

            (*statistics) = (char*)realloc((*statistics), (strlen((*statistics))+1)+3);
            strcat((*statistics),"\n");

            //Save the case 4 of ages
            (*statistics) = (char*)realloc((*statistics), (strlen((*statistics))+1 ) + (strlen("Age range 60+ years: ")+1));
            strcat((*statistics),"Age range 60+ years: ");  

            sprintf(case4, "%d", current->case_4);            
            (*statistics) = (char*)realloc((*statistics), (strlen((*statistics))+1 ) + (strlen(case4)+1));
            strcat((*statistics),case4);

            (*statistics) = (char*)realloc((*statistics), (strlen((*statistics))+1 ) + (strlen(" cases")+1));
            strcat((*statistics)," cases"); 

            (*statistics) = (char*)realloc((*statistics), (strlen((*statistics))+1)+3);
            strcat((*statistics),"\n");

            if(current->next != NULL)
            {
                (*statistics) = (char*)realloc((*statistics), (strlen((*statistics))+1)+3);
                strcat((*statistics),"\n");
            }

            current = current->next;
        }
        (*statistics) = (char*)realloc((*statistics), (strlen((*statistics))+1)+3);
        strcat((*statistics),"\n");
        // printf("%s",statistics);
        Delete_Statistics_List(&head);

        Statistics_AVL(root->right,country,statistics);         
    } 
}

//Do the same work for each same date node if exists.
void Same_Statistics_AVL(avl_ptr root, sumptr* head, char* country, char** statistics)
{
    samedateptr node = root->next_same_date;
            
    while(node != NULL)
    {
        Insert_Statistics_List(head,node->rec_node->diseaseID,atoi(node->rec_node->age));  //Insert or update to the statistics list the disease and the age
        node = node->next;
    }
}