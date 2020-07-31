/*
 * quickSort.h
 *
 *  Created on: Jul 7, 2020
 *      Author: bengi
 */

#ifndef QUICKSORTPROCESS_H_
#define QUICKSORTPROCESS_H_
#include "AbstractProcess.h"
#include <iostream>
#include <algorithm>
class QuickSortProcess: public AbstractProcess {
public:
	QuickSortProcess(OpSys *memory, int base, int limit) :
			AbstractProcess("QuickSort", memory, base, limit) {
	}
	;
	 ~QuickSortProcess();
	void swap(int& a, int& b);
	void* sort(void *s) {

		 int first=this->getBase();
		 int last=this->getLimit();
		 quickSort(first, last);
		return NULL;
	}
	/**
	 * Quicksort.
	 * @param a - The array to be sorted.
	 * @param first - The start of the sequence to be sorted.
	 * @param last - The end of the sequence to be sorted.
	*/
	void quickSort(int first, int last )
	{
	    int pivotElement;

	    if(first < last)
	    {
	        pivotElement = pivot(first, last);
	        quickSort(first, pivotElement-1);
	        quickSort(pivotElement+1, last);
	    }
	}

	/**
	 * Find and return the index of pivot element.
	 * @param a - The array.
	 * @param first - The start of the sequence.
	 * @param last - The end of the sequence.
	 * @return - the pivot element
	*/
	int pivot(int first, int last)
	{
	    int  p = first;
	    int pivotElement = this->get(first);

	    for(int i = first+1 ; i <= last ; i++)
	    {
	        /* If you want to sort the list in the other order, change "<=" to ">" */
	        if(this->get(i) <= pivotElement)
	        {
	            p++;

	            int temp = this->get(i);
	            int temp2 = this->get(p);
	            this->set(i, temp2);
	            this->set(p, temp);

	        }
	    }
	    int temp = this->get(p);
	   	int temp2 = this->get(first);
	   	this->set(p, temp2);
	   	this->set(first, temp);

	    return p;
	}
};

#endif /* QUICKSORTPROCESS_H_ */
