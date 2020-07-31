/*
 * OpSys.h
 *
 *  Created on: Jul 9, 2020
 *      Author: bengi
 */

#ifndef OPSYS_H_
#define OPSYS_H_
#include "AbstractProcessFWD.h"
#include "VirtualMemory.h"
#include "ReplacementPolicy.h"
#include <vector>
#include <pthread.h>
#include <semaphore.h>
using namespace std;
class OpSys {
public:
	OpSys();
	void registerProcess(AbstractProcess *process);

	AbstractProcess* getProcessByName(char *pName);
	void set(unsigned int index, int value, AbstractProcess *process);
	int get(unsigned int index, AbstractProcess *process);

	void setMemory(VirtualMemory *memory);
	void onPageFault(unsigned int index, char *tName);
	int getFreePage(P_PageTable table, char *tName);
	void pageWrite(P_PageTable table, char *tName);

	void setReplacementPolicy(ReplacementPolicy *policy);
	void printPagingStatistics();
	void dispacthProcesses();
	void dispachTestProcess();
	int getMemorySize();
	void initializeMemory();
private:
	vector<AbstractProcess*> processes;
	VirtualMemory *memory;
	ReplacementPolicy *replacementPolicy;
	int accessCount = 0;
	int verboseThreshold =50;


	void checkAccessCount();
};
#endif /* OPSYS_H_ */
