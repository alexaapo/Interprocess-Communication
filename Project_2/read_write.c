#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>
#include "read_write.h"

void Write_Pipe(char* message,char* Pipe_Name, int bufferSize)
{
    int pipe_fd;

    if((pipe_fd = open(Pipe_Name, O_WRONLY))  < 0)  
            perror("Can't open write fifo");

    size_t size_array;
    int mod,div,j;
    
    size_array = (strlen(message)+1)/sizeof(message[0]);
    mod = size_array%bufferSize;  //The last bytes that are smaller than bufferSize 
    div = size_array/bufferSize;
    
    for(j=0;j<(div+1);j++)
    {
        //In case the message fits exactly, all chunks of message will have same size.
        if(mod==0 && j<div)
        {
            write(pipe_fd, &message[j*bufferSize] , bufferSize);
        }
        //In case the chunks of message won't have the same size.
        else if(mod!=0)
        {
            if(j==div)
            {
                write(pipe_fd,&message[j*bufferSize],mod);
            }
            else
            {
                write(pipe_fd,&message[j*bufferSize],bufferSize);
            }
        }
    }
    close(pipe_fd);
}

char *Read_Pipe(char** message, char* Name_Pipe, int bufferSize)
{
    int pipe_fd;
    char buf[bufferSize];
    strcpy(buf,"");
    char* signal_message;

    //Open the pipe P2C for reading.
    if((pipe_fd = open(Name_Pipe, O_RDONLY))  < 0) 
    { 
        signal_message = (char*)malloc((strlen("signal")+1)*sizeof(char));
        strcpy(signal_message,"signal");

        return (char*)(signal_message);
        // perror("Can't open read fifo");
    }
    
    //Read the message from parent/child in size of bufferSize.
    //In case the messsage is bigger bytes than bufferSize, 
    //then I "cut" the message into bufferSize bytes and 
    //put them all together with the use of realloc().
    int k=0,bytes;
    k=read(pipe_fd, buf, bufferSize);
    bytes=k;
    
    (*message) = (char *)malloc((k+1)*sizeof(char));
    strcpy((*message),"");  //Initialize the array with the final message.

    while(k!=0)
    {
        strncat((*message),buf,k);
        k=read(pipe_fd, buf, bufferSize);
        if(k!=0)
        { 
            bytes+=k;
            (*message) = (char *)realloc((*message), (bytes+k+1)*sizeof(char));
        }
    }
    strncat((*message),"\0",1);  //Put the null character in the end of message.
    
    close(pipe_fd);
    return (char*)(*message);
}

