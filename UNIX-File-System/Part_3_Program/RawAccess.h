/*
 * RawAccess.h
 *
 *  Created on: May 20, 2020
 */

#ifndef RAWACCESS_H_
#define RAWACCESS_H_

#define KILO 1024
#define MEGA 1024*1024
typedef long long ll;
typedef unsigned char BYTE;

#include<stdio.h>
#include<stdlib.h>
class RawDisks {
private:
	FILE *disk = NULL;
	ll diskSize;
public:

	RawDisks(ll diskSize) {
		this->diskSize = diskSize;
	}
	void createDisk(char *fileName) {

		disk = fopen(fileName, "w+");
		if (!disk) {
			printf("Error while creating the disk\n");
			exit(0);
		}
		rawFormat();
	}
	void rawFormat() {
		ll i = 0;
		while (i < diskSize) {
			fputc(0, disk);
			i++;
		}
	}

	void loadDisk(char *fileName) {
		disk = fopen(fileName, "r+");
		fseek(disk, 0, SEEK_END);
		this->diskSize = ftell(disk);

		if (!disk) {
			printf("Error while creating the disk\n");
			exit(0);
		}
		rewind(disk);
	}

	int writeAt(int diskPosition, void* stream, int length) {


		rewind(disk);
		fseek(disk, diskPosition, SEEK_SET);
		fwrite(stream, sizeof(BYTE), length, disk);
		fflush(disk);
		return 1;
	}

	BYTE * readFrom(int diskPosition, int length) {
		rewind(disk);

		fseek(disk, diskPosition, SEEK_SET);
		BYTE * buffer = (BYTE *) malloc(sizeof(BYTE) * length);
		fread(buffer, sizeof(BYTE), length, disk);
		//printf("%s\n",buffer);
		return buffer;
	}

	void close() {
		fflush(disk);
		fclose(disk);
	}

	ll getDiskSize() const {
		return diskSize;
	}
};

#endif /* RAWACCESS_H_ */
