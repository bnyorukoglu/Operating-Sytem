#ifndef SYSTEMUTILS_H_
#define SYSTEMUTILS_H_
#include "string.h"
#include<stdio.h>
#include<stdlib.h>
#define FS_PATH_SEPERATOR "/"
int parsePath(char * string2, char parsed[10][50]) { // In File_System Path Parser

	char * token = strtok(string2, FS_PATH_SEPERATOR);
	// loop through the string to extract all other tokens
	int i = 0;
	while (token != NULL) {

		strcpy(parsed[i], token);

		i++;
		token = strtok(NULL, FS_PATH_SEPERATOR);

	}
	return i;

}

class SemiDynamicArray { // Simple structure for holding stuff;
	int size = 0;

	int * buffer;
public:
	int lentgh = 0;
	SemiDynamicArray(int size = 0) {
		this->size = size;
		buffer = (int *) malloc(sizeof(int) * size);
	}

	void put(int item) {
		for (int i = 0; i < this->lentgh; i++) {
			if (buffer[i] == item)
				return;
		}

		buffer[this->lentgh] = item;
		this->lentgh++;

	}

	int get(int index) {
		return this->buffer[index];

	}
	void clear() {
		free(buffer);
	}

};

#endif /* SYSTEMUTILS_H_ */
