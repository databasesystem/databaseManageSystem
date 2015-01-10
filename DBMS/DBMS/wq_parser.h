#ifndef WQPARSER_H
#define WQPARSER_H
#include "pageManage.h"
#include "dbManage.h"
#include <fstream>
#include <string.h>
#include <vector>
#include <iostream>

using namespace std;


#define ENDOFCOMMAND 0
#define ENDOFFILE 1
#define SELECT 2
#define FROM 3
#define WHERE 4

#define OP_AND 5
#define OP_OR 6
#define EQUAL 7
#define LESS 8
#define MORE 9
#define NOTEQUAL 10

#define UPDATE 11
#define SET 12
#define DROP 13
#define TABLE 14
#define DELETE 15
/*
#define LIKE 16
#define IS 17
*/
#define CREATE 18
/*
#define VARCHAR 19
#define NORMALCHAR 20
#define NORMALINT 21
*/
#define ISNULL 22
#define NOTNULL 23
#define NUMBER 24
#define INSERT 25
#define ISINTO 26
#define VALUES 27
#define SELECT_STAR 28

#define LESSEQUAL 28
#define MOREEQUAL 29

#define INT 30
#define VARCHAR 31

#define IS_NOT 32
#define IS_KEY 33
#define IS_PRIMARY 34
#define DESC 35
#define SHOW 36
#define USE 37
#define TABLES 38
#define DATABASE 39
#define IS 40




class parser{
public:
	parser(string dbname);
	~parser();
	void testParse();
	void BatchSqlInFile(char* filename);
	void splitStr(char* str, vector<string>* res);
	bool parserOneCommand(vector<string> commands);
	bool parserCreate(vector<string> commands);
	bool parserDrop(vector<string> commands);
	bool parserDesc(vector<string> commands);
	bool parserDelete(vector<string> commands);
	bool parserShowTable(vector<string> commands);
	bool parserCreateColumn(vector<string> columnInfo, tableColumn* columnInfos);
	bool parserUse(vector<string> commands);
	bool parserInsert(vector<string> commands);
	bool parserWhere(vector<string> commands, string tablename);
	bool checkNameAvaliable(string s);
	bool isCmp(char c);
	bool isEnglishAlphabet(char c);
	bool isOpt(string s);
	bool isDig(char c);
	int getType(string s);
	bool checkStingIsInt(string s);
	string getKeyWords(int keyvalue);
	bool checkKeyWord(string s, int keyvalue);
	bool checkColumnsValue(vector<SysColumn*> sysColumns, vector<string> datas);
	bool checkOneColumnValue(SysColumn sysColumn, string data);
private:
	DBManager* currentDb;
	BYTE **Value;
	string *colName;
	BYTE *type;
	BYTE *len;
	BYTE *op;
	
};
#endif