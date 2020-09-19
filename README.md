# Interprocess-Communication
Project II is associated with creating processes using system calls such as fork/exec, communicating processes through pipes, using low-level I/O, and creating bash scripts.

As part of this work I implemented a distributed information processing tool that will receive, process, record and answer questions about viruses (just like Project I). Specifically, I implement the diseaseAggregator application which will create Worker processes that, together with the application, will answer user's questions. The application has this architecture in general:

![Screenshot from 2020-09-19 19-38-59](https://user-images.githubusercontent.com/60033683/93671990-f2537d00-faaf-11ea-8565-0844d64243b8.png)

A) The **diseaseAggregator** application:

The diseaseAggregator application will be used as follows: 

./diseaseAggregator –w numWorkers -b bufferSize -i input_dir where: 
- The **numWorkers** parameter is the Worker number of processes that the application will create.
- The **bufferSize** parameter : is the size of the buffer for reading over the pipes.
- The **input_dir** parameter: is a directory that contains subdirectories with the files to be processed by Workers. Each subdirectory will have the name of a country and will contain files with names that are DD-MM-YYYY form dates. For example input_dir could contain subdirectories China which have the following files: 

/input_dir/China/21-12-2019  
/input_dir/China/22-12-2019  
/input_dir/China/23 -12-2019  

Each DD-MM-YYYY file contains a series of patient records where each line describes a patient who was entered/discharged to/from a hospital that day and contains the recordID, name, virus, and age. For example if the contents of the file /input_dir/Germany/02-03-2020 of the file are: 

889 ENTER Mary Smith COVID-2019 23  
776 ENTER Larry Jones SARS-1 87  
125 EXIT Jon Dupont H1N1 62  

means that in Germany, on 02-03-2020 two patients (Mary Smith and Larry Jones) with cases of COVID-2019 and SARS-1 entered the hospital while Jon Dupont with H1N1 was discharged from hospital. For an EXIT record there must already be an ENTER record in the system with the same recordID and
earlier date of admission. Otherwise the word ERROR is printed on a blank line (see also the description of Workers below)

To get started, **diseaseAggregator** should start numWorkers Workers child processes and distribute the subdirectories evenly with the countries in input_dir to Workers. It will start the Workers and will have to inform each Worker through named pipe about the subdirectories that the Worker will undertake. When the application (parent process) finishes creating the Worker processes, it will wait for input (commands) from the user from the keyboard (see commands below). 

Each **Worker** process, for each directory assigned to it, will read all its files in chronological order based on the file names and fill in the data structures that it will use to answer questions prompted by the parent process (from Project I). When he has finished reading a file, Worker will send, through named pipe, to the parent process, summary statistics of the file containing the following information: for each virus, in that country, it will send the number of cases per age category. For example, for a file /input_dir/China/22-12-2020 containing records for SARS and COVID-19 it will send summary statistics such as:  

22-12-2020  
China  
SARS  
Age range 0-20 years: 10 cases  
Age range 21 -40 years: 23 cases  
Age range 41-60 years: 34 cases  
Age range 60+ years: 45 cases  

COVID-19  
Age range 0-20 years: 20 cases  
Age range 21-40 years: 230 cases  
Age range 41-60 years : 340 cases  
Age range 60+ years: 450 cases  

These statistics should be sent for each input file (ie for each date file) by a Worker. When the Worker finishes reading its files and has sent all the summary statistics, it notifies the parent process through named pipe that the Worker is ready to accept requests. If a Worker receives a **SIGINT** or **SIGQUIT** signal then it prints to a file named log_file.xxxx (where xxx is its process ID) the name of the countries (subdirectories) it manages, the total number of requests received by the parent process and the total number of requests answered successfully (ie there was no error in the query). 

Logfile format: 

Italy  
China  
Germany  
TOTAL 29150  
SUCCESS 25663  
FAIL 3487

If a Worker receives a **SIGUSR1** signal, it means that 1 or more new files have been placed in one of the subdirectories assigned to it. It will check subdirectories to find new files, read them and update the data structures it holds in memory. For each new file, it will send summary statistics to the parent process. If a Worker process terminates abruptly, the parent process will have to fork a new Worker process to replace it. Therefore, the parent process should handle the **SIGCHLD** signal, as well as **SIGINT** and **SIGQUIT**. If the parent process receives **SIGINT** or **SIGQUIT**, first it must finish processing the current command from the user and after responding to the user, it will send a **SIGKILL** signal to the Workers, wait for them to terminate, and finally print a file named log_file.xxxx where xxx is its process ID, the name of all countries (subdirectories) that "participated" in the data application, the total number of requests received by the user and successfully answered and the total number requests where an error occurred and/or was not completed.

