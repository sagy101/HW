/* 046267 Computer Architecture - Winter 2019/20 - HW #4 */

#include "core_api.h"
#include "sim_api.h"
#include <vector>
#include <stdio.h>
#include <iostream>

using namespace std;

#define NO_THREAD_ALIVE -1
#define NO_THREAD_FREE -2
//#define DBG

class thread {
public:
	//methods

	thread() : pc(-1), waiting(-1), alive(false), id(-1) { };//parameterless c'tor
	thread(int _id) : pc(0), waiting(0), alive(true), id(_id) {//const'
		for (int i = 0; i < REGS_COUNT; i++) {//init registers
			ctx.reg[i] = 0;
		}
	};

	~thread() {};//deconst'


	// execute next instruction on thread
	void execute() {
		Instruction inst;
        int addr;
		SIM_MemInstRead(pc, &inst, id);
		switch (inst.opcode) {
		case CMD_NOP:
#if defined DBG
			printf("CMD_NOP on thread: %d\n", id); //DBG
#endif
			break;
		case CMD_ADD:
#if defined DBG
			printf("CMD_ADD on thread: %d\n", id); // DBG
#endif
			ctx.reg[inst.dst_index] = ctx.reg[inst.src1_index] + ctx.reg[inst.src2_index_imm];
			break;
		case CMD_SUB:
#if defined DBG
			printf("CMD_SUB on thread: %d\n", id); //DBG
#endif
			ctx.reg[inst.dst_index] = ctx.reg[inst.src1_index] - ctx.reg[inst.src2_index_imm];
			break;
		case CMD_ADDI:
#if defined DBG
			printf("CMD_ADDI on thread: %d\n", id); //DBG
#endif
			ctx.reg[inst.dst_index] = ctx.reg[inst.src1_index] + inst.src2_index_imm;
			break;
		case CMD_SUBI:
#if defined DBG
			printf("CMD_SUBI on thread: %d\n", id); //DBG
#endif
			ctx.reg[inst.dst_index] = ctx.reg[inst.src1_index] - inst.src2_index_imm;
			break;
		case CMD_LOAD:
#if defined DBG
			printf("CMD_LOAD on thread: %d\n", id); //DBG
#endif
			addr = ctx.reg[inst.src1_index] + (inst.isSrc2Imm ? inst.src2_index_imm : ctx.reg[inst.src2_index_imm]);
			SIM_MemDataRead(addr, &(ctx.reg[inst.dst_index]));
			waiting = SIM_GetLoadLat();
			break;
		case CMD_STORE:
#if defined DBG
			printf("CMD_STORE on thread: %d\n", id); //DBG
#endif
			addr = ctx.reg[inst.dst_index] + (inst.isSrc2Imm ? inst.src2_index_imm : ctx.reg[inst.src2_index_imm]);
			SIM_MemDataWrite(addr, ctx.reg[inst.src1_index]);
			waiting = SIM_GetStoreLat();
			break;
		case CMD_HALT:
#if defined DBG
			printf("CMD_HALT on thread: %d\n", id); //DBG
#endif
			alive = false;
			break;
		}
		waiting++;//1 cycle of execute it self
		pc++;//go to next instruction
	}

	//lower waiting - cycle has passed
	void threadNextCycle() {
		if (alive && waiting > 0) {
			waiting--;
		}
	}

	//fields
	int id;// thread number
	int pc;// current pc
	tcontext ctx;// thread registers
	int waiting;// thread wait time for memory access or execution
	bool alive;// is thread alive?
};

class CORE {
public:
	//methods

	//constuctor
	CORE(int _numOfThreads): cycle(0), instCnt(0), curThread(0), numOfThreads(_numOfThreads) {
		threads.resize(numOfThreads);
		for (int i = 0; i < numOfThreads; i++) {//init threads
			threads[i] = thread(i);
		}
	};

	//deconstuctor
	~CORE() { };

	virtual void run() = 0;//run routine of core

	virtual int getNextThread(){ };//gets next thread according to core logic (fine grained or blocked)

	// advance cycle in core
	void nextCycle() {
		for (int i = 0; i < numOfThreads; i++) {
			threads[i].threadNextCycle();
		}
		cycle++;
	}

