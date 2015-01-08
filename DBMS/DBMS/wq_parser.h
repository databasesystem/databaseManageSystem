#ifndef WQPARSER_H
#define WQPARSER_H
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
/*
#define OP_AND 5
#define OP_OR 6
#define EQUAL 7
#define LESS 8
#define MORE 9
#define NOTEQUAL 10
*/
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
#define ISTINTO 26
#define VALUES 27
#define SELECT_STAR 28
/*
#define LESSEQUAL 28
#define MOREEQUAL 29
*/

class parser {
public:
	void testParse();
	void BatchSqlInFile(char* filename);
	void splitStr(char* str, vector<string>* res);
	bool parserOneCommand(vector<string> commands);
	bool parserCreate(vector<string> commands);
	bool parserUse(vector<string> commands);
	bool checkNameAvaliable(string s);
	bool isCmp(char c);
	bool isEnglishAlphabet(char c);
	bool isDig(char c);
};
#endif