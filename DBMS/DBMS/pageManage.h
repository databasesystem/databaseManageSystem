#ifndef _PAGEMANAGE_H
#define _PAGEMANAGE_H
#include "globalVariable.h"
#include <iostream>
#include <direct.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

class pageHeader {
public:
	int pageId;
	int fileId;
	int rowCount;
	int firstFreeOffset;
	int freeCount;
};

class dbPage {
public:
	pageHeader header;
	char data[PAGE_SIZE];
	dbPage() {
		memset(data, 0, PAGE_SIZE);
	}
};

class attr{
public:
		int num;
		string *colname;
		int* coltype; //0-int 1-double 2-float 3-char 4-string 5-varchar 6-boolean
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