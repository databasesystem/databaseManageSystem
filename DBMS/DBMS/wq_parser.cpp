#include "wq_parser.h"

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
		if (res.size() > 0 && res[res.size()-1].at(res[res.size()-1].length()-1) == ';') {
			//This is one command;
			res[res.size()-1].erase(res[res.size()-1].length()-1);
			for (int i = 0; i < res.size(); i++)
				cout << res[i].c_str() << " ";
			cout << endl;
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
	while (str[index] != ' ') {
		temp += str[index];
		index++;
		if (str[index] == ' ' && temp.length()!=0) {
			(*res).push_back(temp);
			temp ="";
		}
		while (str[index] ==  ' ')  index++;
		if (index >= strlen(str) || str[index] == '\n')
			break;
	}
	(*res).push_back(temp);
}

bool parser::parserOneCommand(vector<string> commands) {
	// if return false, prove this command is wrong.
	if (commands.size() == 0)
		return false;
	if (strcmp(commands[0].c_str(), "CREATE") == 0) {
		return parserCreate(commands);
	}
	else if (strcmp(commands[0].c_str(), "USE") == 0) {
		return parserUse(commands);
	}
	return true;
}

bool parser::parserCreate(vector<string> commands) {
	if (commands.size() < 3 )
		return false;
	if (strcmp(commands[1].c_str(), "DATEBASE") == 0){
		if (commands.size() != 3 || !checkNameAvaliable(commands[2]))
			return false;
		else {
			//execute create database;
			cout << "***********start parse create database************" << endl;
		}
	} else if (strcmp(commands[1].c_str(), "TABLE") == 0) {
		cout << "***********start parse create table************" << endl;
		if (!checkNameAvaliable(commands[2]))  //tablename
			return false;
		if (commands.size() < 5)
			return false;
		if (strcmp(commands[3].c_str(), "(") != 0 ||  strcmp(commands[commands.size()-1].c_str(), ")") != 0)
			return false;
		int index = 4;
		/*while (strcmp(commands[4].c_str(), ",") != 0) {

		}*/
	}
	return true;
}

bool parser::parserUse(vector<string> commands) {
	cout << "***********start parse use************" << endl;
	if (commands.size() != 2)
		return false;
	if (checkNameAvaliable(commands[1])) {
		//execute use command
	} else
		return false;
	return true;
}

bool parser::checkNameAvaliable(string s){
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