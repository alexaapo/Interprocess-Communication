#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "handler_worker.h"

extern volatile sig_atomic_t flag1;
extern volatile sig_atomic_t flag2;

void Handler1(int sig, siginfo_t *siginfo, void *context)
{
    flag1=1;
}

void Handler_Usr1(int sig, siginfo_t *siginfo, void *context)
{
    flag2=1;
}