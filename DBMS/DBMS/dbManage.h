#ifndef _RECORDMANAGE_H
#define _RECORDMANAGE_H
#include "data_utility.h"
#include "pageManage.h"
#include <direct.h>
using namespace std;

class DBStorage{
public:
	DBStorage(char* dbname, UINT dbid, bool isCreate);
	~DBStorage();
	char* getName();
	TYPE_ID getID();
	TYPE_ID getFileNum();
	void createTable(char* filename,char* databasename, tableAttr tableinfo);
	void createDataBase(char* databasename);
	void dropTable();
	void dropDatabase(char* databasename);
	void printFreeList(char* tablename, int pageid, int recordlength);
private:
	char dbname[30];
	TYPE_ID dbid;
	TYPE_ID filenum;
	char*getTablePath(char* tablename);
};

#endif