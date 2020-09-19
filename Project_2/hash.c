#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int Hash_Function(char* name, int num) 
{
    int sum = 0, i = 0;
    for (sum, i; i < strlen(name); i++)
        sum += name[i];
    return (sum % num);
}

int Hash_Function2(char* name, int num)
{
    int h=0, a=127;
    for(;*name != '\0';name++)
        h = (a*h + *name) % num;
    return h;
}

int Hash_Function3(char* name, int num)
{
    int h, a=31415, b=27183;
    for(h=0; *name != '\0'; name++, a = a*b % (num-1))
        h = (a*h + *name) % num;
    return h;
}
