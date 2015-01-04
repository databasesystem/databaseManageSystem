#include "recordManage.h"

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
	char* path = new char[strlen(tablename) + dbname.size() + 4];
	strcpy(path,"./");
	strcat(path,dbname.c_str());
	strcat(path,"/");
	strcat(path,tablename);
	strcat(path,"\0");
	return path;
}

void DBStorage::createTable(char* filename, char* databasename, tableAttr tableinfo) {

	/*
	* for a simple example
	* CREATE TABLE publisher (id int(10) NOT NULL,
	* name varchar(100) NOT NULL,nation varchar(3),PRIMARY KEY  (id));
	*/
	cout << "************************Start Create Table**********************" << endl;
	dbPage test;
	test.header.pageId = 0;
	test.header.fileId = filenum+1;
	test.header.firstFreeOffset = sizeof(tableAttr);   // the offset for the first free room (no need to +1). 
	test.header.freeCount = PAGE_SIZE-sizeof(tableAttr); //the size of page free 
	test.header.rowCount =0;
	
	char* path = getTablePath(filename);
	memcpy(test.data, &tableinfo, sizeof(tableAttr));

	FileManage::writePageToFile(test.header.pageId, &test, path);
	this->filenum++;
	delete[] path;
	cout << "************************End Create Table**********************" << endl;
}

void DBStorage::createDataBase(char* databasename) {
	FileManage::createFileFolder(databasename);
}

void DBStorage::printFreeList(char* tablename, int pageid, int recordlength) {
	char* path = getTablePath(tablename);
	dbPage* pageInfo = new dbPage();
	FileManage::readPageFromFile(pageid, pageInfo, path);
	int offset = pageInfo->header.firstFreeOffset;
	while(offset != -1) {
		cout << " " << offset << "--->";
		char* offsetValue = dataUtility::getbyte(pageInfo->data, offset + recordlength-sizeof(int), sizeof(int));
		int* temp = dataUtility::char_to_int(offsetValue);
		offset = *temp;
	}
	cout << endl;
	delete pageInfo;
}