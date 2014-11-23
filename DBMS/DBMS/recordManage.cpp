#include "recordManage.h"
#include "fileManage.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>
using namespace std;
void createTable(char* filename, char* databasename) {
	char* path = (char*) malloc(strlen(filename) + strlen(databasename)+4);
	path[0] = '.';
	path[1] = '/';
	for(int i = 0; i < strlen(databasename); i++)
		path[2+i] = databasename[i];
	path[2+strlen(databasename)]='/';
	for(int i = 0; i < strlen(filename); i++)
		path[3+strlen(databasename)+i] = filename[i];
	path[3+strlen(databasename)+strlen(filename)]='\0';
	FILE* filestream;
	filestream = fopen(path,"w");
	fclose(filestream);
	dbPage test;
	test.header.fileId = 0;
	test.header.firstFreeOffset = 0;
	test.header.freeCount = 100;
	test.header.rowCount =2;
	for(int i = 0; i < PAGE_SIZE; i++)
	{
		test.data[i]='a';
	}
	test.data[PAGE_SIZE-1]='\0';
	FileManage::writePageToFile(0, test, path);
	dbPage* result = new dbPage();
	FileManage::readPageFromFile(0, result, path);
	cout<< result->header.freeCount << endl;  //although appear tang, but read out is correct.
}

void createDataBase(char* databasename) {
	FileManage::createFileFolder(databasename);
}