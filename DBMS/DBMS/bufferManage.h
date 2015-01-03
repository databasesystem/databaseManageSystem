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

using namespace std;

typedef pair<int,int> rowID;

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
		
		char* freeOffset = dataUtility::data_to_char<int>(record->offset);
		dataUtility::bytefillbyte(res, freeOffset, index, 4);
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
	void push(int FileID, int PageID, Node* buffer);
	void flush();
	Node* find(rowID id);
	Node* find(int FileID, int PageID);
	Node* readPage(int pageId, char* path);
	void insertData(char* tablename, recordEntry record);
	void deleteData(char* tablename, int pageid, int offset, int recordlength);
	void updateData(char* tablename, int pageid, int offset, recordEntry record);
private:
	int numPage;
	map<rowID, Node*> bufmap;
};


#endif