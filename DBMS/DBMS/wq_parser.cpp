#include "wq_parser.h"
#include "pageManage.h"
#include "globalVariable.h"
#include "dbManage.h"


parser::parser(string dbname) {
	currentDb = new DBManager(dbname);
}
parser::~parser() {
	delete currentDb;
}
void parser::testParse(){
	char filename[100];
	while(true) {
		printf("please input the command filename. If you want to quit, please input quit.\n");
		scanf("%s", filename);
		if (strcmp(filename, "quit") == 0)
			break;
		else
			BatchSqlInFile(filename);
	}
}

void parser::BatchSqlInFile(char* filename) {
	FILE* fin = fopen(filename, "r");
	if (fin == NULL)
	{
		printf("This file does not exist.\n");
		return;
	}
	char* command = new char[1000];
	vector<string> res;
	while(true) {
		fgets(command, 1000,fin);
		//cout  << command << endl;
		splitStr(command, &res);
		//semicolon has deleted
		if (res.size() > 0 && res[res.size()-1].at(res[res.size()-1].length()-1) == ';') {
			//This is one command;
			res[res.size()-1].erase(res[res.size()-1].length()-1);
			if (!parserOneCommand(res))
				cout << "This command is wrong." << endl;
			res.clear();
		}
		if (feof(fin))
			break;
	}
}


void parser::splitStr(char* str, vector<string>* res){
	string temp = ""; //ever a error, because I make it equal 0. 
	int index = 0;
	if (strlen(str) == 0 || str[0] == '\n')
		return;
	while (str[index] ==  ' ')  {
		index++;
		if (index >= strlen(str) || str[index] == '\n')
			return;
	}
	//from the first index is not ' ', all examples is '
	while (str[index] != ' ') {
		if (str[index] == '\'') {
			temp += str[index];
			index++;
			while (index < strlen(str) && str[index] != '\'') {
				temp += str[index];
				index++;
			}
		}
		temp += str[index];
		index++;
		if (index < strlen(str) && str[index] == ' ' && temp.length()!=0) {
			(*res).push_back(temp);
			temp ="";
		}
		while (str[index] ==  ' ')  index++;
		if (index >= strlen(str) || str[index] == '\n'|| str[index] == '\r')
			break;
	}
	if (temp.length() !=0 )
		(*res).push_back(temp);
}

