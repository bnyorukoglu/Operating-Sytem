/*
 * FileSystem.h
 *
 *  Created on: May 20, 2020
 */

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include "SystemHeaders.h"
#include "RawAccess.h"
#include "string.h"
#include "time.h"
#include "SystemUtils.h"
#include "math.h"

class FileSystem {

public:
	void close() {
		rawDisks->writeAt(START_ADDRESS, superBlock, sizeof(SuperBlock));
		rawDisks->close();
	}
	int getBlockSize() const {
		return blockSize;
	}

	void setBlockSize(ll blockSize) {
		this->blockSize = blockSize;
	}
	FileSystem(RawDisks* rawDisks) {
		this->rawDisks = rawDisks;
	}
	FileSystem(RawDisks* rawDisks, ll blockSize) {
		this->blockSize = blockSize;
		this->rawDisks = rawDisks;
	}

	FileDescriptor * getFileDescriptor(int blockId) {

		FileDescriptor *pt = (FileDescriptor *) rawDisks->readFrom(
				getBlockAddress(superBlock->rootAddress + blockId),
				sizeof(FileDescriptor));
		return (FileDescriptor *) rawDisks->readFrom(
				getBlockAddress(superBlock->rootAddress + blockId),
				sizeof(FileDescriptor) * (pt->size + 1));

	}

	FileDescriptor * getRootDirectory() {

		return getFileDescriptor(0);

	}
	FileDescriptor * getDescriptorOf(P_INODE inode) {

		FileDescriptor *pt = getFileDescriptor(inode->references[0]);
		return pt;

	}
	void printBlock(int id) {

		int *array = (int *) this->readBlock(
				this->superBlock->dataBlockStartAddress + id, array,
				sizeof(int) * 4);
		for (int i = 0; i < 4; i++) {
			printf("%d\n", array[i]);
		}
	}

	void printFolderInformation(FileDescriptor *descriptor) {

		int count = descriptor->size;
		char time_buffer[26];

		time_t time = (time_t) getInode(descriptor->iNode)->lastModificationdate;
		tm * tm_info = localtime(&time);
		strftime(time_buffer, 25, "%d %m %Y %H:%M:%S", tm_info);

		printf("\n%-30.50s \t %.12s \t %.5s \t %.5s \t %.5s \t%.30s",
				"File Name", "Parent Inode", "Type", "Inode", "Size",
				"Last Modification Date");
		printf("\n%-30.50s \t %6d \t %3d \t %3d \t %3d \t%.30s", ".",
				descriptor->iNode, descriptor->type, descriptor->iNode, 0,
				time_buffer);
		printf("\n%-30.50s \t %6d \t %3d \t %3d \t %3d \t%.30s", "..", 0,
				descriptor->type, descriptor->iNode, 0, time_buffer);
		for (int i = 1; i < count + 1; i++) {
			FileDescriptor temp = descriptor[i];
			P_INODE inode = getInode(temp.iNode);
			time = (time_t) getInode(descriptor->iNode)->lastModificationdate;
			tm_info = localtime(&time);
			strftime(time_buffer, 25, "%d %m %Y %H:%M:%S", tm_info);
			if (temp.type == F_H_LINK) {
				printf("\n%-30.50s** \t %6d \t %3d \t %3d \t %.3d \t%.30s",
						temp.fileName, temp.parent, temp.type, inode->id,
						inode->size, time_buffer);
			}
			printf("\n%-30.50s \t %6d \t %3d \t %3d \t %3d \t%.30s",
					temp.fileName, temp.parent, temp.type, inode->id,
					inode->size, time_buffer);
		}
		printf("\n");
	}
	FileDescriptor *checkExists(char path[10][50], int length) {
		FileDescriptor *descriptor = getRootDirectory();
		FileDescriptor *parent = descriptor;

		if (length == 0 || length == 1)
			return descriptor;

		int depth = length;
		bool found = false;
		while (depth > 0) {
			for (int i = 0; i < length; i++) {
				found = false;
				for (int j = 1; j < descriptor->size + 1; j++) {

					if (strcmp(parent[j].fileName, path[i]) == 0) {

						depth--;
						found = true;

						if (depth == 1)
							return &parent[j];
						P_INODE inode = getInode(parent[j].iNode);
						descriptor = getFileDescriptor(inode->references[0]);
						parent = descriptor;

					}
				}
				if (found == false)
					return NULL;

			}

		}
		return NULL;
	}
	FileDescriptor *getDirectoryInfo(char path[10][50], int length) {

		FileDescriptor *descriptor = getRootDirectory();
		FileDescriptor *parent = descriptor;
		FileDescriptor *contents = descriptor;
		if (length == 0 || length == 1)
			return descriptor;

		int depth = length;
		bool found = false;
		while (depth > 0) {
			for (int i = 0; i < length; i++) {
				found = false;
				for (int j = 1; j < descriptor->size + 1; j++) {

					if (strcmp(parent[j].fileName, path[i]) == 0) {
						P_INODE inode = getInode(parent[j].iNode);
						descriptor = getFileDescriptor(inode->references[0]);
						parent = descriptor;
						depth--;
						found = true;

						if (depth == 1)
							return parent;

					}
				}
				if (found == false)
					return NULL;

			}

		}
		return NULL;
		for (int i = 0; i < length; i++) {

			for (int j = 1; j < descriptor->size + 1; j++) {
				if (strcmp(descriptor[j].fileName, path[i]) == 0) {
					P_INODE inode = getInode(descriptor[j].iNode);
					descriptor = getFileDescriptor(inode->references[0]);
					contents = descriptor;
					found = true;
				}

			}
			if (found == false) {
				return NULL;
			}

		}
		return contents;

	}

