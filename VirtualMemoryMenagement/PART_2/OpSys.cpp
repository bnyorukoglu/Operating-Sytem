/*
 * OpSys.cpp
 *
 *  Created on: Jul 9, 2020
 *      Author: bengi
 */

#include "OpSys.h"
#include "AbstractProcess.h"
#include "pthread.h"
#include <semaphore.h>
#include "PageDefinitions.h"
#include "TestProcess.h"
#include <iostream>

OpSys::OpSys() {

}
void OpSys::registerProcess(AbstractProcess *p1) {
	this->processes.push_back(p1);
}

AbstractProcess* OpSys::getProcessByName(char *pName) {

	for (int i = 0; i < processes.size(); i++) {
		if (strcmp(processes[i]->getPname(), pName) == 0)
			return processes[i];
	}
	return NULL;
}

void OpSys::checkAccessCount() {

	this->accessCount = (this->accessCount + 1) % this->verboseThreshold;
	if (accessCount == 0) {
		P_PageTable table = this->memory->getPageTable();
		printf(
				"Page No - Frame No - Present - Dirty - Protection - Reference,\n");
		for (int i = 0; i < this->memory->getNumVirtual(); i++) {
			P_PageTableEntry entry = &table->pages[i];
			printf("%4d %8d %12d %8d %8d %12d\n", i, entry->frame_Number,
					entry->present_absent, entry->dirty_bit, entry->protection,
					entry->referenced);
		}
	}

}
static sem_t kernel_lock;
void OpSys::set(unsigned int index, int value, AbstractProcess *process) {

	process->pagingStatistics[Write] += 1;

	if (sem_trywait(&kernel_lock) == 0) {
		//printf("semaphore acquired\n");
	} else {

		sem_wait(&kernel_lock);
	}
	this->memory->set(index, value, process->getPname());
	sem_post(&kernel_lock);
	replacementPolicy->updatePageTables(this->memory->getPageTable(),
			memory->getPageTableAddress(index));
	checkAccessCount();

}
int OpSys::get(unsigned int index, AbstractProcess *process) {

	process->pagingStatistics[Read] += 1;
	if (sem_trywait(&kernel_lock) == 0) {
		//printf("semaphore acquired\n");
	} else {

		sem_wait(&kernel_lock);
	}
	int value = this->memory->get(index, process->getPname());
	sem_post(&kernel_lock);
	checkAccessCount();
	replacementPolicy->updatePageTables(this->memory->getPageTable(),
			memory->getPageTableAddress(index));

	return value;
}

void OpSys::setMemory(VirtualMemory *memory) {
	this->memory = memory;
	this->memory->setOpsys(this);
	this->verboseThreshold = this->memory->getPageTablePrintInt();

}
void OpSys::onPageFault(unsigned int index, char *tName) {
	getProcessByName(tName)->pagingStatistics[PageMiss] += 1;
	getProcessByName(tName)->pagingStatistics[PageRead] += 1;

}
void OpSys::setReplacementPolicy(ReplacementPolicy *policy) {
	this->replacementPolicy = policy;
}
int OpSys::getFreePage(P_PageTable table, char *tName) {

	getProcessByName(tName)->pagingStatistics[PageReplace] += 1;
	return this->replacementPolicy->getFreePage(table);
}
void OpSys::pageWrite(P_PageTable table, char *tName) {
	getProcessByName(tName)->pagingStatistics[PageWrite] += 1;

}

void* dispacthProcess(void *pr1) {
	AbstractProcess *prc = (AbstractProcess*) pr1;
	prc->sort(NULL);
	return NULL;
}
void OpSys::dispacthProcesses() {
	if (sem_init(&kernel_lock, 0, 1) != 0) {
		printf("\n mutex init has failed\n");
		return;
	}

	int length = this->processes.size();
	pthread_t threads[length];
	for (int i = 0; i < length; i++)
		pthread_create(&threads[i], NULL, dispacthProcess, this->processes[i]);

	for (int i = 0; i < length; i++) {

		pthread_join(threads[i], NULL);

	}
	sem_destroy(&kernel_lock);
	this->dispachTestProcess();
	this->printPagingStatistics();

}
void OpSys::printPagingStatistics() {

	int length = this->processes.size();

	printf(
			"\nProcess Name - Read - Write - Miss - Replace - Page Read - Page Write \n");
	for (int i = 0; i < length; i++) {
		AbstractProcess *prt = this->processes[i];
		printf("%s\t", prt->getPname());

		printf("%2d", prt->pagingStatistics[Read]);
		printf("%5d", prt->pagingStatistics[Write]);
		printf("%8d", prt->pagingStatistics[PageMiss]);
		printf("%10d", prt->pagingStatistics[PageReplace]);
		printf("%10d", prt->pagingStatistics[PageRead]);
		printf("%12d", prt->pagingStatistics[PageWrite]);

		printf("\n");
	}

}

int OpSys::getMemorySize() {

	return this->memory->getNumVirtual() * this->memory->getFrameSize();

}
void OpSys::initializeMemory() {
	this->memory->initialize();

}
void OpSys::dispachTestProcess() {

	AbstractProcess *prt = new TestProcess(this, 0, getMemorySize());

	prt->sort(NULL);

}

