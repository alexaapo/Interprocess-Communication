#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"
#include "enum.h"
#include "record.h"
#include <stdbool.h>
#define MAX_VALID_YR 2100 
#define MIN_VALID_YR 1900
#define MAX_AGE 120
#define MIN_AGE 1

int error;

bool isLeap(int year) 
{ 
    return (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0)); 
} 
   
int isValidDate(int dd, int mm, int yyyy) 
{ 
    //If year, month and day are not in given range 
    if(yyyy > MAX_VALID_YR || yyyy < MIN_VALID_YR) 
        return Invalid_Entry_Year; 
    if(mm < 1 || mm > 12) 
        return Invalid_Entry_Month; 
    if(dd < 1 || dd > 30) 
        return Invalid_Entry_Day; 

    //Handle February month with leap year 
    // if (mm == 2) 
    // { 
    //     if(isLeap(yyyy))
    //     {
    //         if(dd > 29) 
    //             return Invalid_Leap_day; 
    //     }
    //     else
    //     {
    //         if(dd > 28)
    //             return Invalid_Leap_day;
    //     }
    // } 

    //Months of April, June, Sept and Nov must have number of days less than or equal to 30. 
    // if (mm == 4 || mm == 6 || mm == 9 || mm == 11) 
    //     if(dd > 30)
    //         return Invalid_Entry_Day; 
    return 0; 
}


