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
	int offset;
	recordEntry(int num_v, int* itemlen_v){
		num = num_v;
		isNull = new bool[num];
		item = new char*[num];
		itemlen = new int[num];
		length = 1+num+4;
		for (int i = 0; i < num; i++) {
			itemlen[i] = itemlen_v[i]; 
			item[i] = new char[itemlen[i]];
			length += itemlen[i];
		}
		offset = -1;
	}
	char* getRecord(recordEntry* record){
		char* res = new char[record->length];
		res[0] = dataUtility::bool_to_byte(record->isdeleted);
		for (int i = 0; i < num; i++) {
			res[1+i]=dataUtility::bool_to_byte(record->isNull[i]);
		}

		int index = 1+num;

		char* temp = dataUtility::data_to_char<int>(record->offset);
		int* t = dataUtility::char_to_int(temp);
		cout << "offset:" << *t << endl;

		for (int i = 0; i < num; i++) {
			
			dataUtility::bytefillbyte(res, record->item[i],index);
			index = index + record->itemlen[i];
		}
		
		dataUtility::bytefillbyte(res,temp, index,4);
		char* test = new char[4];
		test = dataUtility::getbyte(res,index, 4);
		int* res1 = dataUtility::char_to_int(test);
		cout << *res1 << endl;
		return res;
	}
};

class DBStorage{

public:
	string dbname;
	UINT dbid;
	UINT filenum;
	DBStorage(string dbname, UINT dbid, bool isCreate);
	~DBStorage();
	void createTable(char* filename,char* databasename, attr tableinfo);

	void createDataBase(char* databasename);

	void insertData(char* tablename, recordEntry reord);

	char* getTablePath(char* tablename);
};

#endif