	bool removeLink(char path[10][50], int length) {
		return removeDiskEntry(path, length, F_H_LINK);

	}

	bool removeFile(char path[10][50], int length) {
		return removeDiskEntry(path, length, F_FILE);

	}
	bool removeDirectory(char path[10][50], int length) {

		return removeDiskEntry(path, length, F_FOLDER);

	}
	bool createFile(char path[10][50], int length) {
		return createLogicalObject(path, length, F_FILE);

	}

	bool createDirectory(char path[10][50], int length) {
		return createLogicalObject(path, length, F_FOLDER);
	}

	bool createLink(char * source, char * destination) {
		char parsed[10][50];
		int type = F_H_LINK;
		int count = parsePath(destination, parsed);
		FileDescriptor * destinationDescriptor = checkExists(parsed, count + 1);
		if (destinationDescriptor == NULL) {
			printf("\nError while getting information\n");
			return false;
		}
		if ((destinationDescriptor->type == F_FOLDER) && type == F_H_LINK) {
			printf("\n Hard link between folders can not happen\n");
			return false;
		}
		if (type == F_S_LINK) {
			printf("\n Not implemented\n");
			return false;
		}
		count = parsePath(source, parsed);
		if (createLogicalObject(parsed, count, F_FILE) == false) {
			printf("\nError while getting information\n");
			return false;
		}
		FileDescriptor * sourceDescriptor = getDirectoryInfo(parsed, count);
		if (type == F_H_LINK) {

			FileDescriptor *sourceDescriptor2 = getDirectoryInfo(parsed,
					count + 1);
			P_INODE inode_source = getInode(sourceDescriptor2->iNode);
			P_INODE inode_destination = getInode(destinationDescriptor->iNode);
			inode_destination->referenceCount++;

			sourceDescriptor[1].size++;
			sourceDescriptor[1].iNode = destinationDescriptor->iNode;
			sourceDescriptor[1].type = F_H_LINK;
			int address = getBlockAddress(
					superBlock->dataBlockStartAddress
							+ getInode(sourceDescriptor->iNode)->references[0]);
			rawDisks->writeAt(address, sourceDescriptor,
					2 * sizeof(FileDescriptor));

			rawDisks->writeAt(
					superBlock->iNodeStartAddress
							+ (sizeof(INODE) * inode_destination->id),
					inode_destination, sizeof(INODE));

			freeInode(inode_source);

		}

		return true;

	}
	void printptrInasd() {
		FileDescriptor * desc = getFileDescriptor(2);
		printFolderInformation(desc);
	}
	bool copyFromOtherFileSystem(char * source, char * destination) {

		FILE *sourceFile = fopen(source, "r");
		if (sourceFile == NULL) {
			printf("Source target does not exits\n");
			return false;
		}
		char parsed[10][50];
		int count = parsePath(destination, parsed);
		if (!createFile(parsed, count)) {
			printf("\nError while getting information\n");
			fclose(sourceFile);
			return false;
		}

		FileDescriptor *descirptor = getDirectoryInfo(parsed, count + 1);

		if (descirptor == NULL || descirptor->type != F_FILE) {
			printf("Error while creating a File\n");
			fclose(sourceFile);
			return false;
		}

		P_INODE inode = getInode(descirptor->iNode);
		int readCount = 0;
		int inBlockIndex = 0;
		int currentRead = 0;
		BYTE *buffer = (BYTE *) malloc(sizeof(BYTE) * DEFAULT_BUFFER_SIZE);
		int currentBlock = inode->references[0];
		int usedBlock = 0;
		while ((currentRead = fread(buffer, sizeof(BYTE),
		DEFAULT_BUFFER_SIZE, sourceFile)) == DEFAULT_BUFFER_SIZE) {
			//printptrInasd();
			//printf("%d\n", currentBlock);

			int expectedBlock = usedBlock
					+ (int) ((readCount - (usedBlock * superBlock->blocksize))
							/ superBlock->blocksize);

			if (expectedBlock != usedBlock) {
				currentBlock = getNewBlock(inode, usedBlock, expectedBlock);
				if (currentBlock == EMPTY) {
					printf("Error allocating new blocks.\n");
					printf("Reversing Operations.\n");
					fclose(sourceFile);
					free(buffer);
					removeFile(parsed, count);

					return false;
				}

				usedBlock++;
				inBlockIndex = 0;
			}

			writeAtBlock(superBlock->dataBlockStartAddress + currentBlock,
					buffer, currentRead, inBlockIndex);
			readCount += currentRead;
			inBlockIndex += currentRead;
			inode->size = readCount;
			rawDisks->writeAt(
					superBlock->iNodeStartAddress + (inode->id * sizeof(INODE)),
					inode, sizeof(INODE));

		}

		int expectedBlock = usedBlock
				+ (int) ((readCount - (usedBlock * superBlock->blocksize))
						/ superBlock->blocksize);

		if (expectedBlock != usedBlock) {
			currentBlock = getNewBlock(inode, usedBlock, expectedBlock);
			if (currentBlock == EMPTY) {
				printf("Error allocating new blocks.\n");
				printf("Reversing Operations.\n");
				fclose(sourceFile);
				free(buffer);
				removeFile(parsed, count);

				return false;
			}
			usedBlock++;
			inBlockIndex = 0;
			//	writeAtBlock(currentBlock, buffer, currentRead, inBlockIndex);

		}

		writeAtBlock(superBlock->dataBlockStartAddress + currentBlock, buffer,
				currentRead, inBlockIndex);

		readCount += currentRead;
		inode->size = readCount;
		rawDisks->writeAt(
				superBlock->iNodeStartAddress + (inode->id * sizeof(INODE)),
				inode, sizeof(INODE));
		//	printptrInasd();
		descirptor = getDirectoryInfo(parsed, count);

		fclose(sourceFile);
		free(buffer);

		return true;

	}

