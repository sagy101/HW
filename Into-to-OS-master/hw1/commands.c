
//		commands.c
//********************************************
#include "commands.h"

char cmdHist[50][MAX_LINE_SIZE];//history of 50 last commands
int cmdHistCount = 0;//number of recorded commands
int lastJob = 0;//index of last job in jobs
int FG_pID;//pid of the foreground process
char* L_Fg_Cmd;//command (string) of the foreground process
job** jobs;// array of jobs
int lastStoppedId = 0;//index (in jobs) of last job with status stopped

//**************************************************************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(job** jobs, char* lineSize, char* cmdString)
{
	updateCmdHistory(lineSize);
	updateJobs(jobs);
	//update jobs - will remove any finished jobs
	char* cmd; 
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE];
	char* delimiters = " \t\n";  
	int i = 0, num_arg = 0;
	bool illegal_cmd = FALSE; // illegal command
    	cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0; 
   	args[0] = cmd;
	for (i=1; i<MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters); 
		if (args[i] != NULL) 
			num_arg++; 
	}
	
	if (!strcmp(cmd, "cd") ) 
	{
		if (num_arg != 1)
			illegal_cmd = TRUE;
		else if (Cd(args[1]))
			perror("smash error: > cd");
	} 
	
	/*************************************************/
	else if (!strcmp(cmd, "pwd")) 
	{
		if (num_arg != 0)
			illegal_cmd = TRUE;
		else if(Pwd(pwd))
			perror("smash error: > pwd");
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg"))
	{
		if (num_arg > 1)
			illegal_cmd = TRUE;
		else if (Fg(args[1], num_arg, jobs))
			perror("smash error: > fg");
	}
	/*************************************************/
	else if (!strcmp(cmd, "mv"))
	{
		if (num_arg != 2)
			illegal_cmd = TRUE;
		else if (Mv(args[1], args[2]))
			perror("smash error: > mv");
	}
	/*************************************************/
	else if (!strcmp(cmd, "history"))
	{
		if (num_arg != 0)
			illegal_cmd = TRUE;
		else if (History())
			perror("smash error: > history");
	}
	/*************************************************/
	else if (!strcmp(cmd, "jobs")) 
	{
		if (num_arg != 0)
			illegal_cmd = TRUE;
		else if (Jobs(jobs))
			perror("smash error: > jobs");
	}

	/*************************************************/

	else if (!strcmp(cmd, "kill"))
	{
		if (num_arg != 2)
			illegal_cmd = TRUE;
		else if (Kill(jobs, args[1], args[2]))
			perror("smash error: > kill");
	}

	/*************************************************/
	else if (!strcmp(cmd, "showpid")) 
	{
		if (num_arg != 0)
			illegal_cmd = TRUE;
		else if(Showpid())
			perror("smash error: > showpid");
	}
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
		if (num_arg != 0 && num_arg != 1)
			illegal_cmd = TRUE;
		else if (Bg(jobs, args[1], num_arg))
			perror("smash error: > bg");
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
		if (num_arg == 0) {
			if(quit(jobs, FALSE)) perror("smash error: > quit");
		}
		else if (num_arg == 1 && !strcmp(args[1], "kill")) {
			if(quit(jobs, TRUE)) perror("smash error: > quit");
		}
		else illegal_cmd = TRUE;
	} 
	/*************************************************/
	else // external command
	{
		
		int id;
		if (!strcmp(args[num_arg], "&") || args[num_arg][strlen(args[num_arg]) - 1] == '&') {
			if (!strcmp(args[num_arg], "&")) args[num_arg] = NULL;
			else args[num_arg][strlen(args[num_arg]) - 1] = '\0';
			id = ExeExternal(args, cmdString, TRUE);
			if(id == -1) perror("smash error: > external command");
			else add_job(jobs, args[0], id, RUNNING);
		}
		else {
			id = ExeExternal(args, cmdString, FALSE);
			if (id == -1) perror("smash error: > external command");
			else add_job(jobs, args[0], id, RUNNING);
		}
	 	return 0;
	}
	if (illegal_cmd == TRUE)
	{
		printf("smash error: > \"%s\"\n", cmdString);
		return 1;
	}
    return 0;
}

