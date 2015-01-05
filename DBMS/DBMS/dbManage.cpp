#include "dbManage.h"

DBStorage::DBStorage(char* dbname, UINT dbid, bool isCreate) {
	strcpy(this->dbname, dbname);
	this->dbid = dbid;
	this->filenum = 0;
	if (isCreate)
		createDataBase(dbname);
}

DBStorage::~DBStorage() {
}

char* DBStorage::getName(){
	return dbname;
}
USRT DBStorage::getID(){
	return dbid;
}
USRT DBStorage::getFileNum(){
	return filenum;
}
char* DBStorage::getTablePath(char* tablename) {
	char* path = new char[strlen(tablename) + strlen(dbname) + 4];
	strcpy(path,"./");
	strcat(path,dbname);
	strcat(path,"/");
	strcat(path,tablename);
	strcat(path,"\0");
	return path;
}

void DBStorage::createTable(char* filename, char* databasename, tableAttr tableinfo) {
	dbPage* attrPage = new dbPage(++filenum);
	char* path = getTablePath(filename);
	memcpy(attrPage->data, &tableinfo, sizeof(tableAttr));
	FileBuffer::writePageToFile(attrPage->header.pageId, attrPage, path);
	delete attrPage;
	delete[] path;
}

void DBStorage::createDataBase(char* databasename) {
	_mkdir(databasename);
}

void DBStorage::dropDatabase(char* databasename){
	_rmdir(databasename);
}

void DBStorage::printFreeList(char* tablename, int pageid, int recordlength) {
	char* path = getTablePath(tablename);
	dbPage* pageInfo = new dbPage();
	FileBuffer::readPageFromFile(pageid, pageInfo, path);
	TYPE_ID offset = pageInfo->header.firstFreeOffset;
	while(offset != EXIST_INDEX) {
		cout << " " << offset << "--->";
		char* offsetValue = dataUtility::getbyte(pageInfo->data, offset + recordlength - SIZE_OFFSET, SIZE_OFFSET);
		offset = dataUtility::char2short(offsetValue);
	}
	cout << endl;
	delete pageInfo;
}