bool parser::parserOneCommand(vector<string> commands) {
	// if return false, prove this command is wrong.
	//cout << "Parser one command*****************************" << endl;
	if (commands.size() == 0)
		return false;
	if (checkKeyWord(commands[0], CREATE)) {
		return parserCreate(commands);
	}
	else if (checkKeyWord(commands[0], USE)) {
		return parserUse(commands);
	}
	else if (checkKeyWord(commands[0], INSERT)) {
		return parserInsert(commands);
	}
	else if (checkKeyWord(commands[0], DROP)) {
		return parserDrop(commands);
	}
	else if (checkKeyWord(commands[0], SHOW)) {
		return parserShowTable(commands);
	} 
	else if (checkKeyWord(commands[0], DESC)){
		return parserDesc(commands);
	}
	else if (checkKeyWord(commands[0], DELETE)) {
		return parserDelete(commands); 
	}
	else if (checkKeyWord(commands[0], UPDATE)) {
		return parserUpdate(commands);
	}
	else if (checkKeyWord(commands[0], SELECT)) {
		return parserSelect(commands);
	}
	return true;
}
bool parser::parserSelect(vector<string> commands){
	vector<string> tables;
	vector<SysColumn*> sysColumns;
	bool flag = false;
	for (int i = 0 ; i < commands.size(); i ++) {
		if (checkKeyWord(commands[i], WHERE))
			break;
		if (flag == true)
			tables.push_back(commands[i]);	
		if (checkKeyWord(commands[i], FROM)) {
			flag = true;		
		}
	}
	if (tables.size() == 1)
	{
		sysColumns = currentDb->getTableAttr(tables[0]);
		if (sysColumns.size() == 0)   // without this table
				return false;
		else {
			table1Name = tables[0];
			return parserOneTableSelect(commands);
		}
	} else if (tables.size() == 3) {
		if (tables[1].compare(",")!=0)
			return false;
		vector<SysColumn*> sysColumns = currentDb->getTableAttr(tables[0]);
		if (sysColumns.size() == 0)   // without this table
				return false;
		sysColumns = currentDb->getTableAttr(tables[2]);
		if (sysColumns.size() == 0)   // without this table
				return false;
		else {
			table1Name = tables[0];
			table2Name = tables[2];
			return parserTwoTableSelect(commands);
		}
	} else
		return true;
}
bool parser::parserOneTableSelect(vector<string> commands) {
	bool columnFlag = true;
	bool whereFlag = false;
	vector<string> showColumns;
	vector<string> whereCommands;
	vector<SysColumn*> sysColumns = currentDb->getTableAttr(table1Name);
	string tableNameTemp;
	string colNameTemp;
	int index;
	table1ShowColumn.clear();
	for(int i = 1; i < commands.size(); i++) {
		if (checkKeyWord(commands[i], FROM))
			columnFlag = false;
		if (columnFlag == true)
			showColumns.push_back(commands[i]);
		if (whereFlag == true)
			whereCommands.push_back(commands[i]);
		if (checkKeyWord(commands[i], WHERE))
			whereFlag = true;
	}
	if (showColumns.size() <= 0 )
		return false;
	if (showColumns[0].compare("*")==0)
	{
		if (showColumns.size() != 1)
			return false;
		for (int i =0 ;i < sysColumns.size(); i++)
			table1ShowColumn.push_back(sysColumns[i]->name);
	} else {
		for (int i = 0; i < showColumns.size(); i++) {
			tableNameTemp = "";
			colNameTemp = "";
			index = showColumns[i].find('.');
			if (index != -1) {
				tableNameTemp.assign(showColumns[i], 0, index);
				colNameTemp.assign(showColumns[i], index+1, showColumns[i].length()-index-1);
				if (table1Name.compare(tableNameTemp) != 0)
					return false;
			} else
				colNameTemp.assign(showColumns[i]);
			if (!currentDb->checkTableColumn(table1Name, colNameTemp))
				return false;
			table1ShowColumn.push_back(colNameTemp);
			i++;
			if (i < showColumns.size()) {
				if (showColumns[i].compare(",") == 0)
					continue;
				else
					return false;
			}
		}
	}
	table1Require.clear();
	parserWhere(whereCommands, table1Name);
	int Num = table1Require.size();
	BYTE	**value_v = new BYTE*[Num];
	string *colName_v = new string[Num];
	BYTE *type = new BYTE[Num];
	BYTE *len = new BYTE[Num];
	BYTE *op = new BYTE[Num];
	for (int i = 0; i < Num; i++){
		colName_v[i] = table1Require[i].colName;
		type[i] = table1Require[i].type;
		len[i] = table1Require[i].len;
		op[i] = table1Require[i].op;
		value_v[i] = new BYTE[len[i]];
		dataUtility::string_to_char((char*)value_v[i], table1Require[i].value, 0, len[i],len[i]);
	}
	string *showColumnNames = new string[table1ShowColumn.size()];
	for (int i = 0; i < table1ShowColumn.size(); i++)
		showColumnNames[i] = table1ShowColumn[i];
	currentDb->findRecord(table1Name, value_v, colName_v, type,len, op,Num,  showColumnNames, table1ShowColumn.size());  //if condCnt=-1,delete all
	delete[] showColumnNames;
	delete[] type;
	delete[] len;
	delete[] op;
	delete[] colName_v;
	for (int i = 0 ; i < Num; i++)
		delete[] value_v[i];
	delete[] value_v;
	return true;
}
bool parser::parserTwoTableSelect(vector<string> commands) {
	bool columnFlag = true;
	bool whereFlag = false;
	vector<string> showColumns;
	vector<string> whereCommands;
	vector<SysColumn*> sysColumns1 = currentDb->getTableAttr(table1Name);
	vector<SysColumn*> sysColumns2 = currentDb->getTableAttr(table2Name);
	string tableNameTemp;
	string colNameTemp;
	int index;

	table1ShowColumn.clear();
	table2ShowColumn.clear();

	for(int i = 1; i < commands.size(); i++) {
		if (checkKeyWord(commands[i], FROM))
			columnFlag = false;
		if (columnFlag == true)
			showColumns.push_back(commands[i]);
		if (whereFlag == true)
			whereCommands.push_back(commands[i]);
		if (checkKeyWord(commands[i], WHERE))
			whereFlag = true;
	}
	if (showColumns.size() <= 0 )
		return false;
	if (showColumns[0].compare("*")==0)
	{
		if (showColumns.size() != 1)
			return false;
		for (int i = 0 ;i < sysColumns1.size(); i++)
			table1ShowColumn.push_back(sysColumns1[i]->name);
		for (int i = 0; i < sysColumns2.size(); i++)
			table2ShowColumn.push_back(sysColumns2[i]->name);
	} else {
		for (int i = 0; i < showColumns.size(); i++) {
			tableNameTemp = "";
			colNameTemp = "";
			index = showColumns[i].find('.');
			if (index != -1) {
				tableNameTemp.assign(showColumns[i], 0, index);
				colNameTemp.assign(showColumns[i], index+1, showColumns[i].length()-index-1);
				if (!(table1Name.compare(tableNameTemp) == 0 || table2Name.compare(tableNameTemp) == 0) )
					return false;
				if (table1Name.compare(tableNameTemp) == 0)
					table1ShowColumn.push_back(colNameTemp);
				else if (table2Name.compare(tableNameTemp) == 0)
					table2ShowColumn.push_back(colNameTemp);
			} else {
				colNameTemp.assign(showColumns[i]);
				if (!currentDb->checkTableColumn(table1Name, colNameTemp) && !currentDb->checkTableColumn(table2Name,colNameTemp))
					return false;
				if (currentDb->checkTableColumn(table1Name, colNameTemp))
					table1ShowColumn.push_back(colNameTemp);
				if (currentDb->checkTableColumn(table2Name, colNameTemp))
					table2ShowColumn.push_back(colNameTemp);
			}
			i++;
			if (i < showColumns.size()) {
				if (showColumns[i].compare(",") == 0)
					continue;
				else
					return false;
			}
		}
	}
	table1Require.clear();
	table2Require.clear();
	twoTableJoinRequire.clear();
	parserTwoTableWhere(commands);

	int Num = table1Require.size();
	BYTE	**value_v = new BYTE*[Num];
	string *colName_v = new string[Num];
	BYTE *type = new BYTE[Num];
	BYTE *len = new BYTE[Num];
	BYTE *op = new BYTE[Num];
	for (int i = 0; i < Num; i++){
		colName_v[i] = table1Require[i].colName;
		type[i] = table1Require[i].type;
		len[i] = table1Require[i].len;
		op[i] = table1Require[i].op;
		value_v[i] = new BYTE[len[i]];
		dataUtility::string_to_char((char*)value_v[i], table1Require[i].value, 0, len[i],len[i]);
	}
	int Num2 = table2Require.size();
	BYTE **value2 = new BYTE*[Num2];
	string *colName2 = new string[Num2];
	BYTE *type2 = new BYTE[Num2];
	BYTE *len2 = new BYTE[Num2];
	BYTE *op2 = new BYTE[Num2];
	for (int i = 0 ; i < Num2; i++) {
		colName2[i] = table2Require[i].colName;
		type2[i] = table2Require[i].type;
		len2[i] = table2Require[i].len;
		op2[i] = table2Require[i].op;
		value2[i] = new BYTE[len2[i]];
		dataUtility::string_to_char((char*)value2[i], table2Require[i].value, 0, len2[i], len2[i]);
	}
	vector<RecordEntry*> table1Res = currentDb->getFindRecord(table1Name, value_v, colName_v, type,len, op,Num);
	vector<RecordEntry*> table2Res = currentDb->getFindRecord(table2Name, value2, colName2, type2, len2, op2, Num2);
	string* showA = new string[table1ShowColumn.size()];
	for (int i = 0 ;i < table1ShowColumn.size(); i++)
		showA[i] = table1ShowColumn[i];
	string* showB = new string[table2ShowColumn.size()];
	for (int i = 0; i < table2ShowColumn.size(); i++)
		showB[i] = table2ShowColumn[i];
	currentDb->combine(table1Res, table2Res,table1Name,table2Name,twoTableJoinRequire, showA, table1ShowColumn.size(), showB, table2ShowColumn.size());
	delete[] showA;
	delete[] showB;

	delete[] type;
	delete[] len;
	delete[] op;
	delete[] colName_v;
	for (int i = 0 ; i < Num; i++)
		delete[] value_v[i];
	delete[] value_v;
	delete[] type2;
	delete[] len2;
	delete[] op2;
	delete[] colName2;
	for (int i = 0; i < Num2; i++)
		delete[] value2[i];
	delete[] value2;
	//parserWhere(whereCommands, table1Name);
	//int Num = table1Require.size();
	//BYTE	**value_v = new BYTE*[Num];
	//string *colName_v = new string[Num];
	//BYTE *type = new BYTE[Num];
	//BYTE *len = new BYTE[Num];
	//BYTE *op = new BYTE[Num];
	//for (int i = 0; i < Num; i++){
	//	colName_v[i] = table1Require[i].colName;
	//	type[i] = table1Require[i].type;
	//	len[i] = table1Require[i].len;
	//	op[i] = table1Require[i].op;
	//	value_v[i] = new BYTE[len[i]];
	//	dataUtility::string_to_char((char*)value_v[i], table1Require[i].value, 0, len[i],len[i]);
	//}
	//currentDb->findRecord(table1Name, value_v, colName_v, type,len, op,Num);  //if condCnt=-1,delete all
	return true;
}
bool parser::parserTwoTableWhere(vector<string> commands) {
	bool flag = true;
	//cout << "parser where" << commands.size() <<  endl;
	string tableNameTemp;
	string colNameTemp;
	string tableNameTemp2;
	string colNameTemp2;
	int index;
	for (int i = 0; i < commands.size(); i++) {
		tableNameTemp = "";
		colNameTemp = "";
		if (isOpt(commands[i]) && flag == true){
			if (i-1 < 0)
				return false;
			if (i+1 >= commands.size())
				return false;
			/***********************/
			index = commands[i-1].find('.');
			if (index != -1) {
				tableNameTemp.assign(commands[i-1], 0, index);
				colNameTemp.assign(commands[i-1], index+1, commands[i-1].length()-index-1);
				if (!(table1Name.compare(tableNameTemp) == 0 || table2Name.compare(tableNameTemp) == 0) )
					return false;
				if (!currentDb->checkTableColumn(tableNameTemp, colNameTemp))
					return false;
			} else {
				colNameTemp.assign(commands[i-1]);
				if (!currentDb->checkTableColumn(table1Name, colNameTemp) && !currentDb->checkTableColumn(table2Name,colNameTemp))
					return false;
				if (currentDb->checkTableColumn(table1Name, colNameTemp))
					tableNameTemp = table1Name;
				if (currentDb->checkTableColumn(table2Name, colNameTemp))
					tableNameTemp = table2Name;
			}
			/*******************/
			index = commands[i+1].find('.');
			if (index != -1) {
				tableNameTemp2.assign(commands[i+1], 0, index);
				colNameTemp2.assign(commands[i+1], index+1, commands[i+1].length()-index-1);
				if (table1Name.compare(tableNameTemp) == 0) {
					if (table2Name.compare(tableNameTemp2) != 0)
						return false;
					else {
						if (!currentDb->checkTableColumn(table2Name, colNameTemp2))
							return false;
						twoTableJoinRequire.push_back(tableJoinRequire(colNameTemp, colNameTemp2, getOpt(commands[i])));
					}
				} else if (table2Name.compare(tableNameTemp) == 0) {
					if (table1Name.compare(tableNameTemp2) != 0)
						return false;
					else {
						if (!currentDb->checkTableColumn(table1Name, colNameTemp2))
							return false;
						twoTableJoinRequire.push_back(tableJoinRequire(colNameTemp2, colNameTemp, getOpt(commands[i])));
					}
				}
			} else {
				SysColumn t = *(currentDb->getTableColumn(tableNameTemp, colNameTemp));
				if (t.xtype == INT_TYPE) {
					if (!checkStingIsInt(commands[i+1]))
						return false;
				} else if (t.xtype == VARCHAR_TYPE) {
					if (commands[i+1].at(commands[i+1].length()-1)=='\'')
						commands[i+1].erase(commands[i+1].end()-1);
					if (commands[i+1].at(0) == '\'')
						commands[i+1].erase(commands[i+1].begin());
					if (commands[i+1].length() > t.length)
						return false;
				}
				if (tableNameTemp.compare(table1Name) == 0)
					table1Require.push_back(columnRequire(commands[i+1],colNameTemp,t.xtype, commands[i+1].length(), getOpt(commands[i])));
				else if (tableNameTemp.compare(table2Name) == 0)
					table2Require.push_back(columnRequire(commands[i+1],colNameTemp,t.xtype, commands[i+1].length(), getOpt(commands[i])));
			}
			i++;
			flag = false;
		} else if (checkKeyWord(commands[i], OP_AND))
			flag = true;
	}
	if (flag == true)
		return false;
	return true;
}
bool parser::parserUpdate(vector<string> commands) {
	//cout<< "********************start parser update*************************" << endl;
	//without condition, update all
	if (commands.size() < 6 )
		return false;
	if (!checkKeyWord(commands[2], SET) || commands[4].compare("=")!=0)
		return false;
	vector<SysColumn*> sysColumns = currentDb->getTableAttr(commands[1]);
	if (sysColumns.size() == 0)   // without this table
		return false;
	vector<string> whereCommands;  //without where
	vector<string> setCommands;  //without set
	int index = 3;
	for (index = 3 ; index < commands.size(); index ++){
		if (!checkKeyWord(commands[index], WHERE)){
			setCommands.push_back(commands[index]);
		} else
			break;
	}
	for(index++;index < commands.size(); index++){
		whereCommands.push_back(commands[index]);
	}
	table1Require.clear();
	parserSet(setCommands,commands[1]);
	parserWhere(whereCommands,commands[1]);
	int Num = table1Require.size();
	BYTE	**value_v = new BYTE*[Num];
	string *colName_v = new string[Num];
	BYTE *type = new BYTE[Num];
	BYTE *len = new BYTE[Num];
	BYTE *op = new BYTE[Num];
	for (int i = 0; i < Num; i++){
		colName_v[i] = table1Require[i].colName;
		type[i] = table1Require[i].type;
		len[i] = table1Require[i].len;
		op[i] = table1Require[i].op;
		value_v[i] = new BYTE[len[i]];
		dataUtility::string_to_char((char*)value_v[i], table1Require[i].value, 0, len[i],len[i]);
	}
	currentDb->updateRecord(commands[1], value_v, colName_v, type,len, op,Num);  //if condCnt=-1,delete all
	delete[] type;
	delete[] len;
	delete[] op;
	delete[] colName_v;
	for (int i = 0 ; i < Num; i++)
		delete[] value_v[i];
	delete[] value_v;
	return true;
}

