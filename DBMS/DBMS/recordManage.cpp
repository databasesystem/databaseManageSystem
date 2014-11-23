#include "recordManage.h"
#include "fileManage.h"
#include "data_utility.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>
using namespace std;
void createTable(char* filename, char* databasename) {
	/*
	* for a simple example
	* CREATE TABLE publisher (id int(10) NOT NULL,
	* name varchar(100) NOT NULL,nation varchar(3),PRIMARY KEY  (id));
	*/
	attr tableAttr(3);
	tableAttr.coltype[0] = 0;
	tableAttr.coltype[1] = 5;
	tableAttr.coltype[2] = 5;

	tableAttr.collen[0] = 4;  //int(10) 10 is the max show length
	tableAttr.collen[1] = 100;
	tableAttr.collen[2] = 3;

	tableAttr.colname[0] = "id";
	tableAttr.colname[1] = "name";
	tableAttr.colname[2] = "nation";

	tableAttr.colIsNull[0] = false;
	tableAttr.colIsNull[1] = false;
	tableAttr.colIsNull[2] = true;

	char* attrdata = dataUtility::data_to_char<attr>(tableAttr);
	

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
	test.header.pageId = 0;
	test.header.fileId = 0;
	test.header.firstFreeOffset = sizeof(tableAttr)+1;
	test.header.freeCount = 0;
	test.header.rowCount =0;

	memcpy(test.data, attrdata, sizeof(tableAttr));
	test.data[PAGE_SIZE-1]='\0';
	FileManage::writePageToFile(0, test, path);
	dbPage* result = new dbPage();
	FileManage::readPageFromFile(0, result, path);
	cout<< result->data << endl;  //although appear tang, but read out is correct.

	attr* checkresult = dataUtility::char_to_class<attr>(result->data);
	cout << checkresult->colname[0] << endl;
}

void createDataBase(char* databasename) {
	FileManage::createFileFolder(databasename);
}