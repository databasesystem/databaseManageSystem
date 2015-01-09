#ifndef _DBMANAGE_H
#define _DBMANAGE_H
#include "systemManage.h"
#include "pageManage.h"
#include <fstream>
#include <direct.h>
#include <vector>
using namespace std;

class DBManager{
public:
	DBManager(string dbname);
	~DBManager();
	void flush();
	//System manage
	void createDataBase(string databaseName);
	bool switchDataBase(string databaseName);
	void dropDataBase(string databaseName);
	void createTable(string tablename, UINT colNum, string colName[], BYTE type[], TYPE_OFFSET length[], bool nullable[]);
	void dropTable(string tableName);
	//Database manage
	bool insertRecord(RecordEntry *input,string colName[], string tableName);
	bool updateRecord(string tableName,BYTE **Value,string *colName,BYTE *type,BYTE *len,BYTE *op, BYTE condCnt);
	bool deleteRecord(string tableName,BYTE **Value,string *colName,BYTE *type,BYTE *len,BYTE *op, BYTE condCnt);
	//Data retrieval
	string *getColName(string tableName,USRT &colNum);
	vector<RecordEntry*> findRecord(string tableName,BYTE **Value,string *colName, BYTE *type, BYTE *len,BYTE *op, BYTE condCnt);
	void combine(vector<RecordEntry*> &result, const vector<RecordEntry*> &A, const vector<RecordEntry*> &B,
		BYTE colA,BYTE colB,BYTE tarAIndex);
	//print
	void printRecord(string tableName,BYTE colnum,string *colName,TYPE_OFFSET offset,TYPE_ID pageid);
	void printDatabase();
	void printTable(string tableName);
	void printTables();
	vector<SysColumn*> getTableAttr(string tableName);
	
private:
	string dbName;
	map<rowID, Node*> bufmap;
	SysManager sysManager;
	string getTablePath(string tablename);
	//buffer manage
	void pop();
	void push(TYPE_ID FileID, TYPE_ID PageID, Node* buffer);
	Node* findPage(TYPE_ID FileID, TYPE_ID PageID);
	Node* readPage(TYPE_ID FileID, TYPE_ID pageID);
	bool insertData(SysObject* table, char record[], int index);
	bool deleteData(SysObject* table, TYPE_ID pageid, TYPE_OFFSET offset, int index);
	//File IO
	void writePageToFile(TYPE_ID pageid, Page* pagedata, string tablename);
	bool readPageFromFile(TYPE_ID pageid, Page* pagedata, string tablename);


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