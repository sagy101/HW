/*	smash.c
main file. This contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "commands.h"
#include "signals.h"



#define MAX_LINE_SIZE 80
#define MAXARGS 20

extern char* L_Fg_Cmd;//command (string) of the foreground process
extern int FG_pID;//pid of the foreground process 
extern job** jobs;// array of jobs
char lineSize[MAX_LINE_SIZE];//array for in
//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
    char cmdString[MAX_LINE_SIZE]; 	   

	 //ctrl+z
	struct sigaction stop;
	stop.sa_handler = &catch_stop;
	sigaction(SIGTSTP, &stop, NULL);

	//ctrl+c
	struct sigaction init;
	init.sa_handler = &catch_init;
	sigaction(SIGINT, &init, NULL);

	// Init globals 
	jobs = (job**)malloc(sizeof(job*)*101);
	if (jobs == NULL) {
		printf("smash error: > jobs memory allocotion error\n");
		return -1;
	}
	for (int i = 0; i <= 100; i++) jobs[i] = NULL;

	L_Fg_Cmd = (char*)malloc(sizeof(char) * (MAX_LINE_SIZE + 1));
	if (L_Fg_Cmd == NULL) {
		free(jobs);
		printf("smash error: > L_Fg_Cmd memory allocotion error\n");
		return -1;
	}
    while (1)
    {
		printf("smash > ");
		fgets(lineSize, MAX_LINE_SIZE, stdin);
		strcpy(cmdString, lineSize);    	
		cmdString[strlen(lineSize)-1]='\0';
		// perform a complicated Command
		if(!ExeComp(lineSize)) continue; 	
		// built in and background commands
		ExeCmd(jobs, lineSize, cmdString);
		
		/* initialize for next line read*/
		lineSize[0]='\0';
		cmdString[0]='\0';
	}
	for (int i = 0; i <= 100; i++)//free jobs
		rem_job(jobs, i);
	free(jobs);
	free(L_Fg_Cmd);
    return 0;
}

