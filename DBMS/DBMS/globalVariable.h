#ifndef _GLOBALVARIABLE_H
#define _GLOBALVARIABLE_H

#include <iostream>
#include <stdio.h>
#include <string>

using namespace std;
const int PAGE_SIZE = 8192;
typedef  unsigned int UINT;

#define INT_TYPE 0
#define DOUBLE_TYPE 1
#define FLOAT_TYPE 2
#define CHAR_TYPE 3
#define STRING_TYPE 4
#define VARCHAR_TYPE 5
#define BOOLEAN_TYPE 6

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
		}
};
#endif