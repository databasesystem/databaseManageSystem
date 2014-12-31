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
	this->filenum = 0;
	if (isCreate)
		createDataBase((char*)dbname.c_str());
}

DBStorage::~DBStorage() {
}

char* DBStorage::getTablePath(char* tablename) {
	char* path = new char[strlen(tablename) + dbname.size() + 3];
	strcpy(path,"./");
	strcat(path,dbname.c_str());
	strcat(path,"/");
	strcat(path,tablename);
	return path;
}

void DBStorage::createTable(char* filename, char* databasename, attr tableinfo) {

	/*
	* for a simple example
	* CREATE TABLE publisher (id int(10) NOT NULL,
	* name varchar(100) NOT NULL,nation varchar(3),PRIMARY KEY  (id));
	*/
	cout << "************************Start Create Table**********************" << endl;
	char* path = getTablePath(filename);
	FILE* filestream;
	filestream = fopen(path,"w");
	fclose(filestream);
	dbPage test;
	test.header.pageId = 0;
	test.header.fileId = filenum+1;
	test.header.firstFreeOffset = sizeof(attr)+1;   // the offset for the first free room.
	test.header.freeCount = PAGE_SIZE-sizeof(attr); //the size of page free 
	test.header.rowCount =0;

	memcpy(test.data, &tableinfo, sizeof(attr));

	FileManage::writePageToFile(test.header.pageId, &test, path);
	this->filenum++;

	dbPage* result = new dbPage();
	FileManage::readPageFromFile(0, result, path);
	cout << "test page data " << result->header.fileId << endl;
	//cout<< result->data << endl;  //although appear tang, but read out is correct.  cout << char* << endl;

	/*dataUtility::printChars(result->data);
	attr* checkresult = dataUtility::char_to_class<attr>(result->data);
	cout << checkresult->colname[0] << endl;
	cout << checkresult->colname[1] << endl;
	cout << checkresult->colname[2] << endl;
	cout << checkresult->coltype[0] << endl;
	cout << checkresult->coltype[1] << endl;
	cout << checkresult->coltype[2] << endl;
	cout << checkresult->primaryId << endl;*/
	
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
	//cout << "Data length" << record.length << endl;
	
	dbPage* attrPageInfo = new dbPage();
	dbPage* pageInfo = new dbPage();
	char* path = getTablePath(tablename);

	FileManage::readPageFromFile(0, attrPageInfo, path);
	int fileid = attrPageInfo->header.fileId;
	attr* tableAttr = dataUtility::char_to_class<attr>(attrPageInfo->data);
	int pageid = 1;
	
	while(true) {
		cout << " 1page num: " << tableAttr->pagenum << endl;
		if (tableAttr->pagenum <= pageid)
		{
			pageInfo->header.fileId = fileid;
			pageInfo->header.firstFreeOffset = 0;
			pageInfo->header.freeCount = PAGE_SIZE;
			cout << "new page id: " << pageid << endl;

			tableAttr->pagenum++;
			cout << " 2page num: " << tableAttr->pagenum << endl;
			
			memcpy(attrPageInfo->data, tableAttr, sizeof(attr));
			FileManage::writePageToFile(0, attrPageInfo, path);
			FileManage::readPageFromFile(0, attrPageInfo, path);
			tableAttr = dataUtility::char_to_class<attr>(attrPageInfo->data);
			cout << " 3pagenum: "  << tableAttr->pagenum << endl;
			break;
		} else {
			FileManage::readPageFromFile(pageid, pageInfo, path);
			if (pageInfo->header.freeCount >= record.length)
			{
				cout << "exist page id: " << pageid << endl;
				break;
			} else {
				cout << "into the next page" << endl;
				pageid++;
				FileManage::readPageFromFile(pageid, pageInfo, path);
			}
		}
	}
	char* firstOffset = dataUtility::getbyte(pageInfo->data, pageInfo->header.firstFreeOffset+record.length-sizeof(int), sizeof(int));
	int firstFreeOffset = *(int*)firstOffset;
	delete firstOffset;
	if (firstFreeOffset == 0) {
		pageInfo->header.firstFreeOffset += record.length;
	} else {
		pageInfo->header.firstFreeOffset = firstFreeOffset;
		char* secondOffset = dataUtility::getbyte(pageInfo->data, firstFreeOffset+record.length-sizeof(int), sizeof(int));
		record.offset = *(int*)secondOffset;
	}
	pageInfo->header.freeCount -= record.length;
	cout << "fileid " << pageInfo->header.fileId << endl;
	dbPage test;
	test.header.fileId = 2;
	FileManage::writePageToFile(pageid, &test, path);
	dbPage* readtest = new dbPage();
	FileManage::readPageFromFile(pageid, readtest, path);
	cout << readtest->header.fileId << endl;
	cout << "************************End Insert Data**********************" << endl;
}