	bool copyIntoOtherFileSystem(char * source, char * destination) {
		FILE *destinationFile = fopen(destination, "w");
		if (destinationFile == NULL) {
			printf("Source target does not exits\n");
			return false;
		}
		char parsed[10][50];
		int count = parsePath(source, parsed);

		FileDescriptor *descirptor = checkExists(parsed, count + 1);

		if (descirptor == NULL || descirptor->type != F_FILE) {
			printf("\nError while getting information\n");
			fclose(destinationFile);
			return false;
		}
		P_INODE inode = getInode(descirptor->iNode);
		int size = inode->size;
		int blockCount = (int) (size / superBlock->blocksize);
		int residual = size - blockCount * superBlock->blocksize;

		if (blockCount > 0)
			size = superBlock->blocksize;
		BYTE *buffer = (BYTE *) malloc(sizeof(BYTE) * size);

		buffer = readBlock(
				superBlock->dataBlockStartAddress + inode->references[0],
				buffer, size);

		fwrite(buffer, sizeof(BYTE), size, destinationFile);
		if (blockCount == 0) {
			fclose(destinationFile);

			return true;
		}

		int *block = (int *) malloc(sizeof(int) * ADDRESSABLE_DATA_BLOCKS);

		for (int i = 1; i < blockCount; i++) {
			int *paths = getIndirectReference(i);

			int blockId = inode->references[paths[0]];
			for (int j = 1; j < paths[0] + 1; j++) {
				block = (int *) readBlock(
						superBlock->dataBlockStartAddress + blockId, block,
						ADDRESSABLE_DATA_BLOCKS * sizeof(int));
				blockId = block[paths[j]];
			}

			buffer = readBlock(superBlock->dataBlockStartAddress + blockId,
					buffer, size);
			fwrite(buffer, sizeof(BYTE), size, destinationFile);
			fflush(destinationFile);

		}
		free(buffer);
		if (residual > 0) {
			int *paths = getIndirectReference(blockCount);
			int blockId = inode->references[paths[0]];
			for (int j = 1; j < paths[0] + 1; j++) {
				block = (int *) readBlock(
						superBlock->dataBlockStartAddress + blockId, block,
						ADDRESSABLE_DATA_BLOCKS * sizeof(int));
				blockId = block[paths[j]];
			}

			buffer = (BYTE *) malloc(sizeof(BYTE) * residual);

			buffer = readBlock(superBlock->dataBlockStartAddress + blockId,
					buffer, residual);

			fwrite(buffer, sizeof(BYTE), residual, destinationFile);
		}

		fclose(destinationFile);
		free(buffer);
		return true;
	}