	//return core CPI
	double getCPI() {
		return (double)cycle / instCnt;
	}

//fields
public:
	//cpi data
	int cycle;// number of cycles passed
	int instCnt;//number of instruction run by all threads so far

	int curThread;// current thead running
	int numOfThreads;// number of thead created
	vector<thread> threads;//vector of all threads on core
};

class CORE_grain : public CORE {// fine grained core
public:
	//constuctor
	CORE_grain(int numOfThreads) : CORE(numOfThreads) { };

	//deconstuctor
	~CORE_grain() { };

	//gets next thread according to fine grained logic
    int getNextThread(){
        int i = curThread+1;
        bool isAlive = false;
        for(int cnt = 0 ; cnt<numOfThreads ;i++,cnt++){
            int curId = i % numOfThreads;
            if (threads[curId].alive){
                isAlive = true;
                if (!threads[curId].waiting) {// return next in line thread that isnt waiting for memory
                    return curId;
                }
            }
        }
        return isAlive ? curThread : NO_THREAD_ALIVE;// return current thread if all thread are waiting or no threads are alive
    };

	//run routine of core
	void run() {
		//idle = false;
        while (curThread != NO_THREAD_ALIVE) {
#if defined DBG
            printf("%d| ",cycle); //DBG
#endif
            if (!threads[curThread].waiting) {
				//if(idle){
				//	curThread = getNextThread();
				//}
				//idle = false;
                threads[curThread].execute();
                instCnt++;
            }
            else{
				//idle = true;
#if defined DBG
                printf("idle on thread: %d\n", curThread);
#endif
            } //DBG
            nextCycle();
            curThread = getNextThread();
        }
	};
	//bool idle;
};

class CORE_blocked : public CORE {
public:
	//constuctor
	CORE_blocked(int numOfThreads) : CORE(numOfThreads) { };

	//deconstuctor
	~CORE_blocked() { };

	//gets next thread according to fine grained logic
    int getNextThread(){
        int i = curThread;
        bool isAlive = false;
        for(int cnt = 0 ; cnt<numOfThreads ;i++,cnt++){
            int curId = i % numOfThreads;
            if (threads[curId].alive){
                isAlive = true;
                if (!threads[curId].waiting) {// return current thread if not waiting or next in line non waiting thread
                    return curId;
                }
            }
        }
        return isAlive ? curThread : NO_THREAD_ALIVE;// return current thread if all thread are waiting or no threads are alive
    };

	//run routine of core
	void run() {
		int tmpThread;
		while (getNextThread() != NO_THREAD_ALIVE) {
#if defined DBG
            printf("%d| ",cycle); //DBG
#endif
            if (!threads[curThread].waiting) {
                threads[curThread].execute();
                instCnt++;
            }
            else{
#if defined DBG
                printf("idle on thread: %d\n", curThread);
#endif
            } //DBG
            nextCycle();
            tmpThread = getNextThread();
			if (tmpThread != NO_THREAD_ALIVE && tmpThread != curThread){ //context switch
#if defined DBG
				printf("%d| ctx switch: %d -> %d\n" , cycle,curThread,tmpThread); //DBG
#endif
				curThread = tmpThread;
				for(int i = 0; i < SIM_GetSwitchCycles(); i++){//context switch cycle plenty
                    nextCycle();
				}
			}
		}
	}
};//end of CORE_blocked



static CORE_blocked *theBlockedCore;
static CORE_grain *theFineCore;

void CORE_BlockedMT() {
	theBlockedCore = new CORE_blocked(SIM_GetThreadsNum());
	theBlockedCore->run();
}

void CORE_FinegrainedMT() {
    theFineCore = new CORE_grain(SIM_GetThreadsNum());
    theFineCore->run();
}

double CORE_BlockedMT_CPI(){
	return theBlockedCore->getCPI();
}

double CORE_FinegrainedMT_CPI(){
    return theFineCore->getCPI();
}

void CORE_BlockedMT_CTX(tcontext* context, int threadid) {
    for(int i = 0; i < REGS_COUNT; i++) {
        context[threadid].reg[i] = theBlockedCore->threads[threadid].ctx.reg[i];
    }
}

void CORE_FinegrainedMT_CTX(tcontext* context, int threadid) {
    for(int i = 0; i < REGS_COUNT; i++) {
        context[threadid].reg[i] = theFineCore->threads[threadid].ctx.reg[i];
    }
}
