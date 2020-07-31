/*
 * SecondChanceReplacement.h
 *
 *  Created on: Jul 10, 2020
 *      Author: bengi
 */

#ifndef SECONDCHANCEREPLACEMENT_H_
#define SECONDCHANCEREPLACEMENT_H_

#include "ReplacementPolicy.h"

class SecondChanceReplacement: public ReplacementPolicy {
public:
	SecondChanceReplacement() {
		// TODO Auto-generated constructor stub

	}
	int getFreePage(P_PageTable table) {

		int size = table->referencePages->size();
		for (int i = 0; i < size; i++) {
			int pageIndex = table->referencePages->front();
			if (table->pages[i].referenced == 1) {
				table->referencePages->pop_front();
				table->referencePages->push_back(pageIndex);
			} else {
				table->referencePages->pop_front();
				return pageIndex;
			}
		}
		int pageIndex = table->referencePages->front();
		table->referencePages->pop_front();
		return pageIndex;

	}
	void updatePageTables(P_PageTable table,int address) {
		int size = table->referencePages->size();
		for (int i = 0; i < size; i++) {
			int t = table->referencePages->at(i);
			table->pages[t].referenced = 0;
		}
	}
};

#endif /* SECONDCHANCEREPLACEMENT_H_ */
