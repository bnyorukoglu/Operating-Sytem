/*
 * VirtualMemory.h
 *
 *  Created on: Jul 4, 2020
 *      Author: bengi
 */

#ifndef VIRTUALMEMORY_H_
#define VIRTUALMEMORY_H_
#include "malloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <queue>
#include <deque>
#include "OSFWD.h"

#include "PageDefinitions.h"
#include "string.h"

using namespace std;

class VirtualMemory {
public:
	VirtualMemory();
	VirtualMemory(int numVirtual, int numPhysical, int frameSize, int algorithm,
			int policy, char *fileName, int pageTablePrintInt) {

		this->numVirtual = (int) pow(2, numVirtual);
		this->numPhysical = (int) pow(2, numPhysical);
		this->frameSize = (int) pow(2, frameSize);
		this->virtualAddressBits = numVirtual;
		this->offsetBits = frameSize;

		this->policy = policy;
		this->algorithm = algorithm;
		//	this->fileName = fileName;
		this->pageTablePrintInt = pageTablePrintInt;
		this->physicalMem = (int*) malloc(
				sizeof(int) * this->numPhysical * this->frameSize);
		this->virtualMem = (int*) malloc(
				sizeof(int) * (this->numVirtual * this->frameSize));
		initializePageTable();
	}

	void set(unsigned int index, int value, char *tName) {
		int address = resolveAddress(index, tName);
		int tableAddress = getPageTableAddress(index);
		physicalMem[address] = value;
		pageTable->pages[tableAddress].dirty_bit = 1;

	}
	int get(unsigned int index, char *tName) {
		int address = resolveAddress(index, tName);
		return physicalMem[address];
	}

	void initialize() {
		long t = 3000;
		srand(t);

		for (int i = 0; i < this->frameSize * this->numVirtual; i++) {
			virtualMem[i] = (rand() % 3000) + 1;
		}
		for (int i = 0; i < this->frameSize * this->numPhysical; i++) {
			physicalMem[i] = 0;
		}

	}
	~VirtualMemory();
	void printMem() {
		for (int i = 0; i < this->frameSize * this->numPhysical; i++) {
			printf("%d- %d\n", i, physicalMem[i]);
		}

	}
	void printVirtMem() {
		for (int i = 0; i < this->frameSize * this->numVirtual; i++) {
			printf("%d- %d\n", i, virtualMem[i]);
		}

	}
	void testGetOperations(int length) {
		for (int i = 0; i < length; i++) {
			printf("Testing %d: %d\n", i, get(i, "Test"));

		}
	}

	void TestOperations() {
		printf("Frame Size is : %d \n", this->frameSize);
		printf("Physical Memory Size is : %d \n", this->numPhysical);
		printf("Virtual Memory  Size is: %d \n", this->numVirtual);

		int incomingAdress = 0;
		printf("Page Table Address for %d is : %d \n", incomingAdress,
				this->getPageTableAddress(incomingAdress));
		printf("Page Offset Address for %d is : %d \n", incomingAdress,
				this->getOffsetAddress(incomingAdress));
		incomingAdress = 12;

		printf("Page Table Address for %d is : %d \n", incomingAdress,
				this->getPageTableAddress(incomingAdress));
		printf("Page Offset Address for %d is : %d \n", incomingAdress,
				this->getOffsetAddress(incomingAdress));
		//get(0, "Fill");
		/*		incomingAdress = 255;

		 printf("Page Table Address for %d is : %d \n", incomingAdress,
		 this->getPageTableAddress(incomingAdress));
		 printf("Page Offset Address for %d is : %d \n", incomingAdress,
		 this->getOffsetAddress(incomingAdress));
		 incomingAdress = 256;

		 printf("Page Table Address for %d is : %d \n", incomingAdress,
		 this->getPageTableAddress(incomingAdress));

		 printf("Page Offset Address for %d is : %d \n", incomingAdress,
		 this->getOffsetAddress(incomingAdress));*/

		return;
	}

	OpSys* getOpsys() {
		return opsys;
	}

	void setOpsys(OpSys *opsys) {
		this->opsys = opsys;
	}

	P_PageTable getPageTable() {
		return this->pageTable;
	}

	int getNumVirtual() const {
		return numVirtual;
	}
	int getNumPhysical() const {
		return numPhysical;
	}
	int getFrameSize() const {
		return frameSize;
	}

	int getPageTablePrintInt() const {
		return pageTablePrintInt;
	}
	int getPageTableAddress(int incomingAdress) {

		for (int i = offsetBits; i > 0; i--) {
			incomingAdress = incomingAdress >> 1;

		}
		return incomingAdress;
	}
	void setRawMemory(int index, int value) {
		initialize();
	}

private:
	P_PageTable pageTable;
	OpSys *opsys;
	char *fileName;
	int pageTablePrintInt;
	int policy;
	int algorithm;
	int numVirtual;
	int numPhysical;
	int frameSize;
	int size;
	int *physicalMem;
	int *virtualMem;
	int virtualAddressBits;
	int offsetBits;

	void initializePageTable() {
		pageTable = (P_PageTable) malloc(sizeof(PageTable));
		pageTable->current_frame_count = 0;
		pageTable->max_frame_count = this->numPhysical;
		pageTable->referencePages = new deque<int>();
		pageTable->pages = (P_PageTableEntry) (malloc(
				sizeof(PageTableEntry) * this->numVirtual));

		P_PageTableEntry pagingTable = pageTable->pages;
		for (int i = 0; i < this->numVirtual; ++i) {
			pagingTable[i].dirty_bit = 0;
			pagingTable[i].reference_bit = 0;
			pagingTable[i].present_absent = 0;
		}

	}
	int resolveAddress(unsigned int index, char *tName) {
		int address = index;
		int tableAddress = getPageTableAddress(index);
		int offSetAddress = getOffsetAddress(index);

		if (pageTable->pages[tableAddress].present_absent == 0) {
			onPageFault(index, tName);

		}
		address = getPhysicalAddress(
				pageTable->pages[tableAddress].frame_Number, offSetAddress);

		return address;
	}
	void onPageFault(unsigned int index, char *tName);

	int getOffsetAddress(int incomingAdress) {
		int m = 1;
		for (int i = 0; i < offsetBits - 1; i++) {
			m = m << 1;
			m = m + 1;

		}
		int x = incomingAdress & m;
		;

		return x;
	}
	int getPhysicalAddress(int frameAddress, int offsetAddress) {
		for (int i = 0; i < offsetBits; i++) {
			frameAddress = frameAddress << 1;

		}

		return frameAddress + offsetAddress;
	}

};

#endif /* VIRTUALMEMORY_H_ */
