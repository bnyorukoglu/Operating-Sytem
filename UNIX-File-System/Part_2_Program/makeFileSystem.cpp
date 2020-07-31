#include <iostream>
#include<string.h>
#include<stdlib.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
using namespace std;
#include "RawAccess.h"
#include "FileSystem.h"


int main(int argc, char *argv[]) {

    cout<< "\n## WELCOME TO THE FILE SYSTEM ##"<<endl;
    cout<< "## PLEASE ENTER THE FOLLOWING COMMAND ##"<<endl;
    

    if(argc!=4) {
            cout<<"\n Invalid number of argument !\n"<<endl;
	    cout<< "~makeFileSystem - To initialize the file system"<<endl;
    	    cout<< "~Example :: ./makeFileSystem [blockSize] [numberOfFreeInodes] [fileName]"<<endl;
    	    cout<< "~Example :: ./makeFileSystem 4 400 mySystem.dat"<<endl;
            exit(0);
    }

    char *operation = argv[0];
    int blockSize= atoi(argv[1]);

    int freeInodeNumber= atoi(argv[2]);
    char *fileName = argv[3];
    	 
    bool create = true;
    RawDisks *disk = new RawDisks(1 * MEGA);
    
    disk->createDisk(fileName);
    
    FileSystem *fileSystem = new FileSystem(disk);
    if (create)
 	fileSystem->formatDisk(blockSize * KILO, freeInodeNumber);
    if (!create)
	fileSystem->readFileSystemMeta();

    fileSystem->printFileSystemMeta();
   return 0;
}


