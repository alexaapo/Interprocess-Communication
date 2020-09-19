# Interprocess-Communication
Project II is associated with creating processes using system calls such as fork/exec, communicating processes through pipes, using low-level I/O, and creating bash scripts.

As part of this work I implemented a distributed information processing tool that will receive, process, record and answer questions about viruses (just like Project I). Specifically, I implement the diseaseAggregator application which will create Worker processes that, together with the application, will answer user's questions.

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