//Checks if the name has blanks or another non valid character.
int Invalid_Name(char* firstname)
{
    char* c;
    for(c=firstname; *c != '\0'; c++)
    {
        if((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z')) 
            continue;
        else if(*c == ' ')
        {  
            return Fisrt_Name_has_blanks;
        }
        else
        {
            return Invalid_First_Name;
        }
    }
    return 0;
}

//Checks if the disease has blanks or another non valid character.
int Invalid_Disease(char* disid)
{
    char* c;
    for(c=disid; *c != '\0'; c++)
    {
        if((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z') || (*c >= '0' && *c <= '9') || (*c == '-')) 
            continue;
        else if(*c == ' ')  
        {
            return Disease_ID_has_blanks;
        }
        else
        {
            return Invalid_Disease_ID;
        }
    }
}

int Invalid_Record(char* record)
{
    char* c;
    for(c=record; *c != '\0'; c++)
    {
        if((*c >= '0' && *c <= '9') || (*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z')) 
            continue;
        else 
            return Invalid_Record_ID;
    }
}

int is_number(char* record)
{
    char* c;
    for(c=record; *c != '\0'; c++)
    {
        if((*c >= '0' && *c <= '9')) 
            continue;
        else 
            return Invalid_Record_ID;
    }
}

int Invalid_Date(char* date)
{
    if(strlen(date) != 10)  return -1;

    //Take the entry day, month and year and check if it is valid.
    int count = 0;
    char* valid_date;
    char* day = (char*)malloc(3*sizeof(char));
    char* month = (char*)malloc(3*sizeof(char));
    char* year = (char*)malloc(5*sizeof(char));

    for(valid_date=date; *valid_date!='\0';valid_date++)
    {
        if(count<=1)
        {
            if((*valid_date >= '0' && *valid_date <= '9'))
                if(count == 0) day[0] = *valid_date;
                else day[1] = *valid_date;
            else
            {
                free(day);
                free(month);
                free(year);
                return Invalid_Entry_Day;
            } 
        } 
        else if(count<=4)
        {
            if(count == 2 && *valid_date != '-')
            {
                free(day);
                free(month);
                free(year);
                return Invalid_Entry_Date;
            }
            else if(count == 2 && *valid_date == '-')
            {   
                count++;
                continue;
            }
            
            if((*valid_date >= '0' && *valid_date <= '9'))
                if(count == 3) month[0] = *valid_date;
                else month[1] = *valid_date;
            else
            {
                free(day);
                free(month);
                free(year);
                return Invalid_Entry_Month;
            }  
        } 
        else if(count <= 9)
        {
            if(count == 5 && *valid_date != '-')
            {
                free(day);
                free(month);
                free(year);
                return Invalid_Entry_Date;
            }
            else if(count == 5 && *valid_date == '-')
            {   
                count++;
                continue;
            }
            if((*valid_date >= '0' && *valid_date <= '9'))
                if(count == 6) year[0] = *valid_date;
                else if(count == 7) year[1] = *valid_date;
                else if(count == 8) year[2] = *valid_date;
                else year[3] = *valid_date;
            else
            {
                free(day);
                free(month);
                free(year);
                return Invalid_Entry_Year;
            }  
        }
        else  
        {
            free(day);
            free(month);
            free(year);
            return Invalid_Entry_Year;
        }
        count++;
    }
    
    day[2] ='\0';
    month[2] ='\0';
    year[4] ='\0';


    int dd = atoi(day);
    int mm = atoi(month);
    int yyyy = atoi(year);

    free(day);
    free(month);
    free(year);

    int er = isValidDate(dd,mm, yyyy);
    
    if(er != 0)  return er; 
    
    return 0;
}

int Invalid_Age(char* age)
{
    char* c;
    for(c=age; *c != '\0'; c++)
    {
        if((*c >= '0' && *c <= '9')) 
            continue;
        else 
            return Invalid_Record_Age;
    }

    int ag=atoi(age);
    if((ag >= MIN_AGE) && (ag <= MAX_AGE))  return 0;
    else return Invalid_Record_Age;
}

//Checks for errors in records and insert them to the list of records.
int Check_Validity_of_Record(rec_ptr* head, char* recid, char* firstname, char* lastname, char* disid, char* coun, char* age, char* endate, char* exdate)
{
    rec_ptr current = *head;
    int er;

    if(strcmp(endate,"--")==0)
    {
        return Invalid_Entry_Date;
    }

    er = Invalid_Record(recid);
    if(er != 0)  return er;

    if(current != NULL)
    {
        if(strcmp(current->recordID,recid) == 0)  return Record_ID_already_exists;
        //Check if the record id already exists.
        while(current!=NULL)
        {
            if(strcmp(current->recordID,recid) == 0)
            {
                return Record_ID_already_exists;
            }
            current = current -> next;
        }
    }

    er = Invalid_Name(firstname);
    if(er!=0)
    {
        if(er%2!=0)
            return Fisrt_Name_has_blanks;
        else
            return Invalid_First_Name;
    }

    er = Invalid_Name(lastname);

    if(er!=0)
    {
        if(er%2!=0)
            return Last_Name_has_blanks;
        else
            return Invalid_Last_Name;
    }

    er = Invalid_Disease(disid);

    if(er!=0)
    {
        if(er%2!=0)
            return Disease_ID_has_blanks;
        else
            return Invalid_Disease_ID;
    }

    er = Invalid_Name(coun);

    if(er!=0)
    {
        if(er%2!=0)
            return Country_Name_has_blanks;
        else
            return Invalid_Country_Name;
    }

    er = Invalid_Age(age);
    if(er!=0)  return Invalid_Record_Age;

    er = Invalid_Date(endate);
    if(er != 0)  return er;

    if(strcmp(exdate,"--") != 0)
    {
        er = Invalid_Date(exdate);
        if(er != 0)  
        {
            if(er == Invalid_Entry_Day)  return Invalid_Exit_Day;
            if(er == Invalid_Entry_Month)  return Invalid_Exit_Month;
            if(er == Invalid_Entry_Year)  return Invalid_Exit_Year;
            return Invalid_Exit_Date;
        } 
    }

    //Check if the entry date is after the exit date.
    if(strcmp(exdate,"--")!=0)
    {
        if(date_to_seconds(endate)>date_to_seconds(exdate))
        {
            return Entry_Date_after_Exit_Date;
        }
    }
    
    return 0;
}

rec_ptr Insert_Record(rec_ptr* head, char* recid, char* firstname, char* lastname, char* disid, char* coun, char* age, char* endate, char* exdate)
{
    //If the all the record is valid, then allocate the appropriate memory and put it in the list.
    rec_ptr new_node = (rec_ptr)malloc(sizeof(struct Record_Node));
    
    new_node->recordID = (char*)malloc((strlen(recid)+1)*sizeof(char));
    strcpy(new_node->recordID,recid);

    new_node->patientFirstName = (char*)malloc((strlen(firstname)+1)*sizeof(char));
    strcpy(new_node->patientFirstName,firstname);

    new_node->patientLastName = (char*)malloc((strlen(lastname)+1)*sizeof(char));
    strcpy(new_node->patientLastName,lastname);

    new_node->diseaseID = (char*)malloc((strlen(disid)+1)*sizeof(char));
    strcpy(new_node->diseaseID,disid);

    new_node->country = (char*)malloc((strlen(coun)+1)*sizeof(char));
    strcpy(new_node->country,coun);

    new_node->age = (char*)malloc((strlen(age)+1)*sizeof(char));
    strcpy(new_node->age,age);

    new_node->entryDate = (char*)malloc((strlen(endate)+1)*sizeof(char));
    strcpy(new_node->entryDate,endate);

    new_node->exitDate = (char*)malloc((strlen(exdate)+1)*sizeof(char));
    strcpy(new_node->exitDate,exdate);
    
    new_node->next = NULL;

    //Case: the list is empty.
    if(*head == NULL)
    {
        *head = new_node;
        return new_node;
    }

    rec_ptr current = *head;

    while(current->next != NULL) 
        current = current->next; 
   
    current->next = new_node; 
    return new_node;
}

//For /recordPatientExit.
//Find the record id in the record list,
//Check the validity of exit date and put it in the right record.
int Search_ID(rec_ptr head, char* rec_id, char* exit_date)
{
    int count=0,error;

    error = Invalid_Record(rec_id);
    if(error != 0)  return error;

    if(strcmp(exit_date,"-")!=0 & strcmp(exit_date,"")!=0)
    {    
        error = Invalid_Date(exit_date);
        
        if(error != 0)  
        {
            if(error == Invalid_Entry_Day)  return Invalid_Exit_Day;
            if(error == Invalid_Entry_Month)  return Invalid_Exit_Month;
            if(error == Invalid_Entry_Year)  return Invalid_Exit_Year;
            return Invalid_Exit_Date;
        } 
    
        while(head != NULL)
        {
            if(strcmp(head->recordID,rec_id) == 0)
            {
                if(date_to_seconds(head->entryDate)>date_to_seconds(exit_date))
                    return Entry_Date_after_Exit_Date;

                free(head->exitDate);

                head->exitDate = (char*)malloc((strlen(exit_date)+1)*sizeof(char));
                strcpy(head->exitDate,exit_date);
                return 0;  
            }
            head = head->next;
        }
        return There_is_no_Record_with_this_Record_ID;
    }
    return Invalid_Exit_Date;
}

void Print_Error(int error)
{
    printf("ERROR\n");
    // switch(error)
    // {
    //     case 1:
    //         printf("This Record ID is already exists!\n");
    //         break;
    //     case 2:
    //         printf("Entry Date is after Exit Date!\n");
    //         break;
    //     case 3:
    //         printf("First Name has blanks\n");
    //         break;
    //     case 4:
    //         printf("Invalid input of First Name\n");
    //         break;
    //     case 5:
    //         printf("Last Name has blanks\n");
    //         break;
    //     case 6:
    //         printf("Invalid input of Last Name\n");
    //         break;
    //     case 7:
    //         printf("Disease ID has blanks\n");
    //         break;
    //     case 8:
    //         printf("Invalid input of Disease ID\n");
    //         break;
    //     case 9:
    //         printf("Country Name has blanks\n");
    //         break;
    //     case 10:
    //         printf("Invalid input of Country Name\n");
    //         break;
    //     case 11:
    //         printf("Invalid entry day!\n");
    //         break;
    //     case 12:
    //         printf("Invalid entry month!\n");
    //         break;
    //     case 13:
    //         printf("Invalid entry year!\n");
    //         break;
    //     case 14:
    //         printf("Invalid entry date!\n");
    //         break;
    //     case 15:
    //         printf("Invalid exit day!\n");
    //         break;
    //     case 16:
    //         printf("Invalid exit month!\n");
    //         break;
    //     case 17:
    //         printf("Invalid exit year!\n");
    //         break;
    //     case 18:
    //         printf("Invalid exit date!\n");
    //         break;
    //     case 19:
    //         printf("Problem with Leap year!\n");
    //         break;
    //     case 20:
    //         printf("Invalid Input!\n");
    //         break;
    //     case 21:
    //         printf("here is no Record with this Record ID!\n");
    //         break;
    //     case 22:
    //         printf("Invalid Input of Record!\n");
    //         break;
    //     case 23:
    //         printf("Invalid Age of Record!\n");
    //         break;
    //     default:
    //         break;
    // }
}

//Deallocate THe record list.
void Delete_Record_List(rec_ptr* head)
{
    rec_ptr current = *head;  
    rec_ptr next;  
    
    while(current != NULL)  
    {  
        next = current->next;
        free(current->recordID);  
        free(current->patientFirstName);  
        free(current->patientLastName);  
        free(current->diseaseID);  
        free(current->country);  
        free(current->age);  
        free(current->entryDate);  
        free(current->exitDate);  
        free(current);  
        current = next;  
    }  
        
    *head = NULL;  
}

void Print_Record(rec_ptr head)
{
    while(head != NULL)
    {
        printf("%s %s %s %s %s %s %s\n", head->recordID, head->patientFirstName, head->patientLastName, head->diseaseID, head->country, head->entryDate, head->exitDate);
        head = head->next;
    }
}

int Size_of_List(rec_ptr head)
{
    int count=0;
    while(head != NULL)
    {
        count++;
        head = head->next;
    }
    return count;
}