	void fsck_disk() {
		SemiDynamicArray *arr1 = new SemiDynamicArray(superBlock->blockCount);

		int array1[superBlock->blockCount];
		int array2[superBlock->blockCount];
		for (int j = 0; j < superBlock->blockCount; j++) {
			array1[j] = 0;
			array2[j] = 0;

		}

		for (int i = 0; i < superBlock->inodeCount; i++) {

			P_INODE inode = getInode(i);

			if (inode->nodeType != EMPTY) {

				SemiDynamicArray nodes = getUsedBlocksOfInode(inode);

				for (int j = 0; j < nodes.lentgh; j++) {
					arr1->put(nodes.get(j));
				}
			}

		}

		fflush(stdin);
		int *emptyNodes = (int *) rawDisks->readFrom(
				superBlock->emptyDataBlockAddress,
				superBlock->emptyDataBlockCount * sizeof(int));
		for (int j = 0; j < arr1->lentgh; j++) {

			array1[arr1->get(j)]++;

		}
		for (int j = 0; j < superBlock->emptyDataBlockCount - 1; j++) {

			array2[emptyNodes[j]]++;

		}
		int j = 0;

		for (int i = 0; i < (superBlock->blockCount / 50) + 1; i++) {

			printf("\nBlock Number:");
			for (j = 0; j < 50; j++) {
				if (50 * i + j == superBlock->blockCount)
					break;

				printf("%3d ", 50 * i + j);

			}
			printf("\n");

			for (j = 0; j < 212; j++) {

				printf("-");
			}

			printf("\nINode Blocks:");
			for (j = 0; j < 50; j++) {
				if (50 * i + j == superBlock->blockCount)
					break;

				printf("%3d ", array1[50 * i + j]);

			}
			printf("\nEmpty Blocks:");
			for (j = 0; j < 50; j++) {
				if (50 * i + j == superBlock->blockCount)
					break;

				printf("%3d ", array2[50 * i + j]);

			}
			printf("\n");
		}

	}

	void formatDisk(int blockSize, int inodeCount) {
		SuperBlock superBlock;
		setBlockSize(blockSize);
		int diskSize = rawDisks->getDiskSize();
		int blockCount = diskSize / blockSize;
		superBlock.blocksize = blockSize;
		superBlock.blockCount = blockCount;
		superBlock.diskSize = diskSize;
		superBlock.inodeCount = inodeCount;
		superBlock.emptyInodeCount = inodeCount;
		superBlock.emptyDataBlockCount = superBlock.blockCount;
		superBlock.iNodeStartAddress = sizeof(superBlock);
		superBlock.emptyInodeAdress = superBlock.iNodeStartAddress
				+ sizeof(INODE) * inodeCount;
		superBlock.emptyDataBlockAddress = superBlock.emptyInodeAdress
				+ sizeof(int) * inodeCount;
		superBlock.dentryAdress = superBlock.emptyDataBlockAddress
				+ sizeof(int) * blockCount;

		int dataBlockStartAddress = (superBlock.dentryAdress
				+ sizeof(int) * blockCount) / blockSize + 1;

		superBlock.dataBlockStartAddress = dataBlockStartAddress;
		superBlock.rootAddress = dataBlockStartAddress;
		superBlock.blockCount = superBlock.blockCount
				- superBlock.dataBlockStartAddress;
		superBlock.emptyDataBlockCount = superBlock.blockCount;
		rawDisks->writeAt(START_ADDRESS, &superBlock, sizeof(SuperBlock));
		this->maxEntries = superBlock.blocksize / sizeof(FileDescriptor);

		for (int i = 0; i < superBlock.inodeCount; i++) {

			P_INODE node = createInode(i);

			rawDisks->writeAt(
					superBlock.iNodeStartAddress + (i * sizeof(INODE)), node,
					sizeof(INODE));
		}

		for (int i = 0; i < superBlock.inodeCount; i++) { // Empty inodeBlocks

			rawDisks->writeAt(superBlock.emptyInodeAdress + (i * sizeof(int)),
					&i, sizeof(int));
		}
		for (int i = 0; i < superBlock.emptyDataBlockCount; i++) { // Empty dataBlocks

			rawDisks->writeAt(
					superBlock.emptyDataBlockAddress + i * sizeof(int), &i,
					sizeof(int));
		}
		this->superBlock = &superBlock;
		readFileSystemMeta();
		createRootStructure();

	}

	void printBlockInfo(SuperBlock* superBlock) {
		printf("\n");
		printf("Disk Size => %d\n", superBlock->diskSize);
		printf("Block Size is => %d\n", superBlock->blocksize);
		printf(" Block Count is => %d\n", superBlock->blockCount);
		printf("iNode Count is => %d\n", superBlock->inodeCount);
		printf("Empty iNode Count is => %d\n", superBlock->emptyInodeCount);
		printf("Empty Data Block Count is => %d\n",
				superBlock->emptyDataBlockCount);
		printf("iNode Start Address is => %d\n", superBlock->iNodeStartAddress);
		printf("Empty iNode Start Address is => %d\n",
				superBlock->emptyInodeAdress);
		printf("Data Block Start Address is => %d\n",
				superBlock->dataBlockStartAddress);
		printf("Empty Data Block Start Address is => %d\n",
				superBlock->emptyDataBlockAddress);
		printf("Dentry Address is => %d\n", superBlock->dentryAdress);
		printf("Root Address is => %d\n", superBlock->rootAddress);
		printf("File System Info\n");
		FileDescriptor *descriptor = getRootDirectory();
		int fileCount = 0;
		int folderCount = 0;

		printFileSystemInformationRecursively(descriptor, descriptor,
				&fileCount, &folderCount);
		printf(
				"_______________________________________________________________ \n");
		printf("*** Shows Hard link \n");
		printf("Total File count is %d\n", fileCount);
		printf("Total Folder count is %d\n", folderCount);

	}

