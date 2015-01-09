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
	while(!feof(fin)) {
		fgets(command, 1000,fin);
		splitStr(command, &res);
		//semicolon has deleted
			if (res.size() > 0 && res[res.size()-1].at(res[res.size()-1].length()-1) == ';') {
			//This is one command;
			res[res.size()-1].erase(res[res.size()-1].length()-1);
			if (!parserOneCommand(res))
				cout << "This command is wrong." << endl;
			res.clear();
		}
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
		temp += str[index];
		index++;
		if (str[index] == '\'') {
			temp += str[index];
			index++;
			while (index < strlen(str) && str[index] != '\'') {
				temp += str[index];
				index++;
			}
		}
		if (str[index] == ' ' && temp.length()!=0) {
			(*res).push_back(temp);
			temp ="";
		}
		while (str[index] ==  ' ')  index++;
		if (index >= strlen(str) || str[index] == '\n')
			break;
	}
	if (temp.length() !=0 )
		(*res).push_back(temp);
}

bool parser::parserOneCommand(vector<string> commands) {
	// if return false, prove this command is wrong.

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
	return true;
}
bool parser::parserDelete(vector<string> commands) {
	if (commands.size() < 4)
		return false;
	if (!checkKeyWord(commands[1], FROM) || !checkKeyWord(commands[3], WHERE))
		return false;
	if (!checkNameAvaliable(commands[2]))
		return false;
	
	return true;
}
bool parser::parserDesc(vector<string> commands) {
	if (commands.size() != 2)
		return false;
	cout << "desc table info " << endl;
	//desc table , if without the table, return false
	return true;

}
bool parser::parserShowTable(vector<string> commands) {
	if (commands.size() != 2)
		return false;
	if (!checkKeyWord(commands[1], TABLES))
		return false;
	//?show tables
	cout << "show tables" << endl;
	return true;
}
bool parser::parserDrop(vector<string> commands) {
	if (commands.size() != 3)
		return false;
	if (checkKeyWord(commands[1], DATABASE) ) {
		cout << "drop database " << endl;
		(*currentDb).dropDataBase(commands[2]);
		//?if system file does not have this database, return false.
		return true;
	} else if (checkKeyWord(commands[1], TABLE)) {
		cout << "drop table " << endl;
		(*currentDb).dropTable(commands[2]);
		//?if system file does not have this table, return false.
		return true;
	} else
		return false;
}
bool parser::parserInsert(vector<string> commands) {
	cout << "***********start parse insert data************" << endl;
	vector<vector<string*>> datas;
	if (commands.size() <= 4)
		return false;
	if (checkKeyWord(commands[1], ISINTO) && checkKeyWord(commands[3], VALUES)) {
		if (!checkNameAvaliable(commands[2]))
			return false;
		string tablename = commands[2];
		//according to the tablename,find the table column info
		
		int index = 4;
		vector<string*> temp;
		string s = "";
		commands[commands.size()-1]+=',';
		while (index < commands.size()) {
			temp.clear();
			for (int i = 0; i < commands[index].size(); i++) {
				if (commands[index][i] == '(' || commands[index][i] == ')') {
					commands[index].erase(commands[index].begin() +i);
					i--;
				}
				else if (commands[index][i] == ',') {
					temp.push_back(&s);
					s="";
				} else
					s+=commands[index][i];
			}
			index++;
			datas.push_back(temp);
		}
		vector<SysColumn*> sysColumn = (*currentDb).getTableAttr(commands[2]);
		string* name = new string[sysColumn.size()];

		for (int i = 0; i < datas.size(); i++) {
			if (!checkColumnsValue(sysColumn, (datas[i])))
				return false;
			RecordEntry tempRecord;
			for(int j = 0; j < datas[i].size(); j++) {
				name[j] = sysColumn[j]->name;
				if ((*sysColumn[j]).xtype == INT)
				tempRecord.length[j] = 4;
				else if ((*sysColumn[j]).xtype == VARCHAR)
					tempRecord.length[j] = (*(datas[i][j])).length();
				dataUtility::string_fill_char((char*)tempRecord.item[j], (*(datas[i][j])), 0, sysColumn[j]->length);
			}
			currentDb->insertRecord(&tempRecord, name, commands[2]);
		}
		
	} else
		return false;
}

bool parser::checkColumnsValue(vector<SysColumn*> sysColumns, vector<string*> datas) {
	if (datas.size() != sysColumns.size())
		return false;
	for (int i = 0; i < datas.size(); i++) {
		if (!checkOneColumnValue((*sysColumns[i]), datas[i]))
			return false;
	}
	return true;
}
bool parser::checkOneColumnValue(SysColumn syscolumn, string* data) {
	//? null
	//BYTE xtype;		// data type
	//TYPE_OFFSET length;	// the max length of this type
	if (syscolumn.xtype == VARCHAR) {
		if ((*data).length() > 0 && (*data).at(0)=='\'')
			(*data).erase((*data).begin());
		if ((*data).length() > 0 && (*data).at((*data).length()-1)=='\'')
			(*data).erase((*data).end()-1);
	}
	if ((*data).length() > syscolumn.length)
		return false;
	if (syscolumn.xtype == INT) {
		for (int i = 0; i < (*data).length(); i++) {
			if (!isDig((*data)[i]))
				return false;
		}
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
	case INT:
		return "INT";
	case VARCHAR:
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
			cout << "***********start parse create database************" << endl;
		}
	} else if (checkKeyWord(commands[1], TABLE)) {
		cout << "***********start parse create table************" << endl;
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
	cout << "start parse create column*****************" << endl;
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
			cout << columnInfo[2].c_str() << endl;
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
		cout << "tegdsjkagdk" << endl;
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
	cout << "***********start parse use************" << endl;
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
	cout << "get type" << endl;
	for (int i = 0; i < s.length(); i++) {
		if (!isEnglishAlphabet(s.at(i)))
			return -1;
		if (s.at(i) >= 'a' && s.at(i) <= 'z') {
			s.at(i) = s.at(i) -32;
		} 
	}

	if (checkKeyWord(s, INT))
		return INT;
	if (checkKeyWord(s, VARCHAR))
		return VARCHAR;
	return -1;

}
