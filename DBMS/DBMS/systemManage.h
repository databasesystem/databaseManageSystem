#ifndef _SYSMANAGER_H_
#define _SYSMANAGER_H_

#include "globalVariable.h"
#include "data_utility.h"
#include <map>
#include <vector>
#include <algorithm>
using namespace std;

// for name to index mapping
typedef pair<string,TYPE_ID> NameIndexPair;

// for (column name,table id) to column index
typedef pair<NameIndexPair,TYPE_ID> TableIndexPair;

class SysObject{
public:
	SysObject(TYPE_ID id,string name):id(id),name(name){}
	~SysObject(){}
	TYPE_ID id;		// table id
	string name;	// table name
	vector<TYPE_ID> vecCols;
};

class SysColumn{
public:
	SysColumn(TYPE_ID id,TYPE_ID colid,string name,	bool n, BYTE xtype,
		TYPE_OFFSET length,TYPE_OFFSET index):id(id),colid(colid),name(name),
		xtype(xtype),length(length),nullable(n),index(index){}
	TYPE_ID id;		// table id
	TYPE_ID colid;		// column id
	string name;	// column name
	bool nullable;
	BYTE xtype;		// data type
	TYPE_OFFSET length;	// the max length of this type
	TYPE_OFFSET index;		// relative offset in table
};

/*class SysIndex{
public:
	SysIndex(UINT id,string name,UINT rows,UINT first,BYTE indid,UINT root,UINT dpages,UINT reserved,UINT FirstIAM)
		:id(id),name(name),rows(rows),first(first),indid(indid),root(root),dpages(dpages),reserved(reserved),FirstIAM(FirstIAM){}
	UINT id;		// table id it belongs to
	string name;	// name of this index
	UINT rows;		// count of the rows
	UINT first;		// the location of first index page
	BYTE indid;		// 1 : clustered index, 2-250 : no-clustered index
	UINT root;		// location of the root page
	UINT dpages;	// how many pages it takes
	UINT reserved;	// a reserved field
	UINT FirstIAM;	// location of the first IAM page
};*/

class SysManager
{
public:
	SysManager();
	~SysManager();
	void loadSysfile();

	bool createTable(string name);
	SysObject *findTable(string name);
	string findTableName(TYPE_ID tableID);
	bool dropTable(string name);

	bool insertColumn(string name,string tableName,BYTE xtype,TYPE_OFFSET length,bool nullable,TYPE_OFFSET index);
	SysColumn *findColumn(string name,string tablename);
	SysColumn *findColumn(TYPE_ID colID);
	TYPE_OFFSET getRecordLength(string tableName);

	//bool createIndex(string name,UINT indid);
	//SysIndex* findIndex(string name);
	void setName(string dbName);
	vector<string> getAllTable();
	void print();
	void printTable(string tableName);
	void printTables(string dbName);
	void flush();
	void drop();

	//SysIndex  *m_pSysindexes[TABLE_NUM_SCALE];

private:
	string path;
	bool idPool[ID_NUM_SCALE];
	SysObject *m_pSysobjects[TABLE_NUM_SCALE];
	SysColumn *m_pSyscolumns[COLUMN_NUM_SCALE];
	UINT m_nSysobjects,m_nSysindexes,m_nSyscolumns;
	map<string,TYPE_ID> sysobjMap;//,sysindexMap;
	map<NameIndexPair,TYPE_ID> syscolMap;
	TYPE_ID getNewID();
};
#endif