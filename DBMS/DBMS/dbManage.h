#ifndef _RECORDMANAGE_H
#define _RECORDMANAGE_H
#include "pageManage.h"
#include <vector>
using namespace std;

#define INDEX_SYSOBJECT 0
#define INDEX_SYSINDEX	1
#define INDEX_SYSCOLUMN 2

class DBManager{
public:
	DBManager(string dbname);
	~DBManager();
	void createDataBase(string databaseName);
	void switchDataBase(string databaseName);
	void dropDataBase(string databaseName);

	void createTable(string tablename, UINT colNum, string colName[], BYTE type[], TYPE_OFFSET length[], bool nullable[]);
	void dropTable(string tableName);
	void printFreeList(string tablename, int pageid, int recordlength);

	void insertRecord(RecordEntry *input,string colName[],string tableName);
	void deleteRecord(string tableName,BYTE **Value,string *colName,BYTE *type,BYTE *len,BYTE *op, BYTE condCnt);
	vector<RecordEntry*> findRecord(string tableName,BYTE **Value,string *colName, BYTE *type, BYTE *len,BYTE *op, BYTE condCnt);
	string *getColName(string tableName,USRT &colNum);
	void combine(vector<RecordEntry*> &result, const vector<RecordEntry*> &A, const vector<RecordEntry*> &B,
		BYTE colA,BYTE colB,BYTE tarAIndex);

	void printRecord(ofstream &fout,RecordEntry* input,BYTE colnum,
		string tableName,string *prtCol,BYTE prtNum);
	void printRecord(ofstream &fout,RecordEntry* input,bool *tablePos,string *tableName,string *colName,
		BYTE offset,BYTE colnum);
	void flushBuffer();
	void print();
	const char* getTablePath(string tablename);
	
private:
	string dbName;
	BufManager bufManager;
	SysManager sysManager;
	BYTE *makeRecord(RecordEntry* input,string colName[],string tableName,UINT &totalLength);
	RecordEntry *getRecord(BYTE *data,string tableName);
	void findInPage(string tname,Page *page,BYTE **Value,BYTE *len,string *colName,BYTE *type,
		vector<RecordEntry*> &q,BYTE *op,BYTE condCnt);
	void delInPage(string tname,Page *page,BYTE **Value,BYTE *len,string *colName,BYTE *type,
		BYTE *op,BYTE condCnt);

	//BYTE *getKeyValue(string tableName,string colName,BYTE *data,USRT &length);
	// any key value is compared in their byte order
	//bool compare(BYTE **data1,BYTE **data2,BYTE *len,BYTE *op,BYTE *type,BYTE colNum);
	// compare single data
	//BYTE compareSingle(BYTE *data1,BYTE *data2,BYTE len);
};
#endif