	char *getParentOf(FileDescriptor *descriptor) {

		char * name = (char*) malloc(sizeof(char) * 50);
		if (descriptor->type == F_ROOT)
			return FS_PATH_SEPERATOR;
		int length = 0;

		while (descriptor->type != F_ROOT) {

			if (length != 0) {
				for (int i = length; i > 0; i--) {

					name[i + strlen(descriptor->fileName)] = name[i - 1];

				}
				length += strlen(descriptor->fileName) + 1;

			}
			for (int i = 0; i < strlen(descriptor->fileName); i++) {
				name[i + 1] = descriptor->fileName[i];

			}
			name[0] = '/';
			length += strlen(descriptor->fileName) + 1;
			descriptor = getDescriptorOf(getInode(descriptor->parent));

		}
		//name[length] = '/0';
		return name;
	}
	void printFileSystemInformationRecursively(FileDescriptor *parent,
			FileDescriptor *descriptor, int *fileCount, int *folderCount) {

		for (int i = 1; i < descriptor->size + 1; i++) {

			FileDescriptor *temp = &descriptor[i];
			P_INODE inode = getInode(temp->iNode);
			if (temp->type == F_FILE || temp->type == F_H_LINK) {
				(*fileCount)++;
				if (temp->type == F_H_LINK) {
					printf("***");
				}

				printf("%-40.60s \t%d\t", getParentOf(temp), temp->iNode);

				SemiDynamicArray array = getUsedBlocksOfInode(
						getInode(temp->iNode));

				for (int i = 0; i < array.lentgh; i++) {
					printf("%3d,", array.get(i));
				}
				printf("\n");
				array.clear();

			}

			else {
				(*folderCount)++;
				printf("%-40.60s \t%d\t\%3d\n", getParentOf(temp), temp->iNode,
						inode->references[0]);
				printFileSystemInformationRecursively(temp,
						getDescriptorOf(inode), fileCount, folderCount);
			}

		}

	}
	void printFileSystemMeta() {
		printBlockInfo(this->superBlock);

	}
	SuperBlock* readFileSystemMeta() {

		this->superBlock = (SuperBlock *) rawDisks->readFrom(START_ADDRESS,
				sizeof(SuperBlock));
		this->maxEntries = superBlock->blocksize / sizeof(FileDescriptor);
	
		return this->superBlock;
	}

private:
	static const int START_ADDRESS = 0x00000;

	SuperBlock* superBlock = NULL;
	int blockSize = 0;
	int maxEntries = 0;
	RawDisks* rawDisks;

	void consumeDiskResource(P_INODE inode, int blockId) {

		consumeInode(inode);
		consumeDataBlock(blockId);
		rawDisks->writeAt(START_ADDRESS, this->superBlock, sizeof(SuperBlock));
	}

	SemiDynamicArray getUsedBlocksOfInode(P_INODE inode) {
		SemiDynamicArray *sArray = new SemiDynamicArray(200);
		if (inode->references[0] == EMPTY)
			return *sArray;
		sArray->put(inode->references[0]);
		int *block = (int *) malloc(sizeof(int) * ADDRESSABLE_DATA_BLOCKS);
		int blockCount = inode->size / superBlock->blocksize + 1;

		for (int i = 1; i < blockCount; i++) {
			int *paths = getIndirectReference(i);
			int blockId = inode->references[paths[0]];
			sArray->put(blockId);
			for (int j = 1; j < paths[0] + 1; j++) {
				block = (int *) readBlock(
						superBlock->dataBlockStartAddress + blockId, block,
						ADDRESSABLE_DATA_BLOCKS * sizeof(int));
				blockId = block[paths[j]];
				sArray->put(blockId);

			}

		}
		return *sArray;
	}

	void releaseDiskResource(P_INODE inode, int blockId) {

		readFileSystemMeta();

		if (inode->size > 0) {
			//	freeDataBlock(inode->references[0]);
			int *block = (int *) malloc(sizeof(int) * ADDRESSABLE_DATA_BLOCKS);
			int blockCount = inode->size / superBlock->blocksize;

			SemiDynamicArray *sArray = new SemiDynamicArray(200);
			sArray->put(inode->references[0]);
			for (int i = 1; i < blockCount; i++) {
				int *paths = getIndirectReference(i);
				int blockId = inode->references[paths[0]];
				sArray->put(blockId);
				for (int j = 1; j < paths[0] + 1; j++) {
					block = (int *) readBlock(
							superBlock->dataBlockStartAddress + blockId, block,
							ADDRESSABLE_DATA_BLOCKS * sizeof(int));
					blockId = block[paths[j]];

					sArray->put(blockId);

				}

			}

			for (int i = 0; i < sArray->lentgh; i++) {

				freeDataBlock(sArray->get(i));

			}
			sArray->clear();
			free(sArray);

		} else {
			for (int i = 0; i < 2; i++) {
				if (inode->references[i] != EMPTY) {
					freeDataBlock(inode->references[i]);

				}

			}
		}

		freeInode(inode);

		rawDisks->writeAt(START_ADDRESS, superBlock, sizeof(SuperBlock));

	}

