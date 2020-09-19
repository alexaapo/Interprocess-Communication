#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include  <sys/types.h>
#include <fcntl.h>
#include  <sys/stat.h>
#include <sys/errno.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
// #include "worker_info.h"

int main(int argc, char** argv)
{
    printf("This is a inactive Worker with id: %d\n",getpid());
    exit(1);
}
