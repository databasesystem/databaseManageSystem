#include "recordManage.h"
#include "fileManage.h"
#include "data_utility.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>
using namespace std;

DBStorage::DBStorage(string dbname, UINT dbid, bool isCreate) {
	this->dbname = dbname;
	this->dbid = dbid;
	if (isCreate)
		createDataBase((char*)dbname.c_str());
}

DBStorage::~DBStorage() {
}

void DBStorage::createTable(char* filename, char* databasename, attr tableinfo) {

	/*
	* for a simple example
	* CREATE TABLE publisher (id int(10) NOT NULL,
	* name varchar(100) NOT NULL,nation varchar(3),PRIMARY KEY  (id));
	*/
	cout << "************************Start Create Table**********************" << endl;
	char* attrdata = dataUtility::data_to_char<attr>(tableinfo);
	

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
	test.header.firstFreeOffset = sizeof(tableinfo)+1;
	test.header.freeCount = PAGE_SIZE-sizeof(tableinfo); //the size of page free 
	test.header.rowCount =0;

	memcpy(test.data, attrdata, sizeof(tableinfo));
	test.data[PAGE_SIZE-1]='\0';

	FileManage::writePageToFile(0, test, path);
	dbPage* result = new dbPage();
	FileManage::readPageFromFile(0, result, path);
	cout<< result->data << endl;  //although appear tang, but read out is correct.  cout << char* << endl;

	dataUtility::printChars(result->data);
	attr* checkresult = dataUtility::char_to_class<attr>(result->data);
	cout << checkresult->colname[0] << endl;
	cout << checkresult->colname[1] << endl;
	cout << checkresult->colname[2] << endl;

	cout << checkresult->coltype[0] << endl;
	cout << checkresult->coltype[1] << endl;
	cout << checkresult->coltype[2] << endl;

	cout << checkresult->primaryId << endl;
	
	cout << "************************End Create Table**********************" << endl;
}

void DBStorage::createDataBase(char* databasename) {
	FileManage::createFileFolder(databasename);
}

void DBStorage::insertData(char* tablename, recordEntry record) {
	/*
	*INSERT INTO publisher VALUES 
	*(100008,'Oxbow Books Limited','PRC');
	*/

	cout << "************************Start Insert Data**********************" << endl;
	cout << "insertData--record the first column data: " << record.item[0] << endl;
	char* data = record.getRecord(&record);
	

	/*char* data = dataUtility::data_to_char<int>(123);
	char* testone = dataUtility::getbyte(data, 0, 4);
	int* t = dataUtility::char_to_int(testone);
	cout << *t<< endl;
	cout << "ok" << endl;*/
	cout << "************************End Insert Data**********************" << endl;
}