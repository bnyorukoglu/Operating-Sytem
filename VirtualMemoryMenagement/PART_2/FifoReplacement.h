/*
 * FifoReplacement.h
 *
 *  Created on: Jul 8, 2020
 *      Author: bengi
 */

#ifndef FifoReplacement_H_
#define FifoReplacement_H_

#include "ReplacementPolicy.h"
#include "PageDefinitions.h"

class FifoReplacement: public ReplacementPolicy {
public:
	FifoReplacement() {

	}
	void updatePageTables(P_PageTable table,int address) {

	}
	int getFreePage(P_PageTable table) {
		int pageIndex = table->referencePages->front();
		table->referencePages->pop_front();
		return pageIndex;
	}

	~FifoReplacement();
};

#endif /* LRU_H_ */