The user will be able to give the following commands to the application: 

● /listCountries  
The application will print each country along with the process ID of the Worker process that manages its files. This command is useful when the user wants to add new files of a country for editing and needs to know what the Worker process is to send him a notification through SIGUSR1 signal.  
Output format: one line for each country and process ID.  
Example:  

Canada 123  
Italy 5769  
China 5770  

● /diseaseFrequency virusName date1 date2 [country]  
If no country argument is given, the application will print for virusName the number of cases recorded in the system during [date1 ... date2]. If a country argument is given, the application will print for the virusName disease, the number of cases in the country that have been recorded in the period [date1 ... date2]. The date1 date2 arguments will be in DD-MM-YYYY format.  
Output format: a line with the number of cases.  
Example:  
153 

● /topk-AgeRanges k country disease date1 date2  
The application will print, for the country and the virus disease the top k age categories that have shown cases of the specific virus in the specific country and their incidence rate. The date1 date2 arguments will be in DD-MM-YYYY format.  
Output format: one line for each age group.  
Example with k = 2:  
20-30: 40%  
60+: 30%  

● /searchPatientRecord recordID  
The parent process forwards to all Workers the request through named pipes and expects a response from the Worker with the record recordID. When he receives it, he prints it.  
Example for someone who went to the hospital 23-02-2020 and came out 28-02-2020:  
776 Larry Jones SARS-1 87 23-02-2020 28-02-2020  
Example for someone who went to the hospital 23-02-2020 and not yet released:  
776 Larry Jones SARS-1 87 23-02-2020 -  

● /numPatientAdmissions disease date1 date2 [country]  
If country country is given, the application will print, on a new line, the total number of patients entered to hospital with the disease in the specific country within [date1 date2]. If no country argument is given, the application will print, for each country, the number of patients with the disease who were entered to the hospital in the period [date1 date2]. Date1 date2 arguments will be in the format DD-MM-YYYY.  
Output format:  
Italy 153   
Spain 769  
France 570  

● /numPatientDischarges disease date1 date2 [country]  
If given the country argument, the application will print on a new line, the total number of patients with the disease who have been discharged from the hospital in that country within the period [date1 date2]. If no country argument is given, the application will print, for each country, the number of patients with the disease who have been discharged from the hospital in the period [date1 date2]. The date1 date2 arguments will be in DD-MM-YYYY format.  
Example:  
Italy 153  
Spain 769  
France 570  

● /exit  
Exit the application. The parent process sends a SIGKILL signal to the Workers, waits for them to terminate, and prints it in a file called log_file.xxxx where xxx is its process ID, the name of the countries (subdirectories) that "participated" in the application, the total number of requests received by the user and answered successfully and the total number of requests where an error occurred and / or were not completed. Before it terminates, it will free up all the free memory properly.  
Example:  
Italy   
China  
Germany  
TOTAL 29150  
SUCCESS 25663  
FAIL 3487

B) The **script create_infiles.sh** 

I wrote a bash script that creates test subdirectories and input files that I used to debug my program. The script create_infiles.sh works as follows: 

**./create_infiles.sh diseasesFile countriesFile input_dir numFilesPerDirectory numRecordsPerFile**

- **diseaseFile**: a file with virus names (one per line) 
- **countriesFile**: a file with country names (one per line) 
- **input_dir**: name of a directory where the subdirectories and input files will be placed 

The script does the following: 

1. It checks for input numbers 
2. It reads the diseasesFile and countriesFile files 
3. Creates a directory with the name given in the third argument input_dir
4. In input_dir create subdirectories, one for each country name within countriesFile
5. In each subdirectory, it creates numFilesPerDirectory files with a DD-MM-YYYY date name. In each file, place numRecordsPerFile following the format of the input files described above. For diseases, the script will be randomly selected by one of the diseasesFile. 

## Execution:
For the bash script:  
**./create_infiles.sh diseases.txt countries.txt input_dir 5 5**

For the application diseaseAggregator:  
1. **make**
2. **valgrind --trace-children=yes --track-origins=yes --leak-check=full --show-leak-kinds=all ./diseaseAggregator -i input_dir -b 1 -w 6**
3. **make clean**

