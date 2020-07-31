/*
 * AbstractProcess.h
 *
 *  Created on: Jul 7, 2020
 *      Author: bengi
 */
#pragma once
#ifndef ABSTRACTPROCESS_H_
#define ABSTRACTPROCESS_H_
#include <vector>
#include <pthread.h>
#include "OpSys.h"
typedef void* (*THREADFUNCPTR)(void*);

class AbstractProcess {
public:
	int pagingStatistics[6];
	AbstractProcess();

	AbstractProcess(char *pname, OpSys *operatingSystem, int base, int limit) {
		this->pname = pname;
		this->base = base;
		this->limit = limit;
		this->operatingSystem = operatingSystem;
		operatingSystem->registerProcess(this);
		for (int i = 0; i < 6; i++) {
			pagingStatistics[i] = 0;
		}

	}

	~AbstractProcess();

	virtual void* sort(void *s) {
		return NULL;

	}

	virtual pthread_t* run() {

		return NULL;
	}
	void set(unsigned int index, int value) {

		this->operatingSystem->set(index, value, this);
	}
	int get(unsigned int index) {
		return this->operatingSystem->get(index, this);
	}
	int getBase() {
		return base;
	}
	int getLimit() {
		return limit;
	}

	char* getPname() const {
		return pname;
	}

private:
	OpSys *operatingSystem;
	int base;
	int limit;
	std::vector<int> arr;
	char *pname;

};

#endif /* ABSTRACTPROCESS_H_ */
