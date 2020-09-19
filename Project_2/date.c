#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#define __USE_XOPEN 
#include <time.h>
#include "date.h"

//Convert the date to Unix timestamps (in seconds).
//Every timestamp represent a unique date.
time_t date_to_seconds(const char *date)
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

        if(date_to_seconds(*dt1)>date_to_seconds(*dt2))
                return 1;
        if(date_to_seconds(*dt1)<date_to_seconds(*dt2))
                return -1;
        else
                return 0;
}