	int getBlockAddress(int blockNumber) {
		return blockNumber * superBlock->blocksize;
	}

	void freeInode(P_INODE inode) {
		int *emptyNodes = (int *) rawDisks->readFrom(
				superBlock->emptyInodeAdress,
				superBlock->emptyInodeCount * sizeof(int));
		time_t currentTime;
		time(&currentTime);
		inode->referenceCount = 0;

		inode->size = 0;
		inode->nodeType = EMPTY;
		inode->references[0] = EMPTY;
		inode->references[1] = EMPTY;
		inode->references[2] = EMPTY;
		inode->references[3] = EMPTY;
		inode->lastModificationdate = currentTime;
		rawDisks->writeAt(superBlock->emptyInodeAdress + sizeof(int),
				emptyNodes, superBlock->emptyInodeCount * sizeof(int));

		rawDisks->writeAt(superBlock->emptyInodeAdress, &inode->id,
				sizeof(int));

		superBlock->emptyInodeCount++;
		rawDisks->writeAt(
				superBlock->iNodeStartAddress + (inode->id) * sizeof(INODE),
				inode, sizeof(INODE));

		rawDisks->writeAt(START_ADDRESS, this->superBlock, sizeof(SuperBlock));
		free(inode);

	}
	void freeBlockList(SemiDynamicArray *array) {

	}
	void freeDataBlock(int id) {

		superBlock->emptyDataBlockCount++;
		int * qtr = (int *) rawDisks->readFrom(
				superBlock->emptyDataBlockAddress,
				sizeof(int) * (superBlock->blockCount));
		for (int i = superBlock->blockCount - 1; i > -1; i--) {

			if (qtr[i] == id)
				return;

		}

		for (int i = superBlock->blockCount - 2; i > -1; i--) {

			qtr[i + 1] = qtr[i];

		}
		qtr[0] = id;

		rawDisks->writeAt(superBlock->emptyDataBlockAddress, qtr,
				sizeof(int) * (superBlock->blockCount));

		rawDisks->writeAt(START_ADDRESS, this->superBlock, sizeof(SuperBlock));
		free(qtr);

	}

	void consumeInode(P_INODE inode) {
		int *emptyNodes = (int *) rawDisks->readFrom(
				superBlock->emptyInodeAdress,
				superBlock->inodeCount * sizeof(int));
		bool found = false;
		for (int i = 0; i < superBlock->emptyInodeCount; i++) {
			if (emptyNodes[i] == inode->id) {
				found = true;
			}
			if (found) {
				emptyNodes[i] = emptyNodes[i + 1];
			}
		}

		emptyNodes[superBlock->emptyInodeCount] = EMPTY;
		superBlock->emptyInodeCount--;
		rawDisks->writeAt(superBlock->emptyInodeAdress, emptyNodes,
				superBlock->inodeCount * sizeof(int));
		rawDisks->writeAt(
				superBlock->iNodeStartAddress + (inode->id) * sizeof(INODE),
				inode, sizeof(INODE));

	}
	void consumeDataBlock(int blockNumber) {

		int *emptyNodes = (int *) rawDisks->readFrom(
				superBlock->emptyDataBlockAddress,
				superBlock->blockCount * sizeof(int));
		bool found = false;
		for (int i = 0; i < superBlock->blockCount - 1; i++) {

			if (emptyNodes[i] == blockNumber) {
				found = true;

			}
			if (found) {
				emptyNodes[i] = emptyNodes[i + 1];
			}

		}
		//printf("Empty %d\n",superBlock-> emptyDataBlockCount);
		superBlock->emptyDataBlockCount--;
		//printf("Empty %d\n",superBlock-> emptyDataBlockCount);
		rawDisks->writeAt(superBlock->emptyDataBlockAddress, emptyNodes,
				superBlock->blockCount * sizeof(int));
		rawDisks->writeAt(START_ADDRESS, this->superBlock, sizeof(SuperBlock));

	}
	P_INODE getFreeInode() {

		if (this->superBlock->emptyInodeCount < 1)
			return NULL;
		int *freeInodeId = (int *) rawDisks->readFrom(
				superBlock->emptyInodeAdress, sizeof(int));

		return getInode(*freeInodeId);

	}
	int getEmptyBlock() {

		if (this->superBlock->emptyDataBlockCount < 1)
			return EMPTY;
		int *freeInodeId = (int *) rawDisks->readFrom(
				superBlock->emptyDataBlockAddress,
				sizeof(int) * this->superBlock->emptyDataBlockCount);

		return *freeInodeId;

	}

