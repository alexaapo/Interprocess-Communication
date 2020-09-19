#include <signal.h>

#ifndef HANDLER_WORKER_H
#define HANDLER_WORKER_H

void Handler1(int, siginfo_t *, void *);
void Handler_Usr1(int, siginfo_t *, void *);

#endif