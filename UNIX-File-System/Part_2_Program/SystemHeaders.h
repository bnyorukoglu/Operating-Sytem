/*
 * SystemHeaders.h
 *
 *  Created on: May 24, 2020
 *      Author: Bengi
 */

#ifndef SYSTEMHEADERS_H_
#define SYSTEMHEADERS_H_
#define MAX_FILE_NAME_LENGTH 50
#define DEFAULT_BUFFER_SIZE 64

#define F_FILE  0
#define F_FOLDER 1
#define F_ROOT 2
#define F_S_LINK 3
#define F_H_LINK 4
#define EMPTY -1

#define ADDRESSABLE_DATA_BLOCKS 4
#define DEPTH_LEVEL  4

typedef struct FileDescriptor { //68bytes
	int parent = 0; //4
	int type = 0; //4
	int size = 0; //4
	int iNode = 0; //4
	char fileName[MAX_FILE_NAME_LENGTH]; //52
} FILE_DESCRIPTOR;

typedef FILE_DESCRIPTOR * P_FILE_DESCRIPTOR;

typedef struct inode { //44 byte
	int fileSize = 0;//4
	int id = 0; //4 byte
	int nodeType; //4 byte
	int size = 0; //4 byte
	long lastModificationdate = 0; //8 byte
	int referenceCount = 0; //4 byte
	int references[ADDRESSABLE_DATA_BLOCKS]; //16 byte

} INODE;
typedef INODE* P_INODE;

struct SuperBlock { //48 bytes

	int blocksize; //4
	int diskSize; //4
	int iNodeStartAddress; //4
	int dataBlockStartAddress; //4
	int emptyInodeCount; //4
	int emptyDataBlockCount; //4
	int inodeCount; //4
	int rootAddress; //4
	int emptyInodeAdress; //4
	int emptyDataBlockAddress; //4
	int dentryAdress; //4;
	int blockCount; //4;

};




#endif /* SYSTEMHEADERS_H_ */
