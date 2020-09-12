/* 046267 Computer Architecture - Winter 2019/20 - HW #3 */
/* Implementation (skeleton)  for the dataflow statistics calculator */

#include "dflow_calc.h"
#include <vector>
#define ENTRY -1

using namespace std;

//node in graph
class node {
public: //methods

    //parameterless c'tor
    node() : id(-1), opcode(-1), weight(0), dep1(-1), dep2(-1) {} 

    //c'tor
    node(int _id, int _opcode, int _weight, int _dep1, int _dep2) :
        id(_id), opcode(_opcode), weight(_weight), dep1(_dep1), dep2(_dep2) {}

    //default d'tor
    ~node() {} 

public://fields
    int id;
    int opcode;
    int weight;
    //dependencies of command
    int dep1;
    int dep2;
};


class graph {
public:
    //c'tor
    graph(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts) : size(numOfInsts) {
        nodesArr = new node[size];
        int dep1, dep2;
        vector<int> depHist(numOfInsts, 0);//track dependencies so we know which are connnected to exit
        for (int i = numOfInsts-1; i >= 0; i--) { //init all nodes
            dep1 = getDep(i, progTrace[i].src1Idx, progTrace);
            dep2 = getDep(i, progTrace[i].src2Idx, progTrace);
            nodesArr[i] = node(i, progTrace[i].opcode, opsLatency[progTrace[i].opcode], dep1, dep2);
            if (dep1 != ENTRY) {
                depHist[dep1]++;
            }
            if (dep2 != ENTRY) {
                depHist[dep2]++;
            }
        }
        //init exit dependencies
        for (int i = numOfInsts-1; i >= 0; i--) {//add dependencies to exit
            if (depHist[i] == 0) {
                exitNode.push_back(i);
            }
        }
    }

    //d'tor
    ~graph() {
        delete[] nodesArr;
    };
    ////////////////////////////////////////////////////////////////////
    //description: gets the Id's of the instructions upon which the current instruction's argument register is dependent on.
    //parameters: startId - the id of the instrution
    //            depReg - the register which is given as argument to the instruction
    //            progTrace - the trace of the program's instructions.
    // return: the Id of the instruction. -1 if isn't dependant on any former instructions.
    ////////////////////////////////////////////////////////////////////
    int getDep(int startId, int depReg, const InstInfo progTrace[]) {
        for (int i = startId-1; i >= 0; i--) { //we start from startId-1 because we want to look for the former instruction, and not the current (which will always be true)
            if (depReg == progTrace[i].dstIdx)
                return i;
        }
        return ENTRY;
    }

    ////////////////////////////////////////////////////////////////////
    //description: get depth of node in given index
    //parameters: theInst - index of node
    //return: return int depth
    ////////////////////////////////////////////////////////////////////
    int getDepth(unsigned int theInst) {
        int longestRoad1 = 0;
        int longestRoad2 = 0;
        getDepth_rec(nodesArr[theInst].dep1, 0, &longestRoad1);
        getDepth_rec(nodesArr[theInst].dep2, 0, &longestRoad2);
        return (longestRoad1 > longestRoad2) ? longestRoad1 : longestRoad2;
    }

    ////////////////////////////////////////////////////////////////////
    //description: recursive function to scan all possible roads to entry from given node and find the depth of it
    //parameters: curNode - index of current node in recursion, curWeight - weight of current node, longestRoad - longestRoad thus far in recutsion
    //return: return int longest road from given node
    ////////////////////////////////////////////////////////////////////
    void getDepth_rec(int curNode,int curWeight, int* longestRoad) {
        if (curNode == ENTRY) {
            *longestRoad = (curWeight > *longestRoad ? curWeight : *longestRoad);
            return;
        }
        getDepth_rec(nodesArr[curNode].dep1, curWeight + nodesArr[curNode].weight ,longestRoad);
        getDepth_rec(nodesArr[curNode].dep2, curWeight + nodesArr[curNode].weight ,longestRoad);
	}

    //members
    node* nodesArr;
    vector<int> exitNode;//dependencies of exit
    unsigned int size;//size of graph
};

ProgCtx analyzeProg(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts) {
    graph* pGraph;
    if (numOfInsts == 0) {
        return PROG_CTX_NULL;
    }
    try {
        pGraph = new graph(opsLatency, progTrace, numOfInsts);
    }
    catch(...){
        pGraph = PROG_CTX_NULL;
	}
    return pGraph;
}

void freeProgCtx(ProgCtx ctx) {
    delete (graph*)ctx;
}

//return longest track in graph from theInst (in time)
int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    if (ctx == NULL || (theInst > ((graph*)ctx)->size)) {
        return -1;
    }
    return ((graph*)ctx)->getDepth(theInst);
}

//return longest track in graph from exit to entry (in time)
int getProgDepth(ProgCtx ctx) {
    if (ctx == NULL) {
        return -1;
    }
    int largestDepth = 0;
    for (unsigned int i = 0; i < (((graph*)ctx)->exitNode).size(); i++){
        int curDepth = ((graph*)ctx)->nodesArr[((graph*)ctx)->exitNode[i]].weight + getInstDepth(ctx, (((graph*)ctx)->exitNode[i]));
        largestDepth = (curDepth > largestDepth) ? curDepth : largestDepth;
	}
    return largestDepth;
}

//return dependecies of theInst
int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    if (ctx == NULL || (theInst > ((graph*)ctx)->size)) {
        return -1;
    }
    *src1DepInst = (((graph*)ctx)->nodesArr[theInst]).dep1;
    *src2DepInst = (((graph*)ctx)->nodesArr[theInst]).dep2;
    return 0;
}