//**************************************************************************************
// function name: updateCmdHistory
// Description: updated cmdHist with latest command and updated cmdHistCount +1 (Cyclic - will go to 0 when reach's 50);
// Parameters: command string (enite line input)
// Returns: void
//**************************************************************************************
void updateCmdHistory(char* lineSize) {
	if (cmdHistCount == 50) cmdHistCount = 0;
	strcpy(cmdHist[cmdHistCount++], lineSize);
}

//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
int ExeExternal(char *args[MAX_ARG], char* cmdString, bool BG)
{
	int pID;
	int status = 999;
    switch(pID = fork()) 
	{
    		case -1: 
				return -1;
        	case 0 :
                	// Child Process
               		setpgrp();
					if (execvp(args[0], args) == -1) {
						perror("smash error: > external command");
						exit(-1);
					}
					exit(0);
			default:
				if (!BG) {
					FG_pID = pID;
					strcpy(L_Fg_Cmd, cmdString);
					if (waitpid(pID, &status, WUNTRACED) == -1 && (!WIFSTOPPED(status) && !WIFSIGNALED(status)))
						return -1;
					strcpy(L_Fg_Cmd, "");
					FG_pID = -1;
				}
				return pID;
	}
}

//**************************************************************************************
// function name: ExeComp
// Description: executes complicated command
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
int ExeComp(char* lineSize)
{
	if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">")) || (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || (strstr(lineSize, "|&"))) {
		updateCmdHistory(lineSize);
		return 0;
	}
	return -1;
}

//**************************************************************************************
// function name: pwd
// Description: prints present directory
// Parameters: -
// Returns: 0- succesful -1- otherwise
//**************************************************************************************
int Pwd(char pwd[MAX_LINE_SIZE]) {
	if (getcwd(pwd, MAX_LINE_SIZE) == NULL) return -1;	
	printf("%s\n", pwd);
	return 0;
}

//**************************************************************************************
// function name: cd
// Description: changes the current working directory
// Parameters: name of directory 
// Returns: 0- succesful -1- otherwise
//**************************************************************************************
int Cd(char* path) {
	bool prev = !strcmp(path, "-");
	bool noPath = FALSE;
	path = prev ? ".." : path;
	if (chdir(path) == -1) noPath = TRUE;
	if (!noPath && prev) {		
		char pwd[MAX_LINE_SIZE];
		if (Pwd(pwd) == -1) return -1;
	}
	if (noPath) printf("smash error: > %s - path not found\n", path);
	return 0;
}

//**************************************************************************************
// function name: Showpid
// Description: prints smashs' PID
// Parameters: none
// Returns: 0 - no failure
//**************************************************************************************
int Showpid() {
	printf("smash pid is %d\n", getpid());
	return 0;
}

//**************************************************************************************
// function name: Fg
// Description: brings process from background to frontground by id (no id meant last process that moved to background)
// Parameters: job number, number of arguments, jobs array
// Returns: 0 for succes/self care errors, -1 for syscall errors.
//*************************************************************************************
int Fg(char* commandNum, int num_arg, job** jobs) {
	//remove from jobs
	int cmdNum;
	if (num_arg != 0) {
		cmdNum = atoi(commandNum);
	}
	else cmdNum = lastJob;
	if (cmdNum <= 0 || cmdNum > 100) {
		printf("smash error: > fg command number %d is out of jobs bounds\n", cmdNum);
		return 0;
	}
	
	if (jobs[cmdNum] == NULL) {
		printf("smash error: > fg command number %d does not exist\n", cmdNum);
		return 0;
	}
	int status = 999;
	int curPid = jobs[cmdNum]->pid;
	if (kill_handler(curPid, SIGCONT) == -1) return -1;
	else {
		FG_pID = curPid;
		strcpy(L_Fg_Cmd, jobs[cmdNum]->name);
		printf("%s\n", L_Fg_Cmd);
		if (waitpid(curPid, &status, WUNTRACED) == -1 && (!WIFSTOPPED(status) && !WIFSIGNALED(status)))
			return -1;
		strcpy(L_Fg_Cmd, "");
		FG_pID = -1;
		return 0;
	}
}

