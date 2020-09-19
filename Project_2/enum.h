#include <stdio.h>

enum Record_error
{
    Record_ID_already_exists = 1,
    Entry_Date_after_Exit_Date,
    Fisrt_Name_has_blanks,
    Invalid_First_Name,
    Last_Name_has_blanks,
    Invalid_Last_Name,
    Disease_ID_has_blanks,
    Invalid_Disease_ID,
    Country_Name_has_blanks,
    Invalid_Country_Name,
    Invalid_Entry_Day,
    Invalid_Entry_Month,
    Invalid_Entry_Year,
    Invalid_Entry_Date,
    Invalid_Exit_Day,
    Invalid_Exit_Month,
    Invalid_Exit_Year,
    Invalid_Exit_Date,
    Invalid_Leap_day,
    Invalid_Input,
    There_is_no_Record_with_this_Record_ID,
    Invalid_Record_ID,
    Invalid_Record_Age
};