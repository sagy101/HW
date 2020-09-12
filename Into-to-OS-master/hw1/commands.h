#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "signals.h"

#define MAX_LINE_SIZE 80
#define MAX_ARG 20
typedef enum { FALSE , TRUE } bool;
typedef enum { RUNNING , STOPPED , TERMINATED } status;//status of job

int FG_pID;//pid of the foreground process

typedef struct _job {
	int id;
	char* name;
	int pid;
	time_t *startTime;
	status sts;
} job;

//**************************************************************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeComp(char* lineSize);

//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
int ExeCmd(job** jobs, char* lineSize, char* cmdString);

//**************************************************************************************
// function name: ExeComp
// Description: executes complicated command
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
int ExeExternal(char* args[MAX_ARG], char* cmdString, bool BG);

//**************************************************************************************
// function name: pwd
// Description: prints present directory
// Parameters: -
// Returns: 0- succesful -1- otherwise
//**************************************************************************************
int Pwd(char pwd[MAX_LINE_SIZE]);

//**************************************************************************************
// function name: cd
// Description: changes the current working directory
// Parameters: name of directory 
// Returns: 0- succesful -1- otherwise
//**************************************************************************************
int Cd(char* path);

//**************************************************************************************
// function name: quit
// Description: quits smash, if fkill is true then also terminates/kills all child processes
// Parameters: pointer to jobs, fKill - if true terminates/kills all child processes
// Returns: 0 - success, -1 - failure
//**************************************************************************************
int quit(job** jobs, bool fKill);

//**************************************************************************************
// function name: Showpid
// Description: prints smashs' PID
// Parameters: none
// Returns: 0 - no failure
//**************************************************************************************
int Showpid();

//**************************************************************************************
// function name: Mv
// Description: changes name of file
// Parameters: old_name - name of file to be changed, new_name - name to be changed to
// Returns: 0 - success (doesnt fail)
//**************************************************************************************
int Mv(char* old_name, char* new_name);

//**************************************************************************************
// function name: History
// Description: prints last 50 commands that were inputted to smash
// Parameters: null
// Returns: 0 - success (doesnt fail)
//**************************************************************************************
int History();
void updateCmdHistory(char* lineSize);

//**************************************************************************************
// function name: add_job
// Description: adds job with given parameters and updated last job +1
// Parameters: jobs - pointer to jobs, cmd - name of job to be added, pid - pid of job to be added, sts - status of job to be added
// Returns: void
//**************************************************************************************
void add_job(job** jobs, char* cmd, int pid, status sts);

//**************************************************************************************
// function name: rem_job
// Description: removes job of given id in jobs, and sets new lastJob
// Parameters: jobs - pointer to jobs, id - index of job to be removed
// Returns: void
//**************************************************************************************
void rem_job(job** jobs, int id);

//**************************************************************************************
// function name: Fg
// Description: brings process from background to frontground by id (no id meant last process that moved to background)
// Parameters: job number, number of arguments, jobs array.
// Returns: 0 for succes/self care errors, -1 for syscall errors.
//*************************************************************************************
int Fg(char* commandNum, int num_arg, job** jobs);

//**************************************************************************************
// function name: Jobs
// Description: lists all current listed jobs and their characteristics
// Parameters: jobs - pointer to jobs
// Returns: 0 - success, -1 - failure
//**************************************************************************************
int Jobs(job** jobs);

//**************************************************************************************
// function name: updateJobs
// Description: updated status of jobs, deletes them if they dont exist anymore (terminated/exit...)
// Parameters: pointer to jobs
// Returns: void
//**************************************************************************************
void updateJobs(job** jobs);

//**************************************************************************************
// function name: Kill
// Description: send signal, by signum to a job.
// Parameters: jobs array, signal macro, job number
// Returns: 0 
//*************************************************************************************
int Kill(job** jobs, char* flagSignum, char* job_id);

//**************************************************************************************
// function name: Bg
// Description: continue to run a stopped background process
// Parameters: job number, number of arguments, jobs array.
// Returns: 0 for succes/self care errors, -1 for syscall errors.
//*************************************************************************************
int Bg(job** jobs, char* jobId, int num_arg);
#endif


