#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <pthread.h>
#include <iostream>
#include "AbstractProcess.h"
#include "BubbleProcess.h"
#include "IndexSortProcess.h"
#include "MergeProcess.h"
#include "QuickSortProcess.h"
#include "FifoReplacement.h"
#include "NRUReplacement.h"
#include "SecondChanceReplacement.h"
#include "LRUReplacement.h"
#include "OpSys.h"

#include "VirtualMemory.h"
#include <string>
int *mainArr;

struct S {
	int base;
	int limit;
};

VirtualMemory* createVirtualMemory(int numVirtual, int numPhysical,
		int frameSize, int verboseCount) {
	VirtualMemory *vm = new VirtualMemory(numVirtual, numPhysical, frameSize, 0,
			0, "abc.dat", verboseCount);

	return vm;
}

int main(int argc, char **argv) {

	if (argc != 8) {
		printf("Wrong number of parameters ! \n ");
		exit(0);
	}
	ReplacementPolicy *replacement = NULL;

	int frameSize = atoi(argv[1]);
	int numPhysical = atoi(argv[2]);
	int numVirtual = atoi(argv[3]);
	string pageReplacement = string(argv[4]);
	string allocPolicy = string(argv[5]);
	int pageTablePrintInt = atoi(argv[6]);
	string fileName = string(argv[7]);

	OpSys *os = new OpSys();
	VirtualMemory *vm = createVirtualMemory(numVirtual, numPhysical, frameSize,
			pageTablePrintInt);
	vm->initialize();
	os->setMemory(vm);

	if (strcmp("FIFO", argv[4]) == 0)
		replacement = new FifoReplacement();

	else if (strcmp("NRU", argv[4]) == 0)
		replacement = new NRUReplacement();

	else if (strcmp("SC", argv[4]) == 0)
		replacement = new SecondChanceReplacement();

	else if (strcmp("LRU", argv[4]) == 0)
		replacement = new LRUReplacement();

	else if (strcmp("WSClock", argv[4]) == 0) {

		printf("Not Implemented. Falling Back To LRU \n ");
		replacement = new LRUReplacement();

	}

	else {
		printf("Wrong replacement algorithm name ! \n ");
		exit(0);
	}
	int size = os->getMemorySize() / 4;
	int start = 0;
	AbstractProcess *process1 = new BubbleProcess(os, start, size);
	start += size;
	AbstractProcess *process2 = new QuickSortProcess(os, start, start + size);
	start += size;
	AbstractProcess *process3 = new MergeProcess(os, start, start + size);
	start += size;
	AbstractProcess *process4 = new IndexSortProcess(os, start, start + size);

	os->setReplacementPolicy(replacement);

	os->dispacthProcesses();

	return 0;
}