	bool createDiskObject(int parentNodeId, char *name, int type) {
		P_INODE inode = getFreeInode();
		if (inode == NULL) {
			printf("Error While Creating folder Structure\n");
			return false;
		}
		int blockId = getEmptyBlock();
		if (blockId == EMPTY) {
			printf("No empty block has been found\n");
			return false;
		}
		inode->references[0] = blockId;

		P_INODE parent = (P_INODE) rawDisks->readFrom(
				superBlock->iNodeStartAddress + parentNodeId * sizeof(INODE),
				sizeof(INODE));
		FileDescriptor *folderDescriptor = getFileDescriptor(
				parent->references[0]);

		FileDescriptor descriptor;
		descriptor.parent = parentNodeId;
		descriptor.iNode = inode->id;
		strcpy(descriptor.fileName, name);
		descriptor.type = type;
		inode->nodeType = type;
		int address = getBlockAddress(
				superBlock->dataBlockStartAddress + parent->references[0]);
		int childAdress = getBlockAddress(
				superBlock->dataBlockStartAddress + inode->references[0]);
		if (type == F_ROOT) {
			descriptor.parent = inode->id;
			address = getBlockAddress(
					superBlock->dataBlockStartAddress + blockId);
			descriptor.size = 0;
			inode->nodeType = F_ROOT;
			rawDisks->writeAt(
					superBlock->iNodeStartAddress + inode->id * sizeof(INODE),
					inode, sizeof(INODE));
			rawDisks->writeAt(address, &descriptor, sizeof(FileDescriptor));

			consumeDiskResource(inode, blockId);
			return true;

		}

		folderDescriptor->size = folderDescriptor->size + 1;
		inode->referenceCount++;

		rawDisks->writeAt(
				superBlock->iNodeStartAddress + inode->id * sizeof(INODE),
				inode, sizeof(INODE));

		rawDisks->writeAt(address, folderDescriptor, sizeof(FileDescriptor));

		rawDisks->writeAt(
				address + sizeof(FileDescriptor) * (folderDescriptor->size),
				&descriptor, sizeof(FileDescriptor));

		rawDisks->writeAt(childAdress, &descriptor, sizeof(FileDescriptor));

		consumeDiskResource(inode, blockId);
		return true;

	}

	bool writeAtBlock(int blockId, void *buffer, int length, int offset) {
		rawDisks->writeAt(getBlockAddress(blockId) + offset, buffer, length);
		return true;

	}

	bool createLogicalObject(char path[10][50], int length, int type) {
		FileDescriptor *descirptor = getDirectoryInfo(path, length);

		if (descirptor == NULL
				|| (descirptor->type == F_FILE || descirptor->type == F_S_LINK
						|| descirptor->type == F_H_LINK)) {
			printf("\nError while getting information1\n");
			return false;
		}
		if (descirptor->size >= maxEntries) {
			printf("\nError while getting information2\n");
			return false;
		}
		for (int i = 1; i < descirptor->size + 1; i++) {
			if (strcmp(descirptor[i].fileName, path[length - 1]) == 0) {
				printf("\nObject with the same name exits!.\n");
				return false;
			}
		}

		createDiskObject(descirptor->iNode, path[length - 1], type);

		readFileSystemMeta();
		return true;
	}

	void writeBlock(int block, void* stream, int length) {
		this->rawDisks->writeAt(getBlockAddress(block), stream, length);
	}
	BYTE *readBlock(int block, void* stream, int length) {
		return this->rawDisks->readFrom(getBlockAddress(block), length);
	}
	P_INODE getInode(int inodeId) {

		P_INODE inode = (P_INODE) rawDisks->readFrom(
				superBlock->iNodeStartAddress + (sizeof(INODE) * inodeId),
				sizeof(INODE));

		return inode;
	}
	bool createRootStructure() {

		bool val = createDiskObject(0, FS_PATH_SEPERATOR, F_ROOT);
		readFileSystemMeta();
		return val;
	}

	P_INODE createInode(int id) {
		P_INODE inode = (P_INODE) malloc(sizeof(INODE));
		time_t currentTime;
		time(&currentTime);
		inode->id = id;
		inode->size = 0;
		inode->nodeType = EMPTY;
		inode->referenceCount = 0;
		inode->lastModificationdate = currentTime;
		inode->references[0] = EMPTY;
		inode->references[1] = EMPTY;
		inode->references[2] = EMPTY;
		inode->references[3] = EMPTY;
		return inode;

	}

