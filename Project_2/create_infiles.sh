#!/bin/bash

#Check for valid number of arguments
if [ "$#" -ne 5 ]; then
    echo "You must put 5 arguments."; exit 1
#else 
    #echo "OK you have put 5 arguments."
fi

#Check for valid integer arguments
re='^[0-9]+$'
if ! [[ $4 =~ $re ]] ; then
   echo "Your 4th argument must be an integer" >&2; exit 1
fi

re='^[0-9]+$'
if ! [[ $5 =~ $re ]] ; then
   echo "Your 5th argument must be an integer" >&2; exit 1
fi

#Read the diseases File and store the diseases in an array for later.
diseasesFile=$1
dis_arr=()

while read line || [ -n "$line" ]; do 
# echo $line
dis_arr+=($line)
done < $diseasesFile

# echo ${dis_arr[*]}

#Read the names File and store the names in an array for later.
names_array=("Liam" "Emma" "Noah" "Olivia" "William" "Ava" "James" "Isabella" "Oliver" "Sophia" "Benjamin" "Charlotte" "Elijah" "Mia" "Lucas" "Amelia" "Mason" "Harper" "Logan" "Evelyn")

# echo ${names_array[*]}

#Store the surnames in an array for later.
surnames_array=("Smith" "Jones" "Taylor" "Brown" "Williams" "Wilson" "Johnson" "Davies" "Robinson" "Wright" "Thompson" "Evans" "Walker" "White" "Roberts" "Green" "Hall" "Wood" "Jackson" "Clarke")

# echo ${names_array[*]}

#Store the days in an array
days_arr=("01" "02" "03" "04" "05" "06" "07" "08" "09" "10" "11" "12" "13" "14" "15" "16" "17" "18" "19" "20" "21" "22" "23" "24" "25" "26" "27" "28" "29" "30")
# echo ${days_arr[*]}

#Store the months in an array
months_arr=("01" "02" "03" "04" "05" "06" "07" "08" "09" "10" "11" "12")
# echo ${months_arr[*]}

#Create a directory
input_dir=$3
# mkdir -p /home/alexa/Documents/SYSPRO/ERGASIA_2/input_dir;
if [ ! -d ./$input_dir ]; then
  mkdir -p ./$input_dir;
fi

#Helping functions to go in a specific directory or in a previous one
function change_path() {
  cd ./$1
}

function change_path3() {
  cd ..
}

#Store the ENTER and EXIT in an array.
enter_exit_array=("ENTER" "EXIT")

#Read the countries File
countriesFile=$2
numFilesPerDirectory="$4"
numRecordsPerFile="$5"
id=0
# pwd

#Create a directory for each Country in the input dir (if it hasn't exist yet)
while read line || [ -n "$line" ]; do 
  # echo $line

  change_path $input_dir
  # pwd

  if [ ! -d ./$line ]; then
    mkdir -p ./$line;
  fi

  #Go to this subdirectory to create date files
  change_path $line
  # pwd
  #-----------------------------------------------------------------------
  #Create random records in each file
  b=0
  directory_records=()
  # exit_records=()

  # Find if the product of numFilesPerDirectory and numRecordsPerFile is odd or even
  even_or_odd=$((($numFilesPerDirectory*$numRecordsPerFile)%2))
  if [ "$even_or_odd" == 1 ]; then
    num_of_records=$((($numFilesPerDirectory*$numRecordsPerFile)/2 +1))
  else
    num_of_records=$((($numFilesPerDirectory*$numRecordsPerFile)/2))
  fi

  #Create random records and save them to a array
  until [ ! $b -lt $num_of_records ]
  do
    random_name=${names_array[$RANDOM % ${#names_array[@]}]}
    random_surname=${surnames_array[$RANDOM % ${#surnames_array[@]}]}
    random_disease=${dis_arr[$RANDOM % ${#dis_arr[@]}]}

    CEILING=120;
    RANGE=$(($CEILING+1));
    random_age=$RANDOM;
    let "random_age %= $RANGE";
    random_age=$(($random_age));
    
    space=" "

    directory_records+=("$id$space${enter_exit_array[0]}$space$random_name$space$random_surname$space$random_disease$space$random_age")
    directory_records+=("$id$space${enter_exit_array[1]}$space$random_name$space$random_surname$space$random_disease$space$random_age")

    id=`expr $id + 1`
    b=`expr $b + 1`
  done

  if [ "$even_or_odd" == 1 ]; then
    directory_records=( "${directory_records[@]:0:((${#directory_records[@]})-1)}" )
  fi

  # echo ${directory_records[*]}

  a=0
  until [ ! $a -lt $numFilesPerDirectory ]
  do
    # pick a random day, month, year for the new file
    random_day=${days_arr[$RANDOM % ${#days_arr[@]}]}
    random_month=${months_arr[$RANDOM % ${#months_arr[@]}]}
    
    FLOOR=1900;
    CEILING=2020;
    RANGE=$(($CEILING-$FLOOR+1));
    random_year=$RANDOM;
    let "random_year %= $RANGE";
    random_year=$(($random_year+$FLOOR));

    temp="-"
    date="$random_day$temp$random_month$temp$random_year"
    FILE="$date".txt
    if test -f "$FILE"; then
      c=0
    else
      # echo "$date"
      touch "$date".txt #Create an empty file with date name

      #Create random records in each file 
      c=0
      until [ ! $c -lt $numRecordsPerFile ]
      do
        random_number=$RANDOM
        let "random_number %= ${#directory_records[@]}";

        echo ${directory_records[$random_number]} >> "$date".txt
        # echo ${directory_records[$random_number]}
        
        #Concatenate the array after we pick a record, so I don't select it again.
        directory_records=( "${directory_records[@]:0:$random_number}" "${directory_records[@]:$(($random_number + 1))}" )

        c=`expr $c + 1`
      done
      
      a=`expr $a + 1`
    fi
  done
  #Go to the source directory so as to create new subdirectories and new date files
  change_path3
  change_path3
done < $countriesFile