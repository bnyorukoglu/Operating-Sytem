#ifndef TestProcess_h
#define TestProcess_h
#include "AbstractProcess.h"
#include <pthread.h>
class TestProcess: public AbstractProcess {
public:
	TestProcess(OpSys *os, int base, int limit) :
			AbstractProcess("TestProcess", os, base, limit) {
	}

	~TestProcess();

	void* sort(void *s) {

		int size = this->getLimit();
		int startPoint = this->getBase();

		for (int i = startPoint; i < size; ++i) {
			printf("\n%d - %d ", i, this->get(i));

		}

	}

};

#endif