//This is a function that updates the structures of Worker with the new data 
int Worker_Update_Structures(bucket_ptr* disease_array, bucket_ptr* country_array, rec_ptr* head, int num_of_diseases, int num_of_directories, char*** directories, char* input_dir, char*** max_dates)
{
    int i,j,x,num_of_new_date_files=0,sum_of_all_records=0;
    // char s[100];
    // printf("%s\n", getcwd(s, 100));

    struct dirent *ent;
    DIR *dir = opendir(input_dir);
    if(!dir)
    {
        printf("ERROR: Please provide a valid directory path.\n");
        return 1;
    }
    
    chdir(input_dir);  //Change the directory cd input_dir

    char date[11];
    //This is for finding the number of new date files that 
    //have inserted in some subdirectories and the number
    //of all records that have inserted.
    for(i=0;i<num_of_directories;i++)
    {
        DIR *subdir = opendir((*(directories))[i]);
        if(!subdir)
        {
            printf("ERROR: Please provide a valid directory path.\n");
            return 1;
        }
        if(chdir((*(directories))[i]) != 0)
        {
            perror("Can't change directory.\n");
            return 1;
        }
        while((ent = readdir (subdir)) != NULL)
        {
            if(strcmp(ent->d_name,".") != 0 && strcmp(ent->d_name,"..") != 0)
            {    
                // if(i==0)  num_of_date_files++;
                strcpy(date,"");
                memcpy(date,ent->d_name,10);
                date[10] = '\0';

                if(date_to_seconds(date) > date_to_seconds((*(max_dates))[i]))
                {
                    num_of_new_date_files++;
                
                    FILE* fp;
                    char buf[1024];
                    if ((fp = fopen(ent->d_name, "r")) == NULL)
                    { /* Open source file. */
                        perror("Can't open the file.");
                        return 1;
                    }
                    while (fgets(buf, sizeof(buf), fp) != NULL)
                    {
                        sum_of_all_records++;
                    }
                    fclose(fp);
                }
            }
        }
        closedir(subdir);

        if(chdir("..") != 0)
        {
            perror("Can't change directory.\n");
            return 1;
        }
    }

    // printf("new date files: %d\n",num_of_new_date_files);
    // printf("new records: %d\n",sum_of_all_records);

    //I create an array of new the dates of directories and an array of countries that have inserted to them new date files.
    char ** new_dates_array = (char**)malloc(num_of_new_date_files*sizeof(char**));
    for(i=0;i<num_of_new_date_files;i++)
    {
        new_dates_array[i] = (char*)malloc(11*sizeof(char));
    }

    char ** countries_of_new_dates_array = (char**)malloc(num_of_new_date_files*sizeof(char**));
    
    int dates=0;

    //Search and save the new date files and at the same time the countries.
    for(i=0;i<num_of_directories;i++)
    {
        DIR *subdir = opendir((*(directories))[i]);
        if(!subdir)
        {
            printf("ERROR: Please provide a valid directory path.\n");
            return 1;
        }

        if(chdir((*(directories))[i]) != 0)
        {
            perror("Can't change directory.\n");
            return 1;
        }
        while((ent = readdir (subdir)) != NULL)
        {
            //Take the names of dates and copy them to the array of dates.
            if(strcmp(ent->d_name,".") != 0 && strcmp(ent->d_name,"..") != 0)
            {  
                strcpy(date,"");
                memcpy(date,ent->d_name,10);
                date[10] = '\0';

                if(date_to_seconds(date) > date_to_seconds((*(max_dates))[i]))
                {  
                    strcpy(new_dates_array[dates],date);

                    countries_of_new_dates_array[dates] = (char*)malloc((strlen((*(directories))[i])+1)*sizeof(char));
                    strcpy(countries_of_new_dates_array[dates],(*(directories))[i]);
                    dates++;
                }
            }
        }

        closedir(subdir);
        if(chdir("..") != 0)
        {
            perror("Can't change directory.\n");
            return 1;
        }
    }

    //Now find the new max dates for helping me for the next SIGUSR1
    for(j=0;j<num_of_new_date_files;j++)
    {
        int v;
        for(v=0;v<num_of_directories;v++)
        {
            if(strcmp(countries_of_new_dates_array[j],(*(directories))[v]) == 0)
            {
                break;
            }
        }

        if(date_to_seconds(new_dates_array[j]) > date_to_seconds((*(max_dates))[v]))
        {
            strcpy((*(max_dates))[v],new_dates_array[j]);
        }   
    }

    //Just put the .txt in date's array
    for(j=0;j<num_of_new_date_files;j++)
    {
        new_dates_array[j] = (char *) realloc(new_dates_array[j], 16);
        strcat(new_dates_array[j], ".txt");
        //printf("%s %s\n", new_dates_array[j],countries_of_new_dates_array[j]);
    }

///////////////////////////////////////////

    //Now just like the beggining of the worker's code insert the records to the structures 
    char *** records = (char***)malloc((sum_of_all_records)*sizeof(char**));
    for(x=0;x<(sum_of_all_records);x++)
    {
        records[x] = (char**)malloc(8*sizeof(char*));
    }
    
    int z=0;
    for(i=0;i<num_of_new_date_files;i++)
    {
        DIR *subdir = opendir(countries_of_new_dates_array[i]);
        if(!subdir)
        {
            printf("ERROR: Please provide a valid directory path.\n");
            return 1;
        }
        if(chdir(countries_of_new_dates_array[i]) != 0)
        {
            perror("Can't change directory.\n");
            return 1;
        }
        
        FILE* fp;
        char buf[1024];
        if ((fp = fopen(new_dates_array[i], "r")) == NULL)
        { 
            perror("Can't open the file.");
            return 1;
        }
        //For each record
        while (fgets(buf, sizeof(buf), fp) != NULL)
        {
            buf[strlen(buf) - 1] = '\0'; //eat the newline fgets() stores

            char word[1024];
            int length,p,temp=0;
            int num_of_words = 0;
            
            //Take the number of words of each record, because if a record has less words than 6, 
            //its surely invalid, so do not save it into the array.
            for(p=0; 1==sscanf(buf + p, "%s%n", word, &length); p = p + length)
            {   
                num_of_words++;
            }

            if(num_of_words == 6)
            {
                for(p=0; 1==sscanf(buf + p, "%s%n", word, &length); p = p + length)
                {
                    if(temp == 1)  //Word 1 is the "Enter" or "Exit"
                    {
                        char dt[11]="";
                        memcpy(dt,new_dates_array[i],10);

                        //Save the appropriate date to Enter or Exit index of the array.
                        if(strcmp(word,"ENTER") == 0)
                        {
                            records[z][6] = (char*)malloc((strlen(dt)+1)*sizeof(char));
                            strcpy(records[z][6],dt);

                            records[z][7] = (char*)malloc(3*sizeof(char));
                            strcpy(records[z][7],"--");
                        }
                        else if(strcmp(word,"EXIT") == 0)
                        {
                            records[z][7] = (char*)malloc((strlen(dt)+1)*sizeof(char));
                            strcpy(records[z][7],dt);

                            records[z][6] = (char*)malloc(3*sizeof(char));
                            strcpy(records[z][6],"--");
                        }
                        else //If the 2nd word is not "Enter" neither "exit" then this record is invalid.
                        {
                            strcpy(records[z][0],"-");

                            records[z][6] = (char*)malloc(2*sizeof(char));
                            strcpy(records[z][6],"-");

                            records[z][7] = (char*)malloc(2*sizeof(char));
                            strcpy(records[z][7],"-");

                            printf("ERROR\n");
                        }
                        temp++;
                        continue;
                    }
                    else if(temp == 5)  //Word 5 is the age but first I have to store the country
                    {
                        records[z][temp-1] = (char*)malloc((strlen(countries_of_new_dates_array[i])+1)*sizeof(char));
                        strcpy(records[z][temp-1],countries_of_new_dates_array[i]);

                        records[z][temp] = (char*)malloc((strlen(word)+1)*sizeof(char));
                        strcpy(records[z][temp],word);
                    }
                    else if(temp>1) //For name, surname, disease
                    {
                        records[z][temp-1] = (char*)malloc((strlen(word)+1)*sizeof(char));
                        strcpy(records[z][temp-1],word);
                    }
                    else  //For record id
                    {
                        records[z][temp] = (char*)malloc((strlen(word)+1)*sizeof(char));
                        strcpy(records[z][temp],word);
                    }
                    
                    temp++;
                }

            }
            else  printf("ERROR\n");
            num_of_words=0;
            z++;
        }
        fclose(fp);
        
        closedir(subdir);

        //Go to input_dir
        if(chdir("..") != 0)
        {
            perror("Can't change directory.\n");
            return 1;
        }
    }

    int l;
    //Now Worker is ready to edit half records who holds them in array of records so as to create a complete record
    for(x=0;x<(sum_of_all_records);x++)
    {
        //In case of Enter record
        if(strcmp(records[x][6],"--") != 0)
        {
            //Find into the array the EXIT record with same record ID.(if exists)
            for(l=0;l<(sum_of_all_records);l++)
            {
                //If ENTER record and EXIT record have the same record ID update exit_date so as to have a complete record.
                if(strcmp(records[x][0],records[l][0]) == 0 && strcmp(records[l][7],"--") != 0 && strcmp(records[l][0],"-") !=0)
                {
                    free(records[x][7]);
                    records[x][7] = (char*)malloc((strlen(records[l][7])+1)*sizeof(char));
                    strcpy(records[x][7],records[l][7]);
                    
                    strcpy(records[l][0],"-");
                    break;
                }
            }
        }
    }


    //Now that I have the complete records I start to insert them into the structurs.
    int bucket_size=20;

    for(x=0;x<(sum_of_all_records);x++)
    {
        if(strcmp(records[x][0],"-")!=0)
        {
            //Check if the Record is valid
            int error = Check_Validity_of_Record(head,records[x][0],records[x][1],records[x][2],records[x][3],records[x][4],records[x][5],records[x][6],records[x][7]);
            if(error == 1)
            {
                printf("Record ID is already exists!\n");
                return 1;
            }
            if(error!=0)
                Print_Error(error); 
            else //If the records are valid put them in the records list.
            {
                rec_ptr record_node;
                int hash, hash1;   

                record_node = Insert_Record(head,records[x][0],records[x][1],records[x][2],records[x][3],records[x][4],records[x][5],records[x][6],records[x][7]);

                hash = Hash_Function(records[x][3], num_of_diseases);
                hash1 = Hash_Function(records[x][4], num_of_directories);

                //For the Hash table of Diseases
                //Case 1: There is not any created bucket yet.
                if(disease_array[hash] == NULL)
                {
                    bucket_ptr new_bucket;

                    new_bucket = Create_Bucket(bucket_size);
                    
                    if(new_bucket != NULL) disease_array[hash] = new_bucket;
                    else printf("Problem with memory!\n");

                    Insert_Bucket_Entry(new_bucket,record_node,records[x][3],records[x][6],bucket_size);
                }
                else //Case 2: If there at least one bucket.
                {
                    bucket_ptr current = disease_array[hash];
                    //Insert the disease to the apropriate bucket.
                    Insert_Bucket_Entry(current,record_node,records[x][3],records[x][6],bucket_size);
                }

                //For the Hash table of Countries
                //Case 1: There is not any created bucket yet.
                if(country_array[hash1] == NULL)
                {
                    bucket_ptr new_bucket;

                    new_bucket = Create_Bucket(bucket_size);
                    
                    if(new_bucket != NULL) country_array[hash1] = new_bucket;
                    else printf("Problem with memory!\n");

                    Insert_Bucket_Entry(new_bucket,record_node,records[x][4],records[x][6],bucket_size);
                }
                else //Case 2: If there at least one bucket.
                {
                    bucket_ptr current = country_array[hash1];
                    //Insert the disease to the apropriate bucket.
                    Insert_Bucket_Entry(current,record_node,records[x][4],records[x][6],bucket_size);
                }                       
            }
        }
    }
    if(chdir("..") != 0)
    {
        perror("Can't change directory.\n");
        return 1;
    }

    //Deallocate the memory of records array
    for(x=0;x<(sum_of_all_records);x++)
    {
        for(l=0;l<8;l++)
            free(records[x][l]);
        free(records[x]);
    }
    free(records);

    for(i=0;i<num_of_new_date_files;i++)
    {
        free(new_dates_array[i]);
    }
    free(new_dates_array);

    for(i=0;i<num_of_new_date_files;i++)
    {
        free(countries_of_new_dates_array[i]);
    }
    free(countries_of_new_dates_array);

    return 0;
}
