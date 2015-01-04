#ifndef __NODE__H__
#define __NODE__H__

#include "data_utility.h"
#include "fileManage.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>

#define TESTTABLEPATH "./studentManage/studentinfo"
#define TESTFILEID 1

#define ATTR_PAGE_NUM 0

using namespace std;

typedef pair<TYPE_ID,TYPE_ID> rowID;	//<FileID, PageID>

struct recordEntry {
	bool isdeleted;
	bool* isNull;
	char** item;  //C++ byte is defined for unsigned char
	int length;
	int num;
	int* itemlen;
	TYPE_OFFSET offset;
	recordEntry(int num_v, int* itemlen_v){
		num = num_v;
		isNull = new bool[num];
		item = new char*[num];
		itemlen = new int[num];
		length = 1+num+2;
		for (int i = 0; i < num; i++) {
			itemlen[i] = itemlen_v[i]; 
			item[i] = new char[itemlen[i]];
			length += itemlen[i];
		}
		offset = EXIST_INDEX;
	}
	char* getRecord(recordEntry* record){
		char* res = new char[record->length+1];
		res[0] = dataUtility::bool_to_byte(record->isdeleted);
		for (int i = 0; i < num; i++) {
			res[1+i]=dataUtility::bool_to_byte(record->isNull[i]);
		}

		int index = 1+num;
		for (int i = 0; i < num; i++) {
			dataUtility::bytefillbyte(res, record->item[i],index);
			index = index + record->itemlen[i];
		}
		
		char* freeOffset = dataUtility::data_to_char<TYPE_OFFSET>(record->offset);
		dataUtility::bytefillbyte(res, freeOffset, index, SIZE_OFFSET);
		res[record->length] = 0;
		delete[] freeOffset;
		return res;
	}
};

struct Node{
	bool dirty;
	dbPage* page;
	Node(){
		dirty = false;
		page = NULL;
	}
	Node(dbPage* _page) : page(_page){
		dirty = false;
	}
};

class FileBuffer{
public:
	FileBuffer();
	~FileBuffer();
	void pop();
	void push(rowID id, Node* buffer);
	void push(TYPE_ID FileID, TYPE_ID PageID, Node* buffer);
	void flush();
	Node* findPage(rowID id);
	Node* findPage(TYPE_ID FileID, TYPE_ID PageID);
	Node* readPage(TYPE_ID pageId, char* path);
	void insertData(char* tablename, recordEntry record);
	void deleteData(char* tablename, TYPE_ID pageid, TYPE_OFFSET offset, int recordlength);
	void updateData(char* tablename, TYPE_ID pageid, TYPE_OFFSET offset, recordEntry record);
	void searchData(char* tablename);
private:
	map<rowID, Node*> bufmap;
};


#endif