//**************************************************************************************
// function name: Bg
// Description: continue to run a stopped background process
// Parameters: job number, number of arguments, jobs array.
// Returns: 0 for succes/self care errors, -1 for syscall errors.
//*************************************************************************************
int Bg(job** jobs, char* jobId, int num_arg) {
	int cmdNum;
	if (num_arg == 0) cmdNum = lastStoppedId;
	else cmdNum = atoi(jobId);
	if (jobs[cmdNum] != NULL) {			//command exists in jobs array
		if (jobs[cmdNum]->sts != RUNNING) {			//command is not running
			if (kill_handler(jobs[cmdNum]->pid, SIGCONT) == -1) return -1;
			else {
				printf("%s\n", jobs[cmdNum]->name);
				return 0;
			}
		}
		else {
			printf("smash error: > bg command number %d already running in background\n", cmdNum);
			return 0;
		}
	}
	else {
		printf("smash error: > bg command number %d does not exist\n", cmdNum);
		return 0;
	}
	return 0;
}

//**************************************************************************************
// function name: Kill
// Description: send signal, by signum to a job.
// Parameters: jobs array, signal macro, job number
// Returns: 0 
//*************************************************************************************
int Kill(job** jobs, char* flagSignum, char* job_id) {
	if (jobs [atoi(job_id)]== NULL) {
		printf("smash error: > kill %d - job does not exist\n", atoi(job_id));
		return 0;
	}
	char* signum =  strtok(flagSignum, "-");
	if (kill_handler(jobs[atoi(job_id)]->pid, atoi(signum)) == -1) {
		printf("smash error: > kill %d - cannot send signal\n", atoi(job_id));
		return 0;
	}
	return 0;
}

//**************************************************************************************
// function name: Jobs
// Description: lists all current listed jobs and their characteristics
// Parameters: jobs - pointer to jobs
// Returns: 0 - success, -1 - failure
//**************************************************************************************
int Jobs(job** jobs) {
	time_t curr_time;
	for (int i = 0; i <= lastJob; i++) {
		if (jobs[i] != NULL){
			if(time(&curr_time) == -1) return -1;
			printf("[%d] %s : %d %lu secs", i, jobs[i]->name, jobs[i]->pid, (curr_time - *(jobs[i]->startTime)));
			if(jobs[i]->sts == STOPPED)
				printf(" (STOPPED)");
			printf("\n");
		}
	}
	return 0;
}

//**************************************************************************************
// function name: add_job
// Description: adds job with given parameters and updated last job +1
// Parameters: jobs - pointer to jobs, cmd - name of job to be added, pid - pid of job to be added, sts - status of job to be added
// Returns: void
//**************************************************************************************
void add_job(job** jobs, char* cmd, int pid, status sts){
	lastJob = lastJob + 1;
	if ((jobs[lastJob] = (job*)malloc(sizeof(job))) == NULL) {
		printf("smash error: > adding job: memory allocation\n");
		return;
	}
	jobs[lastJob]->id = lastJob;
	if ((jobs[lastJob]->name = (char*)malloc(sizeof(char) * strlen(cmd))) == NULL) {
		printf("smash error: > adding job: memory allocation\n");
		free(jobs[lastJob]); 
		return;
	}
	strcpy(jobs[lastJob]->name, cmd);
	jobs[lastJob]->pid = pid;
	if((jobs[lastJob]->startTime = (time_t*)malloc(sizeof(time_t))) == NULL){
		printf("smash error: > adding job: memory allocation\n");
		free(jobs[lastJob]); 
		free(jobs[lastJob]->name);
		return;
	}
	if (time(jobs[lastJob]->startTime) == -1) {
		perror("smash error: > adding job time");
		return;
	}
	jobs[lastJob]->sts = sts;
}