bool parser::parserDelete(vector<string> commands) {
	//cout << "*********start parser delete****************" << endl;
	if (commands.size() < 4)
		return false;
	if (!checkKeyWord(commands[1], FROM) || !checkKeyWord(commands[3], WHERE))
		return false;
	vector<SysColumn*> sysColumns = currentDb->getTableAttr(commands[2]);
	if (sysColumns.size() == 0)   // without this table
		return false;
	//bool deleteRecord(string tableName,BYTE **Value,string *colName,BYTE *type,BYTE *len,BYTE *op, BYTE condCnt);
	vector<string> whereCommands;
	for(int i = 4; i < commands.size(); i++){
		whereCommands.push_back(commands[i]);
	}
	table1Require.clear();
	if (!parserWhere(whereCommands, commands[2]))
		return false;
	int Num = table1Require.size();
	BYTE	**value_v = new BYTE*[Num];
	string *colName_v = new string[Num];
	BYTE *type = new BYTE[Num];
	BYTE *len = new BYTE[Num];
	BYTE *op = new BYTE[Num];
	for (int i = 0; i < Num; i++){
		colName_v[i] = table1Require[i].colName;
		type[i] = table1Require[i].type;
		len[i] = table1Require[i].len;
		op[i] = table1Require[i].op;
		value_v[i] = new BYTE[len[i]];
		dataUtility::string_to_char((char*)value_v[i], table1Require[i].value, 0, len[i],len[i]);
	}
	currentDb->deleteRecord(commands[2], value_v, colName_v, type,len, op,Num);  //if condCnt=-1,delete all
	delete[] type;
	delete[] len;
	delete[] op;
	delete[] colName_v;
	for (int i = 0 ; i < Num; i++)
		delete[] value_v[i];
	delete[] value_v;
	return true;
}
bool parser::parserSet(vector<string> commands, string tablename) {
	bool flag = true;
	//cout << "parser set" << commands.size() <<  endl;
	string tableNameTemp;
	string colNameTemp;
	int index;
	for (int i = 0; i < commands.size(); i++) {
		index = commands[i].find('.');
		if (index != -1) {
			tableNameTemp.assign(commands[i], 0, index);
			colNameTemp.assign(commands[i], index+1, commands[i].length()-index-1);
			if (tablename.compare(tableNameTemp) != 0)
				return false;
		} else
			colNameTemp.assign(commands[i]);
		if (!currentDb->checkTableColumn(tablename, colNameTemp))  // without this column
			return false;
		SysColumn t = *(currentDb->getTableColumn(tablename, colNameTemp));
		i++;
		if (i >= commands.size() || commands[i].compare("=")!=0)
			return false;
		i++;
		if (i >= commands.size())
			return false;
		if (t.xtype == INT_TYPE) {
			if (!checkStingIsInt(commands[i]))
				return false;
		} else if (t.xtype == VARCHAR_TYPE) {
			if (commands[i].at(commands[i].length()-1)=='\'')
				commands[i].erase(commands[i].end()-1);
			if (commands[i].at(0) == '\'')
				commands[i].erase(commands[i].begin());
			if (commands[i].length() > t.length)
				return false;
		}
		table1Require.push_back(columnRequire(commands[i],colNameTemp,t.xtype, commands[i].length(), SET));
		i++;
		if (i < commands.size()) {
			if (commands[i].compare(",")==0)
				continue;
			else
				return false;
		}
	}
}
bool parser::parserWhere(vector<string> commands, string tablename) {
	bool flag = true;
	//cout << "parser where" << commands.size() <<  endl;
	string tableNameTemp;
	string colNameTemp;
	int index;
	for (int i = 0; i < commands.size(); i++) {
		tableNameTemp = "";
		colNameTemp = "";
		if (isOpt(commands[i]) && flag == true){
			if (i-1 < 0)
				return false;
			if (i+1 >= commands.size())
				return false;
			/***********************/
			index = commands[i-1].find('.');
			if (index != -1) {
				tableNameTemp.assign(commands[i-1], 0, index);
				colNameTemp.assign(commands[i-1], index+1, commands[i-1].length()-index-1);
				if (tablename.compare(tableNameTemp) != 0)
					return false;
			} else
				colNameTemp.assign(commands[i-1]);
			/*******************/
			if (!currentDb->checkTableColumn(tablename, colNameTemp))
				return false;
			SysColumn t = *(currentDb->getTableColumn(tablename, colNameTemp));
			if (t.xtype == INT_TYPE) {
				if (!checkStingIsInt(commands[i+1]))
					return false;
			} else if (t.xtype == VARCHAR_TYPE) {
				if (commands[i+1].at(commands[i+1].length()-1)=='\'')
					commands[i+1].erase(commands[i+1].end()-1);
				if (commands[i+1].at(0) == '\'')
					commands[i+1].erase(commands[i+1].begin());
				if (commands[i+1].length() > t.length)
					return false;
			}
			table1Require.push_back(columnRequire(commands[i+1],colNameTemp,t.xtype, commands[i+1].length(), getOpt(commands[i])));
			i++;
			flag = false;
		} else if (checkKeyWord(commands[i], OP_AND))
			flag = true;
	}
	if (flag == true)
		return false;
	return true;
}
bool parser::parserDesc(vector<string> commands) {
	if (commands.size() != 2)
		return false;
	//cout << "desc table info " << endl;
	//desc table , if without the table, return false
	currentDb->printTable(commands[1]);
	return true;

}
bool parser::parserShowTable(vector<string> commands) {
	if (commands.size() != 2)
		return false;
	if (!checkKeyWord(commands[1], TABLES))
		return false;
	//?show tables
	//cout << "show tables" << endl;
	currentDb->printTables();
	return true;
}
bool parser::parserDrop(vector<string> commands) {
	if (commands.size() != 3)
		return false;
	if (checkKeyWord(commands[1], DATABASE) ) {
		//cout << "drop database " << endl;
		(*currentDb).dropDataBase(commands[2]);
		//?if system file does not have this database, return false.
		return true;
	} else if (checkKeyWord(commands[1], TABLE)) {
		//cout << "drop table " << endl;
		(*currentDb).dropTable(commands[2]);
		//?if system file does not have this table, return false.
		return true;
	} else
		return false;
}
bool parser::parserInsert(vector<string> commands) {
	//cout << "***********start parse insert data************" << endl;
	vector<vector<string>> datas;
	if (commands.size() <= 4)
		return false;
	if (checkKeyWord(commands[1], ISINTO) && checkKeyWord(commands[3], VALUES)) {
		if (!checkNameAvaliable(commands[2])) {
			cout << "table name is unableuse" << endl;
			return false;
		}
		string tablename = commands[2];
		//according to the tablename,find the table column info
		
		int index = 4;
		vector<string> temp;
		string s = "";
		commands[commands.size()-1]+=',';
		while (index < commands.size()) {
			temp.clear();
			//cout << commands[index].c_str() << endl;
			for (int i = 0; i < commands[index].size(); i++) {

				if (commands[index][i] == '(' || commands[index][i] == ')') {
					commands[index].erase(commands[index].begin() +i);
					i--;
				}
				else if (commands[index][i] == '\'') {
						i++;
						while (i < commands[index].size() && commands[index][i] != '\'') {
							s += commands[index][i];
							i++;
						}
						s+='\'';
						if (s.length() != 0)
							temp.push_back(s);
						s="";
				}
				else if (commands[index][i] == ',') {
					if (s.length() != 0)
						temp.push_back(s);
						s="";
				} else
					s+=commands[index][i];
			}
			index++;
			datas.push_back(temp);
		}
		vector<SysColumn*> sysColumn = (*currentDb).getTableAttr(commands[2]);
		string* name = new string[sysColumn.size()];
		RecordEntry tempRecord;
		tempRecord.length  =  new BYTE[sysColumn.size()];
		tempRecord.item = new BYTE*[sysColumn.size()];

		for (int i = 0; i < sysColumn.size(); i++) {
			name[i] = sysColumn[i]->name;
		}
		for (int i = 0; i < datas.size(); i++) {
			/*for (int k = 0; k < datas[i].size(); k++)
				cout << datas[i][k] << "|";
			cout <<endl;*/
			if (datas[i].size() != sysColumn.size()) {
				return false;
			}
			for (int j = 0; j < datas[i].size(); j++) {
				if (sysColumn[j]->xtype == VARCHAR_TYPE) {
					if (datas[i][j].length() > 0 && datas[i][j].at(0)=='\'')
						datas[i][j].erase(datas[i][j].begin());
					if (datas[i][j].length() > 0 && datas[i][j].at(datas[i][j].length()-1)=='\'')
						datas[i][j].erase(datas[i][j].end()-1);
				}
			}
			if (!checkColumnsValue(sysColumn, datas[i])) {
				//cout << "column error----------------------"<< endl;
				return false;
			}
			for(int j = 0; j < datas[i].size(); j++) {
				if ((*sysColumn[j]).xtype == INT_TYPE) {
					tempRecord.length[j] = 4;
					tempRecord.item[j] = new BYTE[tempRecord.length[j]];
					tempRecord.item[j] =(BYTE*) dataUtility::data_to_char<int>(atoi(datas[i][j].c_str()));
				}
				else if ((*sysColumn[j]).xtype == VARCHAR_TYPE) {
					tempRecord.length[j] = datas[i][j].length();
					tempRecord.item[j] = new BYTE[tempRecord.length[j]];
					dataUtility::string_to_char((char*)tempRecord.item[j], datas[i][j], 0, datas[i][j].length(),sysColumn[j]->length );

				}
			}
			
			currentDb->insertRecord(&tempRecord, name, commands[2]);
		}
	} else
		return false;
}

