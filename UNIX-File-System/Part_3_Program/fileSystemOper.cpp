#include <iostream>
#include<string.h>
#include<stdlib.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
using namespace std;
#include "RawAccess.h"
#include "FileSystem.h"

#include "RawAccess.h"
#include "SystemUtils.h"

string commands[10] = {"list", "mkdir", "rmdir", "dumpe2fs","write","read", "del","ln","lnsym","fsck"};

int main(int argc, char *argv[]) {

	int count=0;

	char *fileSystemFileName= argv[1]; //fileSystem.data
	char *operation= argv[2]; //operation

	int found=0;
	for (int i = 0; i < 10; i++){
	if(operation==commands[i])
		found=1;
	}
	if (!found) {
	cout<<"command not found:\n";
	exit(0);
	}
	//LOAD DISK
	RawDisks *disk = new RawDisks(1 * MEGA);
	disk->loadDisk(fileSystemFileName);

	FileSystem *fileSystem = new FileSystem(disk);
	fileSystem->readFileSystemMeta();
	//fileSystem->printFileSystemMeta();


	if(operation==commands[0]){//list
		if(argc!=4){
			cout<<"You entered wrong parameter for list operation!\n";
			exit(0);
		}
		char *folder = argv[3]; //parameters
		char folderArr[19] ;
		char parsed[10][50];
		strcpy(folderArr, folder);
		count = parsePath(folderArr, parsed);
		FileDescriptor *descirptor = fileSystem->getDirectoryInfo(parsed, count+1);
		if(descirptor != NULL)
			fileSystem->printFolderInformation(descirptor);
		else
			printf("\nDirectory does not exits!\n");

	}
	else if(operation==commands[1]){//mkdir ok
		if(argc!=4){
			cout<<"You entered wrong parameter for mkdir operation!\n";
			exit(0);
		}
		char *folder = argv[3]; //parameters
		char parsed[10][50];
		char folderNameArray[40];
		strcpy(folderNameArray, folder);//to pass char array to parsePath function
		count = parsePath(folderNameArray, parsed);
		fileSystem->createDirectory(parsed, count);
		count=0;
	}
	else if(operation==commands[2]){//rmdir ok
		if(argc!=4){
			cout<<"You entered wrong parameter for rmdir operation!\n";
			exit(0);
		}
		char *folder = argv[3]; //parameters
		char parsed[10][50];

		char folderNameArray[40];
		strcpy(folderNameArray, folder);
		count = parsePath(folderNameArray, parsed);
		fileSystem->removeDirectory(parsed, count);
		count=0;
	}
	else if(operation==commands[3]){//dumpe2fs ok
		if(argc!=3){
			cout<<"You entered wrong parameter for dumpe2fs operation!\n";
			exit(0);
		}
		fileSystem->printFileSystemMeta();
	}
	else if(operation==commands[4]){//write ok

		char *file=argv[3];
		char *linuxFile=argv[4];
		char fileName[50] ;
		char linuxFileName[50] ;
		strcpy(fileName, file);
		strcpy(linuxFileName, linuxFile);
		fileSystem->copyFromOtherFileSystem(
			fileName, linuxFileName);
		
	}
	else if(operation==commands[5]){//read ok
		
		char *file=argv[3];
		char *linuxFile=argv[4];
		char fileName[50] ;
		char linuxFileName[50] ;
		strcpy(fileName, file);
		strcpy(linuxFileName, linuxFile);

		fileSystem->copyIntoOtherFileSystem(fileName,
			linuxFileName);
	}
	else if(operation==commands[6]){//del ok
		if(argc!=4){
			cout<<"You entered wrong parameter for del operation!\n";
			exit(0);
		}
		char *file = argv[3];
		char fileNameArray[50]  ;
		char parsed[10][50];
		strcpy(fileNameArray, file);
		count = parsePath(fileNameArray, parsed);
		fileSystem->removeFile(parsed, count);
		count=0;

	}
	else if(operation==commands[7]){//ln
		char *fileName1=argv[3];
		char *fileName2=argv[4];
		char fileNameArr1[50] ;
		char fileNameArr2[50] ;

		strcpy(fileNameArr1, fileName1);
		strcpy(fileNameArr2, fileName2);

		fileSystem->createLink(fileNameArr2,fileNameArr1);
	}
	else if(operation==commands[8]){//lnsym


	}
	else if(operation==commands[9]){//fsck
		if(argc!=3){
			cout<<"You entered wrong parameter for fsck operation!\n";
			exit(0);
		}
		fileSystem->fsck_disk();
	}


	return 0;
}



