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
		memset(data, -1, PAGE_SIZE);
	}
	dbPage(char* src){
		memset(data, -1, PAGE_SIZE);
		strcpy(data,src);
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
		int pagenum;
		attr(int num_v){
			num = num_v;
			colname = new string[num];
			coltype = new int[num];
			collen = new int[num];
			colIsNull = new bool[num];
			pagenum = 0;  //first attr page is also a count
		};
};
#endif