bool parser::checkColumnsValue(vector<SysColumn*> sysColumns, vector<string> datas) {
	if (datas.size() != sysColumns.size())
		return false;
	for (int i = 0; i < datas.size(); i++) {
		if (!checkOneColumnValue((*sysColumns[i]), datas[i])) {
			return false;
		}
	}
	return true;
}

bool parser::checkOneColumnValue(SysColumn syscolumn, string data) {
	//? null
	//BYTE xtype;		// data type
	//TYPE_OFFSET length;	// the max length of this type
	
	if (syscolumn.xtype == INT_TYPE) {
		for (int i = 0; i < data.length(); i++) {
			if (!isDig(data[i]))
				return false;
		}
	} else if (syscolumn.xtype == VARCHAR_TYPE) {
		if (data.length() > syscolumn.length)
		return false;
	}
	return true;
}
bool parser::checkKeyWord(string s, int keyvalue) {
	for (int i = 0; i < s.length(); i++) {
		if (!isEnglishAlphabet(s.at(i)))
			return false;
		if (s.at(i) >= 'a' && s.at(i) <= 'z') {
			s.at(i) = s.at(i) -32;
		} 
	}
	if (s.compare(getKeyWords(keyvalue)) == 0)
		return true;
	else
		return false;
}
string parser::getKeyWords(int keyvalue) {
	switch (keyvalue)
	{
	case CREATE:
		return "CREATE";
	case INSERT:
		return "INSERT";
	case DROP:
		return "DROP";
	case ISNULL:
		return "NULL";
	case ISINTO:
		return "INTO";
	case UPDATE:
		return "UPDATE";
	case IS_NOT:
		return "NOT";
	case IS_PRIMARY:
		return "PRIMARY";
	case IS_KEY:
		return "KEY";
	case DESC:
		return "DESC";
	case SHOW:
		return "SHOW";
	case USE:
		return "USE";
	case VALUES:
		return "VALUES";
	case TABLES:
		return "TABLES";
	case TABLE:
		return "TABLE";
	case DATABASE:
		return "DATABASE";
	case INT_TYPE:
		return "INT";
	case VARCHAR_TYPE:
		return "VARCHAR";
	case DELETE:
		return "DELETE";
	case FROM:
		return "FROM";
	case WHERE:
		return "WHERE";
	case SELECT:
		return "SELECT";
	case SET:
		return "SET";
	case IS:
		return "IS";
	case EQUAL:
		return "=";
	case NOTEQUAL:
		return "!=";
	case MORE:
		return ">";
	case LESS:
		return "<";
	case MOREEQUAL:
		return ">=";
	case LESSEQUAL:
		return "<=";
	case OP_AND:
		return "AND";
	default:
		break;
	}
}
bool parser::parserCreate(vector<string> commands) {
	if (commands.size() < 3 )
		return false;
	if (checkKeyWord(commands[1], DATABASE)){
		if (commands.size() != 3 || !checkNameAvaliable(commands[2]))
			return false;
		else {
			//execute create database;
			(*currentDb).createDataBase(commands[2]);
			//cout << "***********start parse create database************" << endl;
		}
	} else if (checkKeyWord(commands[1], TABLE)) {
		//cout << "***********start parse create table************" << endl;
		if (!checkNameAvaliable(commands[2]))  //tablename
			return false;
		
		if (commands.size() < 5)
			return false;
		
		if (strcmp(commands[3].c_str(), "(") != 0) 
			return false;
		if ( strcmp(commands[commands.size()-1].c_str(), ")") != 0)
			return false;

		commands.erase(commands.begin()+3);
		commands.erase(commands.end()-1);
		if (commands.size() == 3)  //without column
		{
			//create a table without column
			return true;
		}
		vector<string> columnInfos;
		int index = 3;
		tableColumn colInfos;
		while(index < commands.size()) {
			if (commands[index].at(commands[index].length()-1) == ',') {
				commands[index].erase(commands[index].length()-1);
				columnInfos.push_back(commands[index]);
				parserCreateColumn(columnInfos, &colInfos);

				columnInfos.clear();
			} else 
			columnInfos.push_back(commands[index]);
			index++;
		}
		if (columnInfos.size() !=0)
			parserCreateColumn(columnInfos, &colInfos);
		//void DBManager::createTable(string tablename, UINT colNum, string colName[], BYTE type[], TYPE_OFFSET length[], bool nullable[]){
		string* colName = new string[colInfos.colNum];
		BYTE* type =  new BYTE[colInfos.colNum];
		TYPE_OFFSET* length = new TYPE_OFFSET[colInfos.colNum];
		bool*  nullable = new bool[colInfos.colNum];

		
		for (int i = 0; i < (colInfos).colNum; i++)
		{
			colName[i]  = (colInfos).colInfo[i].name;
			type[i] = (colInfos).colInfo[i].type;
			length[i] = (colInfos).colInfo[i].length;
			nullable[i] = (colInfos).colInfo[i].null;
		}
		(*currentDb).createTable(commands[2], colInfos.colNum, colName, type, length, nullable);
	
		delete[] colName;
		delete[] type;
		delete[] length;
		delete[] nullable;
	}
	return true;
}