	bool removeDiskEntry(char path[10][50], int length, int type) {

		FileDescriptor *descirptor = getDirectoryInfo(path, length);
		FileDescriptor *folderDescriptor = checkExists(path, length + 1);

		if (descirptor == NULL) {
			printf("\nError while getting information\n");
			return false;
		}

		if (folderDescriptor == NULL) {
			printf("\nError while getting information\n");
			return false;
		}
		if (folderDescriptor->type == F_ROOT) {
			printf("\nYou can not delete root folder\n");
			return false;
		}
		if (folderDescriptor->type != type) {
			printf("\nError while getting information\n");
			return false;
		}

		if (type == F_FOLDER){
			folderDescriptor = getDescriptorOf(getInode(folderDescriptor->iNode));
			if (folderDescriptor->size > 0) {
				printf("Folder is not empty\n");
				return false;
			}
		}

		P_INODE parent = getInode(descirptor->iNode);
		P_INODE child = getInode(folderDescriptor->iNode);

		FileDescriptor *newDesc = (FileDescriptor *) malloc(
				sizeof(FileDescriptor) * (descirptor->size + 1));

		int count = 0;
		for (int i = 0; i < descirptor->size + 1; i++) {
			if (strcmp(descirptor[i].fileName, folderDescriptor->fileName)
					!= 0) {
				memcpy(&newDesc[count], &descirptor[i], sizeof(FileDescriptor));
				count++;
			}

		}

		newDesc->size = descirptor->size - 1;
		rawDisks->writeAt(
				getBlockAddress(
						superBlock->dataBlockStartAddress
								+ parent->references[0]), newDesc,
				sizeof(FileDescriptor) * (newDesc->size + 1));

		if (child->referenceCount > 1) {
			child->referenceCount--;
			rawDisks->writeAt(
					superBlock->iNodeStartAddress + (child->id) * sizeof(INODE),
					child, sizeof(INODE));
			return true;
		}

		child->referenceCount--;
	
		if (child->referenceCount < 1)
			releaseDiskResource(child, child->references[0]);
		else
			rawDisks->writeAt(
					superBlock->iNodeStartAddress + (child->id) * sizeof(INODE),
					child, sizeof(INODE));
		rawDisks->writeAt(START_ADDRESS, superBlock, sizeof(SuperBlock));
		return true;
	}
public:
	int * getIndirectReference(int blockIndex) {

		int arr[ADDRESSABLE_DATA_BLOCKS - 1];

		arr[0] = 1;
		int depth = 0;
		int i = 1;
		int *t = (int *) malloc(sizeof(int) * (DEPTH_LEVEL + 1));

		for (i = 1; i < ADDRESSABLE_DATA_BLOCKS - 1; i++) {
			arr[i] = arr[i - 1] + (int) pow(ADDRESSABLE_DATA_BLOCKS, i);

		}
		for (int i = 0; i < ADDRESSABLE_DATA_BLOCKS - 1; i++) {
			if (blockIndex >= arr[i])
				depth++;
		}
		t[0] = depth;
		int t_index = blockIndex - arr[depth - 1];
		for (int i = depth; i > 0; i--) {

			t[depth - i + 1] = t_index
					/ (int) pow(ADDRESSABLE_DATA_BLOCKS, i - 1);
			t[depth - i + 2] = (int) t_index
					% (int) pow(ADDRESSABLE_DATA_BLOCKS, i - i);
			t_index = t_index % (int) pow(ADDRESSABLE_DATA_BLOCKS, i - 1);

		}

		return t;
	}

	int getNewBlock(P_INODE inode, int oldBlock, int newBlock) {
		int dId = -1;

		int max_block = 0;

		max_block = (int) (pow(ADDRESSABLE_DATA_BLOCKS, DEPTH_LEVEL) - 1)
				/ (ADDRESSABLE_DATA_BLOCKS - 1);
		if (newBlock == max_block)
			return dId;
		int * oldAddress = getIndirectReference(oldBlock);
		int * newAddress = getIndirectReference(newBlock);
		int oldBlockId = 0;

		if (oldAddress[0] != newAddress[0]) { //Different Depths;
			dId = getEmptyBlock();
			if (dId == EMPTY)
				return EMPTY;
			inode->references[newAddress[0]] = dId;
			oldBlockId = dId;
			consumeDataBlock(dId);

			rawDisks->writeAt(
					superBlock->iNodeStartAddress + (inode->id * sizeof(INODE)),
					inode, sizeof(INODE));
			for (int i = 1; i < newAddress[0] + 1; i++) {
				dId = getEmptyBlock();
				if (dId == EMPTY)
					return EMPTY;

				consumeDataBlock(dId);

				writeAtBlock(superBlock->dataBlockStartAddress + oldBlockId,
						&dId, sizeof(int), 0);
				oldBlockId = dId;
			}
			rawDisks->writeAt(
					superBlock->iNodeStartAddress + (inode->id * sizeof(INODE)),
					inode, sizeof(INODE));
			return dId;
		}
		oldBlockId = inode->references[oldAddress[0]];

		for (int i = 1; i < oldAddress[0] + 1; i++) {

			if (oldAddress[i] != newAddress[i]) {
				for (int j = i; j < oldAddress[0] + 1; j++) {
					dId = getEmptyBlock();
					if (dId == EMPTY)
						return EMPTY;

					consumeDataBlock(dId);

					writeAtBlock(superBlock->dataBlockStartAddress + oldBlockId,
							&dId, sizeof(int), newAddress[j] * sizeof(int));
					oldBlockId = dId;

				}
				return dId;

			}
			oldBlockId = ((int *) readBlock(
					superBlock->dataBlockStartAddress + oldBlockId, &oldBlockId,
					sizeof(int) * ADDRESSABLE_DATA_BLOCKS))[oldAddress[i]];

		}
		rawDisks->writeAt(START_ADDRESS, superBlock, sizeof(SuperBlock));
		return dId;

	}

};

#endif /* FILESYSTEM_H_ */
