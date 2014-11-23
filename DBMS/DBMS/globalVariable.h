#ifndef _GLOBALVARIABLE_H
#define _GLOBALVARIABLE_H

#include <iostream>
#include <stdio.h>
#include <string>

using namespace std;
const int PAGE_SIZE = 8192;
class attr{
public:
		int num;
		string *colname;
		int* coltype; //0-int 1-double 2-float 3-char 4-string 5-varchar 6-boolean
		int* collen;
		bool* colIsNull;
		attr(int num_v){
			num = num_v;
			colname = new string[num];
			coltype = new int[num];
			collen = new int[num];
			colIsNull = new bool[num];
		}
};
#endif