/*
 * VirtualMemory.h
 *
 *  Created on: Jul 4, 2020
 *      Author: bengi
 */

#include "malloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <queue>
#include "VirtualMemory.h"
#include "PageDefinitions.h"
#include "OpSys.h"
#include "string.h"

using namespace std;

void VirtualMemory::onPageFault(unsigned int index,char *tName) {

	int nextPageIndex = 0;

	int tableAddress = getPageTableAddress(index);

	int evicted = -1;
	int memAddress = 0;
	int virtualAddress = 0;
	this->opsys->onPageFault(index,tName);
	if (pageTable->current_frame_count < pageTable->max_frame_count) {
		nextPageIndex = tableAddress; // pageTable->current_frame_count;
		pageTable->referencePages->push_back(nextPageIndex);
		memAddress = nextPageIndex * this->frameSize;
		virtualAddress = nextPageIndex * this->frameSize;

		pageTable->pages[nextPageIndex].present_absent = 1;
		pageTable->pages[nextPageIndex].frame_Number = nextPageIndex;
		pageTable->current_frame_count++;

	} else {

		nextPageIndex = this->opsys->getFreePage(this->pageTable,tName);

		pageTable->referencePages->push_back(tableAddress);
		pageTable->pages[nextPageIndex].present_absent = 0;
		pageTable->pages[tableAddress].present_absent = 1;

		evicted = pageTable->pages[nextPageIndex].frame_Number;

		pageTable->pages[tableAddress].frame_Number = evicted;
		memAddress = evicted * this->frameSize;
		virtualAddress = tableAddress * this->frameSize;
		/*printf("\n Next_Page %d  Evicted %d  Table_adress %d %d %d ",
				nextPageIndex, evicted, virtualAddress, index,
				pageTable->referencePages->size());
		for (int i = 0; i < frameSize; i++)
			printf("%d-", *(this->physicalMem + memAddress + i));
		printf("|");
		for (int i = 0; i < frameSize; i++)
			printf("%d-", *(this->virtualMem + virtualAddress + i));*/
	}

	if (pageTable->pages[nextPageIndex].dirty_bit != 0) {

		int *temp = (int*) malloc(sizeof(int) * this->frameSize);
		memcpy(temp, this->virtualMem + virtualAddress,
				sizeof(int) * this->frameSize);
		memcpy(this->virtualMem + nextPageIndex * this->frameSize,
				this->physicalMem + memAddress, sizeof(int) * this->frameSize);
		memcpy(this->physicalMem + memAddress, temp,
				sizeof(int) * this->frameSize);
		pageTable->pages[nextPageIndex].dirty_bit = 0;
		this->opsys->pageWrite(this->pageTable,tName);

	} else {
		memcpy(this->physicalMem + memAddress,
				this->virtualMem + virtualAddress,
				sizeof(int) * this->frameSize);
		pageTable->pages[nextPageIndex].dirty_bit = 0;
	}

}
