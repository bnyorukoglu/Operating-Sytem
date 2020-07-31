/*
 * ReplacementPolicy.h
 *
 *  Created on: Jul 8, 2020
 *      Author: bengi
 */

#ifndef REPLACEMENTPOLICY_H_
#define REPLACEMENTPOLICY_H_

#include "PageDefinitions.h"
class ReplacementPolicy {
public:
	ReplacementPolicy(){

	}
	virtual void updatePageTables(P_PageTable table,int address)=0;


	virtual int getFreePage(P_PageTable table)=0;
	~ReplacementPolicy();
};

#endif /* REPLACEMENTPOLICY_H_ */
