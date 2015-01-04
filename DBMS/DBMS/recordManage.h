#ifndef _RECORDMANAGE_H
#define _RECORDMANAGE_H
#include "data_utility.h"
#include "fileManage.h"
class DBStorage{
public:
	string dbname;
	UINT dbid;
	UINT filenum;
	DBStorage(string dbname, UINT dbid, bool isCreate);
	~DBStorage();
	void createTable(char* filename,char* databasename, tableAttr tableinfo);
	void createDataBase(char* databasename);
	void printFreeList(char* tablename, int pageid, int recordlength);
	char* getTablePath(char* tablename);
};

#endif