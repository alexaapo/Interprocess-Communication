#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "handler.h"

extern volatile sig_atomic_t flag;
extern volatile sig_atomic_t worker_pid;

void Handler(int sig, siginfo_t *siginfo, void *context)
{
    flag=1;
}

void Handler_New_Worker(int sig, siginfo_t *siginfo, void *context)
{
    worker_pid = (long)siginfo->si_pid;
}