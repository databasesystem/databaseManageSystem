#ifndef WQPARSER_H
#define WQPARSER_H
#include "pageManage.h"
#include "dbManage.h"
#include "globalVariable.h"
#include <fstream>
#include <string.h>
#include <vector>
#include <iostream>

using namespace std;


struct columnRequire {
	string value;
	string colName;
	BYTE type;
	BYTE len;
	BYTE op;
	columnRequire(string value_v, string colName_v, BYTE type_v, BYTE len_v, BYTE op_v) {
		cout << "Requirement: " << value << " " << colName_v << " " << (int)type_v << " " << (int)len_v << " " << (int)op_v << endl;
		value = value_v;
		len = value.length();
		colName = colName_v;
		type = type_v;
		op = op_v;
		cout << "Value: " << value << endl;
	}
};
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
	int getOpt(string s);
	bool checkStingIsInt(string s);
	string getKeyWords(int keyvalue);
	
	bool checkKeyWord(string s, int keyvalue);
	bool checkColumnsValue(vector<SysColumn*> sysColumns, vector<string> datas);
	bool checkOneColumnValue(SysColumn sysColumn, string data);
private:
	DBManager* currentDb;
	vector<columnRequire> table1Require;
	vector<columnRequire> table2Require;
};
#endif