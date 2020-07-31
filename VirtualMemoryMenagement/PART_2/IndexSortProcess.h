/*
 * indexSortProcess.h
 *
 *  Created on: Jul 7, 2020
 *      Author: bengi
 */

#ifndef INDEXSORTPROCESS_H_
#define INDEXSORTPROCESS_H_
#include "AbstractProcess.h"
class IndexSortProcess: public AbstractProcess {
public:
	IndexSortProcess(OpSys *memory, int base, int limit) :
			AbstractProcess("IndexSort", memory, base, limit) {
	}

	~IndexSortProcess();

	void* sort(void *s) {
		int set = 0, temp = 0;
		int size = this->getLimit();
		int startPoint = this->getBase();

		while (set != 1) {
			set = 1;
			for (int i = startPoint; i < size - 1; ++i) {

				int a = this->get(i);
				int b = this->get(i + 1);
				if (a > b) {
					set = 0;
					temp = a;
					this->set(i, b);
					this->set(i + 1, temp);
				}
			}
			size = size - 1;
			//printf("%d\n",size);
		}
		pthread_exit(NULL);
	}

};

#endif /* INDEXSORTPROCESS_H_ */
