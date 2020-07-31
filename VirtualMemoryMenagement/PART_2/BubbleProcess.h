/*
 * bubbleProcess.h
 *
 *  Created on: Jul 7, 2020
 *      Author: bengi



 #define BUBBLEPROCESS_H_
 #include "AbstractProcess.h"
 #include <pthread.h>
 class BubbleProcess: public AbstractProcess {
 public:
 BubbleProcess(OpSys *os, int base, int limit) :
 AbstractProcess("BubleSort", os, base, limit) {
 }

 ~BubbleProcess();
 void* sort(void *s) {
 char temp;
 int i = 0, j = 0;
 int index = 0;
 int size = this->getLimit();
 int startPoint = this->getBase();

 for (i = startPoint; i < size; i++) {
 for (j = startPoint; j < size; j++) {
 temp = this->get(i);
 int k = this->get(j);
 if (temp < k) {
 this->set(i, k);
 this->set(j, temp);
 this->set(j, temp);
 }

 }
 }
 //pthread_exit(NULL);
 //return NULL;
 }


 };
 */
#ifndef BUBBLEPROCESS_H_
#define BUBBLEPROCESS_H_
#include "AbstractProcess.h"
#include <pthread.h>
class BubbleProcess: public AbstractProcess {
public:
	BubbleProcess(OpSys *os, int base, int limit) :
			AbstractProcess("BubleSort", os, base, limit) {
	}

	~BubbleProcess();

	void* sort(void *s) {
		int control = 0, temp = 0;
		int size = this->getLimit();
		int startPoint = this->getBase();

		while (control != 1) {
			control = 1;
			for (int i = startPoint; i < size - 1; ++i) {

				int a = this->get(i);
				int b = this->get(i + 1);
				if (a > b) {
					control = 0;
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

#endif