bool parser::parserCreateColumn(vector<string> columnInfo, tableColumn* colInfos) {
	//cout << "start parse create column*****************" << endl;
	//for (int i = 0 ; i < columnInfo.size(); i++ )
	//	cout << columnInfo[i].c_str() << endl;

	int tempIndex;
	if (columnInfo.size() < 2 )
		return false;
	#pragma region primary key
	if (checkKeyWord(columnInfo[0], IS_PRIMARY)) {
		if (columnInfo.size() < 3 || columnInfo.size() > 5)
			return false;
		if (checkKeyWord(columnInfo[1], IS_KEY))
		{
			if (columnInfo.size() == 3) {
				if (columnInfo[2].at(0) != '(' || columnInfo[2].at(columnInfo[2].length()-1) != ')')
					return false;
				
				columnInfo[2].erase(columnInfo[2].begin());
				columnInfo[2].erase(columnInfo[2].end()-1);
			} else if (columnInfo.size() == 4) {
				if (columnInfo[2].compare("(") && columnInfo[3].at(columnInfo[3].length()-1)==')') {
					columnInfo[2] = columnInfo[3].erase(columnInfo[3].length()-1);
				} else if (columnInfo[3].compare(")") && columnInfo[2].at(0)=='(') {
					columnInfo[2].erase(columnInfo[2].begin());
				} else
					return false;
			} else if (columnInfo.size() == 5) {
				if (!(columnInfo[2].compare("(") && columnInfo[4].compare(")")))
					return false;
			} else
				return false;
			//cout << columnInfo[2].c_str() << endl;
			tempIndex = (*colInfos).checkColName(columnInfo[2]);
			if ( tempIndex != -1)
				(*colInfos).colInfo[tempIndex].primaryKey = 1;
			else
				return false;
		} else 
			return false;
	} 
	#pragma endregion
	else {
		//cout << "add new column" << endl;
		column newColumn;
		if (!checkNameAvaliable(columnInfo[0]) || columnInfo.size() < 2 || columnInfo.size() > 4)
			return false;
		newColumn.name = columnInfo[0];
		int length; 
		string typeString = "";
		string lengthString = "";
		tempIndex = 0;
		while (tempIndex < columnInfo[1].size() && columnInfo[1].at(tempIndex) != '(') {
			typeString+=columnInfo[1].at(tempIndex);
			tempIndex++;
			if (columnInfo[1].at(tempIndex) == '(' || tempIndex >= columnInfo[1].size() )
				break;
		}
		newColumn.type = getType(typeString);
		if (tempIndex < columnInfo[1].size() && columnInfo[1].at(tempIndex) == '(') {
			//get length
			tempIndex++;
			while (tempIndex < columnInfo[1].size() && columnInfo[1].at(tempIndex) != ')') {
				lengthString+=columnInfo[1].at(tempIndex);
				tempIndex++;
			if (columnInfo[1].at(tempIndex) == ')' || tempIndex >= columnInfo[1].size() )
				break;
			}
		}
		if (newColumn.type == INT_TYPE)
			newColumn.length = 4;
		else
			newColumn.length = atoi(lengthString.c_str());
		if (columnInfo.size() == 4) {
			if (checkKeyWord(columnInfo[2], IS_NOT) && checkKeyWord(columnInfo[3], ISNULL))
				newColumn.null = 0;
			else
				return false;
		}
		
		tableColumn* newTableColumn = new tableColumn((*colInfos).colNum+1);
		for (int i = 0; i < (*colInfos).colNum; i++)
		{
			(*newTableColumn).colInfo[i] = (*colInfos).colInfo[i];
		}
		(*newTableColumn).colInfo[(*colInfos).colNum] = newColumn;
		(*colInfos) = (*newTableColumn);
		delete[] newTableColumn;
		//cout << "end add new" << endl;
	}
}

