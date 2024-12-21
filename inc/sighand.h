#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

void signal_handler00(int signum);

void signal_handler01(int signum);

void signal_intallchild(pid_t arr[],int arrSize);