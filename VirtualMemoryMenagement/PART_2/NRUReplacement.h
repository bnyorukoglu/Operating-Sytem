/*
 * NRUReplacement.h
 *
 *  Created on: Jul 10, 2020
 *      Author: bengi
 */

#ifndef NRUREPLACEMENT_H_
#define NRUREPLACEMENT_H_

#include "ReplacementPolicy.h"

class NRUReplacement: public ReplacementPolicy {
public:
	NRUReplacement() {
		// TODO Auto-generated constructor stub

	}

	int getFreePage(P_PageTable table) {

		int size = table->referencePages->size();

		for (int i = 0; i < size; i++) {
			int t = table->referencePages->at(i);
			if (table->pages[t].referenced == 0
					&& table->pages[t].dirty_bit == 0) {
				table->referencePages->erase(
						table->referencePages->begin() + i);

				return t;
			}
		}
		for (int i = 0; i < size; i++) {
			int t = table->referencePages->at(i);
			if (table->pages[t].referenced == 0) {
				table->referencePages->erase(
						table->referencePages->begin() + i);

				return t;
			}
		}
		for (int i = 0; i < size; i++) {
			int t = table->referencePages->at(i);
			if (table->pages[t].referenced == 1
					&& table->pages[t].dirty_bit == 0) {
				table->referencePages->erase(
						table->referencePages->begin() + i);

				return t;
			}
		}
		for (int i = 0; i < size; i++) {
			int t = table->referencePages->at(i);
			if (table->pages[t].referenced == 1
					&& table->pages[t].dirty_bit == 1) {
				table->referencePages->erase(
						table->referencePages->begin() + i);

				return t;
			}
		}

	}
	void updatePageTables(P_PageTable table,int address) {
		int size = table->referencePages->size();
		for (int i = 0; i < size; i++) {
			int t = table->referencePages->at(i);
			table->pages[t].referenced = 0;
		}
	}
};

#endif /* NRUREPLACEMENT_H_ */
