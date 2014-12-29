#ifndef _DATABASEMANAGE_H
#define _DATABASEMANAGE_H

#include "pageManage.h"

#define TABLE_NUM_SCALE 100
#define COLUMN_NUM_SCALE 10000
#define ID_NUM_SCALE (COLUMN_NUM_SCALE+2*TABLE_NUM_SCALE)
#include <vector>
#include <fstream>
#include <iostream>
#include <map>
using namespace std;

typedef pair<string, int> NameIndexPair;
typedef pair<NameIndexPair, int> TableIndexPair;

class SysObject{
public:
	SysObject(int id,string name,char xtype = 0,
		char fixCnt = 0, char varCnt = 0 ):id(id),name(name),xtype(xtype)
		,fixCnt(fixCnt),varCnt(varCnt),varStart(NULL){}
	~SysObject(){if (varStart!=NULL)delete[] varStart;}
	int id;		// table id
	string name;	// table name
	char xtype;
	vector<int> vecCols;
	char fixCnt;
	char varCnt;
	char *varStart;
};

class SysColumn{
public:
	SysColumn(int id,int colid,string name,char xtype,int length,
		bool n,char index):id(id),colid(colid),name(name),xtype(xtype)
		,length(length),nullable(n),index(index){}
	int id;			// table id
	int colid;		// column id
	char index;		// relative offset in table
	string name;	// column name
	char xtype;		// data type
	int length;		// the max length of this type
	bool nullable;
};

class SysIndex{
public:
	SysIndex(int id,string name,int rows,int first,char indid,int root,int dpages,int reserved,int FirstIAM)
		:id(id),name(name),rows(rows),first(first),indid(indid),root(root),dpages(dpages),reserved(reserved),FirstIAM(FirstIAM){}
	int id;			// table id it belongs to
	string name;	// name of this index
	int rows;		// count of the rows
	int first;		// the location of first index page
	char indid;		// 1 : clustered index, 2-250 : no-clustered index
	int root;		// location of the root page
	int dpages;		// how many pages it takes
	int reserved;	// a reserved field
	int FirstIAM;	// location of the first IAM page
};

class Database
{
public:
	Database(void);
	~Database(void);

	bool createTable(string name,int uid);
	SysObject* findTable(string name);
	bool dropTable(string name);

	bool insertColumn(string name,string tableName,char xtype,int length,bool nullable);
	SysColumn* findColumn(string name,string tablename);

	bool createIndex(string name,int indid);
	SysIndex* findIndex(string name);
	void print();
	void flush();

	SysObject *m_pSysobjects[TABLE_NUM_SCALE];
	SysIndex *m_pSysindexes[TABLE_NUM_SCALE];
	SysColumn *m_pSyscolumns[COLUMN_NUM_SCALE];

private:
	int getNewID();
	int m_nSysobjects,m_nSysindexes,m_nSyscolumns;
	int idPool[ID_NUM_SCALE];
	ofstream fout;
	ifstream fin;
	map<string,int> sysobjMap,sysindexMap;
	map<NameIndexPair,int> syscolMap;
};


class attr{
public:
		int num;
		string *colname;
		int* coltype; //0-int 1-double 2-char
		int* collen;
		bool* colIsNull;
		int primaryId;
		attr(int num_v){
			num = num_v;
			colname = new string[num];
			coltype = new int[num];
			collen = new int[num];
			colIsNull = new bool[num];
		}
};
#endif