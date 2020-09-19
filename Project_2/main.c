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
#include "worker_info.h"
#include "read_write.h"
#include "handler.h"

#define PERMS 0666

volatile sig_atomic_t flag=0;
volatile sig_atomic_t worker_pid=-10;

int main(int argc, char** argv)
{
    //Check for invalid input.
    if( argc < 7 || argc >7 )
    {
        printf("Invalid Input. Please give 7 arguments.\n");
        return(1);
    }
    
    int  numWorkers, bufferSize, i;
    char* input_dir;

    //In case we give the arguments with random sequence.
    for(i=1;i<=5;i+=2)
    {
        if(strcmp(argv[i],"-i") == 0)
        {
            input_dir = (char*)malloc((strlen(argv[i+1])+1)*sizeof(char));
            strcpy(input_dir,argv[i+1]);
        }
        else if(strcmp(argv[i],"-w") == 0)
            numWorkers = atoi(argv[i+1]);
        else if(strcmp(argv[i],"-b") == 0)
            bufferSize = atoi(argv[i+1]);
        else
        {
            printf("Please give an input with this form: ./diseaseAggregator –w numWorkers -b bufferSize -i input_dir\n");
            exit(1);
        }
    }

    //Check for validation of bufferSize and num of Workers.
    if((bufferSize==0) || (numWorkers==0))
    {
        printf("Please give at least 1 in buffersize/numWorkers\n");
        free(input_dir);
        exit(1);
    }
    
    //Open the directory to take the county names and save them in an array.
    struct dirent *ent;
    DIR *dir = opendir(input_dir);
    if(!dir)
    {
        printf("ERROR: Please provide a valid directory path.\n");
        free(input_dir);
        exit(1);
    }

    //Take the number of Countries.
    int num_of_countries=0, k=0;
    while ((ent = readdir (dir)) != NULL)
    {
        if(strcmp(ent->d_name,".") != 0 && strcmp(ent->d_name,"..") != 0)
            num_of_countries++;
    }
    closedir (dir);

    //Save the country names in an array.
    dir = opendir(input_dir);
    char** countries_array = (char**)malloc(num_of_countries*sizeof(char*));
    // printf("%d\n",num_of_countries);
    while ((ent = readdir (dir)) != NULL)
    {
        if((strcmp(ent->d_name,".") != 0) && (strcmp(ent->d_name,"..") != 0))
        {
            countries_array[k] = (char*)malloc((strlen(ent->d_name)+1)*sizeof(char));
            strcpy(countries_array[k],ent->d_name);
            k++;
        }
    }
    closedir (dir);

    // for(i=0;i<num_of_countries;i++)  printf("%s\n",countries_array[i]);

    //Now I "split" the directories to Workers.
    int times=0, directories_per_child, left_over_workers=0,left_over_directories=0;
    k=0;

    //In case countries are more than Workers,
    //then there are leftover directories.
    if(num_of_countries > numWorkers)
    {
        directories_per_child = num_of_countries/numWorkers;
        left_over_directories = num_of_countries%numWorkers;
        times=numWorkers;
    }
    //In case countries are less than Workers,
    //then each Worker will take 1 directory.
    else 
    {
        directories_per_child = 1;
        times=num_of_countries; //This is for inactive Workers.
    }

    char pipe_P2C[100], pipe_C2P[100];
    pid_t pid;

    worker_info worker_info_array[times];
    // printf("times %d numworkers %d\n",times,numWorkers);

    for( i=0 ; i<numWorkers ; i++)
    {
        //In case of active Workers.
        if(i < times)
        {
            //Create the FIFOS.
            sprintf(pipe_P2C,"Pipe_P2C_%d", i+1);
            sprintf(pipe_C2P,"Pipe_C2P_%d", i+1);

            if (mkfifo(pipe_P2C, PERMS) < 0) 
                perror("Fail to create a named Pipe.\n");
            if (mkfifo(pipe_C2P, PERMS) < 0) 
            {
                // unlink(pipe_P2C);
                perror("Fail to create a named Pipe.\n");
            }
            //Save the names of pipes
            strcpy(worker_info_array[i].pipe_P2C,pipe_P2C);
            strcpy(worker_info_array[i].pipe_C2P,pipe_C2P);
            worker_info_array[i].num_of_directories=0;
            
            //In case we have left over directories, there are some Workers that will take more directories than others.
            if(left_over_directories > 0 )
            {
                int j;
                // printf("geia\n");
                worker_info_array[i].worker_directories = (char**)malloc((directories_per_child+1)*sizeof(char*));
                worker_info_array[i].num_of_directories = directories_per_child+1;
                for(j=0 ; j<directories_per_child+1; j++)
                {
                    worker_info_array[i].worker_directories[j] = (char*)malloc((strlen(countries_array[k])+1)*sizeof(char));
                    strcpy(worker_info_array[i].worker_directories[j],countries_array[k]);
                    k++;
                }
                left_over_directories--;
            }
            else if(left_over_directories==0)
            {
                int j;
                worker_info_array[i].worker_directories = (char**)malloc((directories_per_child)*sizeof(char*));
                worker_info_array[i].num_of_directories = directories_per_child;
                for(j=0 ; j<(directories_per_child); j++)
                {
                    worker_info_array[i].worker_directories[j] = (char*)malloc((strlen(countries_array[k])+1)*sizeof(char));
                    strcpy(worker_info_array[i].worker_directories[j],countries_array[k]);
                    k++;
                }
            }
        }
        
        //Create Workers
        if((pid = fork()) == -1)
        {
            printf("Faild to fork\n");
            exit(1);
        }
        else if(pid == 0)
        {
            if(i<times)  //Exec the program for each active Worker
            {
                char buffersize[10];
                sprintf(buffersize,"%d",bufferSize);
                // printf("%s\n",buffersize);
                char* args[] = {"./worker", worker_info_array[i].pipe_P2C, buffersize, NULL};
                execv("./worker", args);
            }
            else  //In case of inactive workers
            {
                char inactive[5];
                sprintf(inactive,"%d",-1);
                char* args1[] = {"./worker", inactive, NULL};
                execv("./worker", args1); 
            }
        }
        
        if(i<times)  worker_info_array[i].pid = pid;  //SAve the pid of each Worker in the struct
               
    }
    
    // printf("Parent id: %d\n", getpid());
    
    int j;

    //Open the pipe P2C for sending to the Worker the informations about his directories
    for(i=0;i<times;i++)
    {

        //Save the information that want to send in a Worker in an array.
        //Communication protocol:
        //1st line: name of input dir.
        //2nd line: number of directories.
        //3rd line: name of P2C pipe.
        //4rd line: directories of Worker.
        char msg[20*num_of_countries];
        char num_of_dir[20];
        sprintf(num_of_dir,"%d",worker_info_array[i].num_of_directories);

        strcpy(msg,"");

        strncat(msg,input_dir,strlen(input_dir));
        strncat(msg,"\n",1);

        strncat(msg,num_of_dir,strlen(num_of_dir));
        strncat(msg,"\n",1);

        // strncat(msg,worker_info_array[i].pipe_P2C,strlen(worker_info_array[i].pipe_P2C));
        // strncat(msg,"\n",1);
        
        strncat(msg,worker_info_array[i].pipe_C2P,strlen(worker_info_array[i].pipe_C2P));
        strncat(msg,"\n",1);

        //Save the name of directories of each Worker.
        for(j=0;j<worker_info_array[i].num_of_directories;j++)
        {
            strncat(msg,worker_info_array[i].worker_directories[j],strlen(worker_info_array[i].worker_directories[j]));
            if((j<worker_info_array[i].num_of_directories-1))  strncat(msg," ",1);
            else  strncat(msg,"\0",1); 
        }
        
        //Start writing in the pipe P2C with size bufferSize
        Write_Pipe(msg,worker_info_array[i].pipe_P2C,bufferSize);
    }

    char* whole_statistics=NULL;

    for(i=0;i<times;i++)
    {
        char* statistics;
        
        statistics = Read_Pipe(&statistics,worker_info_array[i].pipe_C2P,bufferSize);
        
        // printf("%s",statistics);
        if(whole_statistics==NULL)
        {
            (whole_statistics) = (char*)malloc((strlen(statistics)+1)*sizeof(char));
            strcpy((whole_statistics),statistics);
        }
        else
        {
            (whole_statistics) = (char*)realloc((whole_statistics), (strlen((whole_statistics))+1 ) + (strlen(statistics)+1));
            strcat((whole_statistics),statistics); 
        }

        free(statistics);       
    }
    printf("%s",whole_statistics);
    free(whole_statistics);

    int failure=0, success=0, total=0;

    //For signal SIGINT
    struct sigaction act;
 
	memset(&act, 0, sizeof(act));
	act.sa_sigaction = Handler;
	act.sa_flags = SA_SIGINFO;
 
	if (sigaction(SIGINT, &act, NULL) < 0) {
		perror ("Sigaction\n");
		return 1;
	}

    //For signal SIGQUIT
    struct sigaction act1;
 
	memset(&act1, 0, sizeof(act1));
	act1.sa_sigaction = Handler;
	act1.sa_flags = SA_SIGINFO;
 
	if (sigaction(SIGQUIT, &act1, NULL) < 0) {
		perror ("Sigaction\n");
		return 1;
	}

    //For signal SIGCHLD (useful for parent to know when a Worker has ended)
    struct sigaction act2;
 
	memset(&act2, 0, sizeof(act2));
	act2.sa_sigaction = Handler_New_Worker;
	act2.sa_flags = SA_SIGINFO | SA_RESTART;
 
	if (sigaction(SIGCHLD, &act2, NULL) < 0) {
		perror ("Sigaction\n");
		return 1;
	}

    printf("Parent PID: %d\n",getpid());

    //Begin the application of diseaseMonitor
    while(1)
    {
        printf("Please give an input:\n");
        
        char line[200];
        char *result;
        
        if ((result = fgets(line,200,stdin)) != NULL) {total++;}
        else if (ferror(stdin))
        {
            //In case the Parent accept a SIGINT/SIGQUIT signal
            //Then he has to kill all the Workers and make a
            //log_file
            if(flag == 1)
            {
                for(i=0;i<times;i++)
                {
                    kill(worker_info_array[i].pid,SIGKILL);
                }

                char* file_name;
                char pid[10];

                file_name = (char*)malloc((strlen("log_file.")+1)*sizeof(char));
                strcpy(file_name,"log_file.");

                sprintf(pid,"%d", getpid());

                file_name = (char*)realloc(file_name, (strlen(file_name)+1 ) + (strlen(pid)+1));
                strcat(file_name,pid); 

                FILE *fp;

                if((fp = fopen(file_name, "w")) == NULL)
                { 
                    perror("Can't create the file.");
                    break;
                }

                for(i=0;i<num_of_countries;i++)
                {
                    fputs(countries_array[i], fp);
                    fputs("\n", fp);
                }

                char totals[50], successes[50], failures[50];
                
                sprintf(totals,"%d",total);
                sprintf(successes,"%d",success);
                sprintf(failures,"%d",failure);

                fputs("TOTAL ", fp);
                fputs(totals,fp);
                fputs("\n", fp);

                fputs("SUCCESS ", fp);
                fputs(successes,fp);
                fputs("\n", fp);

                fputs("FAIL ", fp);
                fputs(failures,fp);
                fputs("\n", fp); 

                fclose(fp); 

                free(file_name);    

                //Restart the flag for SIGINT/SIGQUIT
                flag=0;          

                break;
            }
            else  perror("Error");
        }
       
        int len=strlen(result); //where buff is your char array fgets is using
        if(result[len-1]=='\n')
            result[len-1]='\0';

        // printf("%zu\n",strlen(result));


        int j=0,w=0; 
        char user_input[10][100]; 

        for(i=0;i<=(strlen(result));i++)
        {
            // if space or NULL found, assign NULL into user_input[w]
            if(result[i]==' '|| result[i]=='\0')
            {
                user_input[w][j]='\0';
                w++;  //for next word
                j=0;    //for next word, init index to 0
            }
            else
            {
                user_input[w][j]=result[i];
                j++;
            }
        }

        //In case the parent accepts a SIGCHLD signal
        //it means that a child has ended suddenly and 
        //he has to create a new Worker with the same 
        //directories as the dead Worker.       
        if(worker_pid != -10)
        {
            for(i=0;i<times;i++)
            {
                if(worker_info_array[i].pid == worker_pid)
                {
                    if((pid = fork()) == -1)
                    {
                        printf("Faild to fork\n");
                        exit(1);
                    }
                    else if(pid == 0)
                    {
                        char buffersize[10];
                        sprintf(buffersize,"%d",bufferSize);
                        char* args[] = {"./worker", worker_info_array[i].pipe_P2C, buffersize, NULL};
                        execv("./worker", args);
                    }
                    worker_info_array[i].pid = pid;  //Save the pid of each Worker in the struct

                    char msg[20*num_of_countries];
                    char num_of_dir[20];
                    sprintf(num_of_dir,"%d",worker_info_array[i].num_of_directories);

                    strcpy(msg,"");

                    strncat(msg,input_dir,strlen(input_dir));
                    strncat(msg,"\n",1);

                    strncat(msg,num_of_dir,strlen(num_of_dir));
                    strncat(msg,"\n",1);

                    strncat(msg,worker_info_array[i].pipe_C2P,strlen(worker_info_array[i].pipe_C2P));
                    strncat(msg,"\n",1);

                    //Save the name of directories of each Worker.
                    for(j=0;j<worker_info_array[i].num_of_directories;j++)
                    {
                        strncat(msg,worker_info_array[i].worker_directories[j],strlen(worker_info_array[i].worker_directories[j]));
                        if((j<worker_info_array[i].num_of_directories-1))  strncat(msg," ",1);
                        else  strncat(msg,"\0",1); 
                    }
                    
                    //Start writing in the pipe P2C with size bufferSize
                    Write_Pipe(msg,worker_info_array[i].pipe_P2C,bufferSize);
                    
                    //Take the statistics again, but now I don't have to print them, because they are the same as before.
                    char* statistics;
    
                    statistics = Read_Pipe(&statistics,worker_info_array[i].pipe_C2P,bufferSize);
                    
                    // printf("%s",statistics);

                    free(statistics);

                    //Restart the flag for SIGCHLD
                    worker_pid =-10;
                }
            }
        }

        if(strcmp(user_input[0],"/listCountries")==0)
        {
            if(w==1)
            {
                //Begin the prossess of writting and reading data through the pipes.
                for(i=0;i<times;i++)
                {
                    //Send the user input to all Workers
                    Write_Pipe(result,worker_info_array[i].pipe_P2C,bufferSize);
                }

                //Read the Workers response to user input
                for(i=0;i<times;i++)
                {
                    char* list_countries=NULL;
                    
                    list_countries = Read_Pipe(&list_countries,worker_info_array[i].pipe_C2P,bufferSize);
                    
                    printf("%s",list_countries);

                    free(list_countries);      
                }
                success++;
            }
            else
            {
                printf("ERROR\n");
                failure++;
            }
            
        }        
        else if(strcmp(user_input[0],"/exit")==0) 
        {
            if(w==1)
            {
                for(i=0;i<times;i++)
                {
                    // Write_Pipe(result,worker_info_array[i].pipe_P2C,bufferSize);
                    kill(worker_info_array[i].pid,SIGKILL);
                }
                success++;

                char* file_name;
                char pid[10];

                file_name = (char*)malloc((strlen("log_file.")+1)*sizeof(char));
                strcpy(file_name,"log_file.");

                sprintf(pid,"%d", getpid());

                file_name = (char*)realloc(file_name, (strlen(file_name)+1 ) + (strlen(pid)+1));
                strcat(file_name,pid); 

                FILE *fp;

                if((fp = fopen(file_name, "w")) == NULL)
                { 
                    perror("Can't create the file.");
                    break;
                }

                for(i=0;i<num_of_countries;i++)
                {
                    fputs(countries_array[i], fp);
                    fputs("\n", fp);
                }

                char totals[50], successes[50], failures[50];
               
                sprintf(totals,"%d",total);
                sprintf(successes,"%d",success);
                sprintf(failures,"%d",failure);

                fputs("TOTAL ", fp);
                fputs(totals,fp);
                fputs("\n", fp);

                fputs("SUCCESS ", fp);
                fputs(successes,fp);
                fputs("\n", fp);

                fputs("FAIL ", fp);
                fputs(failures,fp);
                fputs("\n", fp); 

                fclose(fp); 

                free(file_name);              

                break;
            }
            else
            {
                printf("ERROR\n");
                failure++;
            }
            
        }
        else if(strcmp(user_input[0],"/diseaseFrequency") == 0)
        {
            if((w == 4) || (w == 5))  
            {
                //In case there is no country given
                if(w==4)
                {
                    //Begin the prossess of writting and reading data through the pipes.
                    for(i=0;i<times;i++)
                    {
                        Write_Pipe(result,worker_info_array[i].pipe_P2C,bufferSize);
                    }

                    int frequency=0, error=0;
                    for(i=0;i<times;i++)
                    {
                        char* freq=NULL;
                        
                        freq = Read_Pipe(&freq,worker_info_array[i].pipe_C2P,bufferSize);
                        
                        //Sum all the responses of Workers, if they don't return Erro.
                        if(strcmp(freq,"ERROR") != 0)  
                            frequency+=atoi(freq);
                        else
                        {
                            error++;
                            printf("ERROR\n");
                        }

                        free(freq);      
                    }
                    if(error != times)
                    { 
                        printf("%d\n",frequency);
                        success++;
                    }
                    else failure++;
                }
                else //I n case country is given
                {
                    //Find the Worker that has the specific country
                    int worker=-1;
                    for(i=0;i<times;i++)
                    {
                        for(j=0;j<worker_info_array[i].num_of_directories;j++)
                        {
                            if(strcmp(worker_info_array[i].worker_directories[j],user_input[4]) == 0)
                                worker=i; 
                        }
                    }

                    //If there is such a country, then write only to him the user input
                    if(worker!=-1)
                    {
                        Write_Pipe(result,worker_info_array[worker].pipe_P2C,bufferSize);

                        char* freq=NULL;
                        
                        freq = Read_Pipe(&freq,worker_info_array[worker].pipe_C2P,bufferSize);
                    
                        if(strcmp(freq,"ERROR") != 0) 
                        { 
                            printf("%s\n",freq);
                            success++;
                        }
                        else
                        {  
                            printf("ERROR\n");
                            failure++;
                        }
                        free(freq);
                    }
                    else
                    {
                        printf("ERROR\n");
                        failure++;
                    }
                                            
                }
            }
            else
            {
                printf("ERROR\n");
                failure++;
            }
        }
        else if(strcmp(user_input[0],"/topk-AgeRanges")==0)
        {
            if(w==6)
            {
                //Find the Worker that has the specific Country
                int worker=-1;
                for(i=0;i<times;i++)
                {
                    for(j=0;j<worker_info_array[i].num_of_directories;j++)
                    {
                        if(strcmp(worker_info_array[i].worker_directories[j],user_input[2]) == 0)
                            worker=i; 
                    }
                }

                //If there is such a country then send him through the pipe the user input
                if(worker!=-1)
                {
                    Write_Pipe(result,worker_info_array[worker].pipe_P2C,bufferSize);

                    char* topk=NULL;
                    
                    topk = Read_Pipe(&topk,worker_info_array[worker].pipe_C2P,bufferSize);
                
                    if((strcmp(topk,"ERROR") != 0) && (strcmp(topk,"empty") != 0))  
                    {
                        printf("%s",topk);
                        success++;
                    }
                    else
                    {  
                        printf("ERROR\n");
                        failure++;
                    }
                    
                    if(topk != NULL)  free(topk);
                }
                else
                {
                    printf("ERROR\n");
                    failure++;
                }
            }
            else
            {
                printf("ERROR\n");
                failure++;
            }
        }
        else if(strcmp(user_input[0],"/numPatientAdmissions") == 0)
        {
            if((w == 4) || (w == 5))  
            {
                if(w==4)  //If country is not given
                {
                    for(i=0;i<times;i++)
                    {
                        Write_Pipe(result,worker_info_array[i].pipe_P2C,bufferSize);
                    }

                    int error=0, temp=0;
                    char* num_pat=NULL;
                    
                    for(i=0;i<times;i++)
                    {    
                        num_pat=NULL;
                        num_pat = Read_Pipe(&num_pat,worker_info_array[i].pipe_C2P,bufferSize);
                        
                        if((strcmp(num_pat,"ERROR") == 0) || (strcmp(num_pat,"empty") == 0))
                        {
                            error++;
                            printf("ERROR\n");
                        }
                        else
                        {
                            printf("%s",num_pat);
                            if(temp==0)
                            {
                                success++;
                                temp=1;
                            }
                        }
                        if(num_pat!=NULL)   free(num_pat);
                    }
                    if(error == times)  failure++;
                }
                else  //If country is given do the same as disease Frequency
                {
                    int worker=-1;
                    for(i=0;i<times;i++)
                    {
                        for(j=0;j<worker_info_array[i].num_of_directories;j++)
                        {
                            if(strcmp(worker_info_array[i].worker_directories[j],user_input[4]) == 0)
                                worker=i; 
                        }
                    }

                    if(worker!=-1)
                    {
                        Write_Pipe(result,worker_info_array[worker].pipe_P2C,bufferSize);

                        char* num_pat=NULL;
                        
                        num_pat = Read_Pipe(&num_pat,worker_info_array[worker].pipe_C2P,bufferSize);
                    
                        if((strcmp(num_pat,"ERROR") != 0) && (strcmp(num_pat,"empty") != 0)) 
                        { 
                            printf("%s",num_pat);
                            success++;
                        }
                        else
                        {
                            printf("ERROR\n");
                            failure++;
                        }
                        free(num_pat);
                    }
                    else
                    {
                        printf("ERROR\n");
                        failure++;
                    }
                                            
                }
            }
            else
            {
                printf("ERROR\n");
                failure++;
            }
        }
        else if(strcmp(user_input[0],"/numPatientDischarges") == 0)  //The same behavior as numPatientAdmissions
        {
            if((w == 4) || (w == 5))  
            {
                if(w==4)
                {
                    //Begin the prossess of writting and reading data through the pipes.
                    for(i=0;i<times;i++)
                    {
                        Write_Pipe(result,worker_info_array[i].pipe_P2C,bufferSize);
                    }

                    int error=0,temp=0;
                    char* num_pat=NULL;
                    
                    for(i=0;i<times;i++)
                    {    
                        num_pat=NULL;
                        num_pat = Read_Pipe(&num_pat,worker_info_array[i].pipe_C2P,bufferSize);
                        
                        if((strcmp(num_pat,"ERROR") == 0) || (strcmp(num_pat,"empty") == 0))
                        {
                            error++;
                            printf("ERROR\n");
                        }
                        else
                        {
                            printf("%s",num_pat);
                            if(temp==0)
                            {
                                success++;
                                temp=1;
                            }
                        }
                        if(num_pat!=NULL)   free(num_pat);
                    }
                    if(error == times)  failure++;
                }
                else
                {
                    int worker=-1;
                    for(i=0;i<times;i++)
                    {
                        for(j=0;j<worker_info_array[i].num_of_directories;j++)
                        {
                            if(strcmp(worker_info_array[i].worker_directories[j],user_input[4]) == 0)
                                worker=i; 
                        }
                    }

                    if(worker!=-1)
                    {
                        Write_Pipe(result,worker_info_array[worker].pipe_P2C,bufferSize);

                        char* num_pat=NULL;
                        
                        num_pat = Read_Pipe(&num_pat,worker_info_array[worker].pipe_C2P,bufferSize);
                    
                        if((strcmp(num_pat,"ERROR") != 0) && (strcmp(num_pat,"empty") != 0))  
                        {
                            printf("%s",num_pat);
                            success++;
                        }
                        else  
                        {
                            printf("ERROR\n");
                            failure++;
                        }
                        free(num_pat);
                    }
                    else
                    {
                        printf("ERROR\n");
                        failure++;
                    }
                                            
                }
            }
            else
            {
                printf("ERROR\n");
                failure++;
            }
        }  
        else if(strcmp(user_input[0],"/searchPatientRecord")==0)
        {
            if(w==2)  //There must be at least 2 arguments in the user input
            {
                //Begin the prossess of writting and reading data through the pipes.
                for(i=0;i<times;i++)
                {
                    Write_Pipe(result,worker_info_array[i].pipe_P2C,bufferSize);
                }

                int error=0;

                for(i=0;i<times;i++)
                {
                    char* record=NULL;
                    
                    record = Read_Pipe(&record,worker_info_array[i].pipe_C2P,bufferSize);
                    
                    if((strcmp(record,"ERROR") != 0) && (strcmp(record,"empty") != 0))
                    {  
                        printf("%s",record);
                        success++;
                    }
                    else
                    {
                        printf("ERROR\n");
                        error++;
                    }
                    
                    if(record != NULL)  free(record);
                }
                if(error == times)  failure++;
            }
            else
            {
                printf("ERROR\n");
                failure++;
            }
        } 
        else if(strcmp(user_input[0],"/print_records")==0)  
        {
            for(i=0;i<times;i++)
            {
                Write_Pipe(result,worker_info_array[i].pipe_P2C,bufferSize);
            }

            for(i=0;i<times;i++)
            {
                char* record=NULL;
                        
                record = Read_Pipe(&record,worker_info_array[i].pipe_C2P,bufferSize);
                
                if((strcmp(record,"OK") == 0))
                {  
                    free(record);
                }
            }
            success++;
        } 
        else if(strcmp(user_input[0],"/old_exit")==0)  
        {
            for(i=0;i<times;i++)
            {
                Write_Pipe(result,worker_info_array[i].pipe_P2C,bufferSize);
            }
            success++;
            break;
        }   
        else 
        {
            for(i=0;i<times;i++)
            {
                Write_Pipe("THE_END",worker_info_array[i].pipe_P2C,bufferSize);
            }
            printf("ERROR\n");
            failure++;
        }
    }

    int z = 0;
    for (z; z < times; z++) wait(NULL);

    
    //Deallocate memory.
    for(i=0; i<times ; i++)
    {
        if(remove(worker_info_array[i].pipe_P2C) != 0)
            printf("Fail to delete the named Pipe.\n");
        
        if(remove(worker_info_array[i].pipe_C2P) != 0)
            printf("Fail to delete the named Pipe.\n");
    }

    for(i=0; i<times; i++)
    {
        for(j=0;j<(worker_info_array[i].num_of_directories);j++)
            free(worker_info_array[i].worker_directories[j]);
        free(worker_info_array[i].worker_directories);
    }

    for(i=0; i<num_of_countries; ++i)
        free(countries_array[i]);
    free(countries_array);

    free(input_dir);


    return 0;
}