//**************************************************************************************
// function name: rem_job
// Description: removes job of given id in jobs, and sets new lastJob
// Parameters: jobs - pointer to jobs, id - index of job to be removed
// Returns: void
//**************************************************************************************
void rem_job(job** jobs, int id){
	if (jobs[id] == NULL) return;
	free(jobs[id]->name);
	free(jobs[id]->startTime);
	free(jobs[id]); 
	jobs[id] = NULL;
	if (id == lastJob) {
		int i = lastJob - 1;
		for (; i > 0; i--) 
			if (jobs[i] != NULL) break;
		lastJob = i;
	}
}

//**************************************************************************************
// function name: updateJobs
// Description: updated status of jobs, deletes them if they dont exist anymore (terminated/exit...)
// Parameters: pointer to jobs
// Returns: void
//**************************************************************************************
void updateJobs(job** jobs) {
	int status;
	for (int i = 0; i <= lastJob; i++) {
		if (jobs[i] != NULL && waitpid(jobs[i]->pid, &status, WNOHANG | WUNTRACED | WCONTINUED)) {
			if (WIFEXITED(status) || WIFSIGNALED(status))
				rem_job(jobs, i);
			if (WIFCONTINUED(status)) {
				jobs[i]->sts = RUNNING;
			}
			if (WIFSTOPPED(status)) {
				jobs[i]->sts = STOPPED;
				lastStoppedId = i;
			}
		}	
	}
}

//**************************************************************************************
// function name: Mv
// Description: changes name of file
// Parameters: old_name - name of file to be changed, new_name - name to be changed to
// Returns: 0 - success (doesnt fail)
//**************************************************************************************
int Mv(char* old_name, char* new_name) {
	if (rename(old_name, new_name)) return -1;
	printf("%s has been renamed to %s\n", old_name, new_name);
	return 0;
}

//**************************************************************************************
// function name: History
// Description: prints last 50 commands that were inputted to smash
// Parameters: null
// Returns: 0 - success (doesnt fail)
//**************************************************************************************
int History() {
	int i = cmdHistCount-2;
	while (i != cmdHistCount-1 && strcmp(cmdHist[i], "")) {
		printf("%s", cmdHist[i--]);
		if (i == -1) i = 49;
	}
	return 0;
}

//**************************************************************************************
// function name: quit
// Description: quits smash, if fkill is true then also terminates/kills all child processes
// Parameters: pointer to jobs, fKill - if true terminates/kills all child processes
// Returns: 0 - success, -1 - failure
//**************************************************************************************
int quit(job** jobs, bool fKill) {
	if(fKill) {
		job *curJob = jobs[1];
		for (int i = 1; i <= lastJob; i++) {
			curJob = jobs[i];
			if (curJob == NULL) continue;
			printf("[%d] %s - Sending SIGTERM... ", i, curJob->name);
			if (kill_handler(curJob->pid, SIGTERM) == -1) return -1;
			time_t startTime, endTime;
			if (time(&startTime) == -1) return -1;
			while (endTime - startTime < 5) {//wait 5 seconds
				if (time(&endTime) == -1) return -1;
			}
			if (kill_handler(0, curJob->pid)) {
				printf("(5 sec passed) Sending SIGKILL... ");
				if (kill_handler(curJob->pid, SIGKILL) == -1) return -1;
			}
			printf("Done \n");
		}
	}
	return (kill_handler(getpid(), SIGTERM) == -1) ? -1 : 0;
}

