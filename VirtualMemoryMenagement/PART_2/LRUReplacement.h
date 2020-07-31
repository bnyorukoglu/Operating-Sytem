/*
 * LRUReplacement.h
 *
 *  Created on: Jul 10, 2020
 *      Author: bengi
 */

#ifndef LRUREPLACEMENT_H_
#define LRUREPLACEMENT_H_
#include "ReplacementPolicy.h"
#include "PageDefinitions.h"
#include <stdint.h>
class LRUReplacement: public ReplacementPolicy {
public:
	LRUReplacement() {
		// TODO Auto-generated constructor stub

	}

	void updatePageTables(P_PageTable table, int address) {
		int size = table->referencePages->size();
		for (int i = 0; i < size; i++) {
			int t = table->referencePages->at(i);
			if (t == address) {
				table->pages[t].referenced += 2;
			}
			if (table->pages[t].referenced != 0)
				table->pages[t].referenced--;
		}

	}
	int getFreePage(P_PageTable table) {
		unsigned int min = INT32_MAX;

		int pageIndex = 0;
		int delIndex =0 ;
		int size = table->referencePages->size();
		for (int i = 0; i < size; i++) {
			int t = table->referencePages->at(i);
			if (table->pages[t].referenced <= min) {
				min = table->pages[t].referenced;
				pageIndex = t;
				delIndex = i;
			}
		}
		table->referencePages->erase(
				table->referencePages->begin() + delIndex);
		return pageIndex;
	}
	~LRUReplacement();
};

#endif /* LRUREPLACEMENT_H_ */
