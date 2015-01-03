#ifndef _RECORDMANAGE_H
#define _RECORDMANAGE_H
#include "data_utility.h"
#include "fileManage.h"
#include "bufferManage.h"
class DBStorage{
public:
	string dbname;
	UINT dbid;
	UINT filenum;
	DBStorage(string dbname, UINT dbid, bool isCreate);
	~DBStorage();
	void createTable(char* filename,char* databasename, attr tableinfo);
	void createDataBase(char* databasename);
	void insertData(char* tablename, recordEntry record);
	void deleteData(char* tablename, int pageid, int offset, int recordlength);
	void updateData(char* tablename, int pageid, int offset, recordEntry record);
	void printFreeList(char* tablename, int pageid, int recordlength);
	void searchData(char* tablename);
	char* getTablePath(char* tablename);
};

#endif