bool parser::parserUse(vector<string> commands) {
	//cout << "***********start parse use************" << endl;
	if (commands.size() != 2)
		return false;
	if (checkNameAvaliable(commands[1])) {
		//execute use command
		(*currentDb).switchDataBase(commands[1]);
	} else
		return false;
	return true;
}

bool parser::checkNameAvaliable(string s){
	//? should not be key words
	if (s.length() == 0)
		return false;
	for (int i = 0; i < s.length(); i++) {
		if (!(isDig(s.at(i)) || isEnglishAlphabet(s.at(i)) || s.at(i)=='_'))
			return false;
	}
	return true;
}

bool parser::isCmp(char k) {
	return (k == '>' || k == '<' || k == '=');
}
bool parser::isEnglishAlphabet(char k) {
	return (('a' <= k && k <= 'z') || ('A' <= k && k <= 'Z'));
}
bool parser::isDig(char k) {
	return '0' <= k && k <= '9';
}

int parser::getType(string s) {
	for (int i = 0; i < s.length(); i++) {
		if (!isEnglishAlphabet(s.at(i)))
			return -1;
		if (s.at(i) >= 'a' && s.at(i) <= 'z') {
			s.at(i) = s.at(i) -32;
		} 
	}

	if (checkKeyWord(s, INT_TYPE))
		return INT_TYPE;
	if (checkKeyWord(s, VARCHAR_TYPE))
		return VARCHAR_TYPE;
	return -1;

}

bool parser::isOpt(string s) {
	if (s.compare(getKeyWords(EQUAL))==0 || s.compare(getKeyWords(NOTEQUAL))==0 ||
		s.compare(getKeyWords(MORE))==0 || s.compare(getKeyWords(MOREEQUAL))==0 ||
		s.compare(getKeyWords(LESS))==0 || s.compare(getKeyWords(LESSEQUAL))==0)
		return true;
	else
		return false;
}
int parser::getOpt(string s) {
	if (s.compare(getKeyWords(EQUAL))==0)
		return EQUAL;
	if (s.compare(getKeyWords(NOTEQUAL))==0)
		return NOTEQUAL;
	if (s.compare(getKeyWords(MORE))==0)
		return MORE;
	if (s.compare(getKeyWords(LESS))==0)
		return LESS;
	if (s.compare(getKeyWords(MOREEQUAL)) ==0)
		return MOREEQUAL;
	if (s.compare(getKeyWords(LESSEQUAL))==0)
		return LESSEQUAL;
	return -1;
}
bool parser::checkStingIsInt(string s){
	for (int i = 0; i < s.length(); i++)
	{
		if (!isDig(s.at(i)))
			return false;
	}
	return true;
}


