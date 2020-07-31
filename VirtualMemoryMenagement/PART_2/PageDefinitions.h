/*
 * PageDefinitions.h
 *
 *  Created on: Jul 9, 2020
 *      Author: bengi
 */

#ifndef PAGEDEFINITIONS_H_
#define PAGEDEFINITIONS_H_
using namespace std;
#include <queue>
#include <deque>
typedef enum AccessOperation {
	Read, Write, PageMiss, PageReplace, PageRead, PageWrite
};
typedef struct pageTableEntry {
	int frame_Number;
	int present_absent;
	int protection;
	int reference_bit;
	int dirty_bit;
	unsigned int referenced =0;

} PageTableEntry;

typedef PageTableEntry *P_PageTableEntry;
typedef struct pageTable {
	int max_frame_count = 0;
	int current_frame_count = 0;
	P_PageTableEntry pages;

	deque<int> *referencePages;

} PageTable;
typedef PageTable *P_PageTable;

#endif /* PAGEDEFINITIONS_H_ */
