/*
 * mergeProcess.h
 *
 *  Created on: Jul 7, 2020
 *      Author: bengi
 */

#ifndef MERGEPROCESS_H_
#define MERGEPROCESS_H_
#include "AbstractProcess.h"

class MergeProcess: public AbstractProcess {
public:
	MergeProcess(OpSys *memory, int base, int limit) :
			AbstractProcess("MergeSort", memory, base, limit) {
	}
	;
	~MergeProcess();
	void* merge(void *threadarg);

	void merge(int startPoint, int size) {
		int beforeMidPoint = startPoint;
		int middlePoint = (startPoint + size) / 2;
		int afterMidPoint = middlePoint + 1;
		int tempArr[size - startPoint + 1];
		int mergeIndex = 0;
		int tempArrIndex = 0;

		while (afterMidPoint <= size  && beforeMidPoint <= middlePoint ) {

			if (this->get(beforeMidPoint) > this->get(afterMidPoint)) {
				tempArr[tempArrIndex++] = this->get(afterMidPoint++);

			} else {

				tempArr[tempArrIndex++] = this->get(beforeMidPoint++);

			}
			mergeIndex++;
		}
		for (int i=afterMidPoint; i <= size; i++) {
			tempArr[tempArrIndex++] = this->get(i);
		}
		for (int i=beforeMidPoint; i <= middlePoint; i++) {
			tempArr[tempArrIndex++] = this->get(i);
		}

		int control=size - startPoint + 1;
		int index=0;
		for (beforeMidPoint = 0; beforeMidPoint < control; beforeMidPoint++) {
			index=startPoint + beforeMidPoint;
			this->set(index, tempArr[beforeMidPoint]);

		}

	}

	void mergesort(int startPoint, int size) {
		int add=1;
		int mid = (startPoint + size) / 2;

		if (startPoint >= size)
			return;

		mergesort(startPoint, mid);
		mergesort(mid + add, size);
		merge(startPoint, size);
		return;
	}
	void* sort(void *s) {
		int size = this->getLimit();
		int startPoint = this->getBase();

		mergesort(startPoint, size);
		return NULL;
	}

};

#endif /* MERGEPROCESS_H_ */
