#ifndef _SIGS_H
#define _SIGS_H
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "commands.h"

//**************************************************************************************
// function name: catch_stop
// Description: handling ctrl z signal
// Parameters: signal number
// Returns: void
//*************************************************************************************
void catch_stop(int sig_num);

//**************************************************************************************
// function name: catch_init
// Description: handling ctrl c signal
// Parameters: signal number
// Returns: void
//*************************************************************************************
void catch_init(int sig_num);

//**************************************************************************************
// function name: kill_handler
// Description: handling signals, prints message accordingly
// Parameters: PID, signal number
// Returns: -1 if signal sending failed, 0 if success
//*************************************************************************************
int kill_handler(int pid, int sig_num);


#endif

