#ifndef _RECORDMANAGE_H
#define _RECORDMANAGE_H
#include "data_utility.h"

struct recordEntry {
	bool isdeleted;
	bool* isNull;
	char** item;  //C++ byte is defined for unsigned char
	int length;
	int num;
	int* itemlen;
	recordEntry(int num_v, int* itemlen_v){
		num = num_v;
		isNull = new bool[num];
		item = new char*[num];
		itemlen = new int[num];
		length = 1+num;
		for (int i = 0; i < num; i++) {
			itemlen[i] = itemlen_v[i]; 
			item[i] = new char[itemlen[i]];
			length += itemlen[i];
			
		}
	}
	char* getRecord(recordEntry* record){
		char* res = new char[record->length];
		char* temp =dataUtility::data_to_char<bool>(record->isdeleted);
		dataUtility::printChars(temp);
		res = dataUtility::bytefillbyte(res, temp,0);
		for (int i = 0; i < num; i++) {
			char* temp =dataUtility::data_to_char<bool>(isNull[i]);
			res = dataUtility::bytefillbyte(res, temp,1+i);
		}
		int index = 1+num;
		for (int i = 0; i < num; i++) {
			res = dataUtility::bytefillbyte(res, item[i],index);
			index+=itemlen[i];
		}
		dataUtility::printChars(res);
		return res;
	}
};

class DBStorage{

public:
	string dbname;
	UINT dbid;

	DBStorage(string dbname, UINT dbid, bool isCreate);
	~DBStorage();
	void createTable(char* filename,char* databasename, attr tableinfo);

	void createDataBase(char* databasename);

	void insertData(char* tablename, recordEntry reord);
};

#endif