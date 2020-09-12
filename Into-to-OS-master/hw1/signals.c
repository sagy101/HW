// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.h"
const char* sigs_name[] = //array for signal handling and printing
{
	"",				//0
	"SIGHUP",		//1
	"SIGINT",		//2
	"SIGQUIT",		//3
	"SIGILL",		//4
	"SIGTRAP",		//5
	"SIGABRT",		//6
	"SIGBUS",		//7
	"SIGFPE",		//8
	"SIGKILL",		//9
	"SIGUSR1",		//10
	"SIGSEGV",		//11
	"SIGUSR2",		//12
	"SIGPIPE",		//13
	"SIGALRM",		//14
	"SIGTERM",		//15
	"SIGSTKFLT",	//16
	"SIGCHLD",		//17
	"SIGCONT",		//18
	"SIGSTOP",		//19 
	"SIGTSTP",		//20
	"SIGTTIN",		//21
	"SIGTTOU",		//22
	"SIGURG",		//23
	"SIGXCPU",		//24
	"SIGXFSZ",		//25
	"SIGVTALRM",	//26
	"SIGPROF",		//27
	"SIGWINCH",		//28
	"SIGIO",		//29
	"SIGPWR",		//30
	"SIGSYS",		//31
	"SIGRTMIN",		//34
};

int FG_pID;//pid of the foreground process
job** jobs;// array of jobs
char* L_Fg_Cmd;//command (string) of the foreground process


//**************************************************************************************
// function name: catch_stop
// Description: handling ctrl z signal
// Parameters: signal number
// Returns: void
//*************************************************************************************
void catch_stop(int sig_num){
	sigset_t mask_set;
	sigset_t old_set;
	sigfillset(&mask_set);
	sigprocmask(SIG_SETMASK, &mask_set, &old_set);
	if (FG_pID > 0 && FG_pID != getpid()) {
		if (kill_handler(FG_pID, sig_num) == -1) {
			perror("smash erorr: > stop\n");
			return;
		}
	}
	sigprocmask(SIG_SETMASK, &old_set, &mask_set);
}

//**************************************************************************************
// function name: catch_init
// Description: handling ctrl c signal
// Parameters: signal number
// Returns: void
//*************************************************************************************
void catch_init(int sig_num){
	sigset_t mask_set;
	sigset_t old_set;
	sigfillset(&mask_set);
	sigprocmask(SIG_SETMASK, &mask_set, &old_set);
	if (FG_pID > 0 && FG_pID != getpid()) {
		if (kill_handler(FG_pID, sig_num) == -1) {
			perror("smash erorr: > terminate\n");
			return;
		}
	}
	sigprocmask(SIG_SETMASK, &old_set, &mask_set);
}

//**************************************************************************************
// function name: kill_handler
// Description: handling signals, prints message accordingly
// Parameters: PID, signal number
// Returns: -1 if signal sending failed, 0 if success
//*************************************************************************************
int kill_handler(int pid, int sig_num) {
	if (kill(pid, sig_num) == -1) return -1;
	else {
		printf("signal %s was sent to pid %d\n", sigs_name[sig_num], pid);
		return 0;
	}
}