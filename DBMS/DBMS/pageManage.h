#ifndef _PAGEMANAGE_H
#define _PAGEMANAGE_H

#include "globalVariable.h"
#include "data_utility.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>
#include <direct.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

#define TESTTABLEPATH "./studentManage/studentinfo"
#define TESTFILEID 1
typedef pair<USRT,USRT> rowID;	//<FileID, PageID>

struct tableAttr{
	TYPE_ID pageNum;	//Number of page in table
	USRT colNum;		//Number of columns
	UINT colIsNull;		//Boolean for each column
	string* colName;	//Name of columns
	BYTE* colType;		//0-int 1-double 2-float 3-char 4-varchar 5-boolean
	USRT* colLen;		//Length of each column
	tableAttr(USRT num){
		colNum = num;
		pageNum = 0;
		colIsNull = 0;
		colName = new string[colNum];
		colType = new BYTE[colNum];
		colLen	= new USRT[colNum];
	}
};

struct pageHeader {
	TYPE_ID fileId;
	TYPE_ID pageId;
	TYPE_OFFSET firstFreeOffset;
	TYPE_OFFSET freeCount;
};

struct recordEntry {
	int num;
	USRT length;
	UINT isNull;
	TYPE_OFFSET offset;
	int* itemlen;
	char** item;  //C++ byte is defined for unsigned char
	recordEntry(int num_v, USRT* itemlen_v){
		num = num_v;
		length = sizeof(UINT) + SIZE_OFFSET;
		isNull = 0;
		itemlen = new int[num];
		item = new char*[num];
		for (int i = 0; i < num; i++) {
			itemlen[i] = itemlen_v[i]; 
			item[i] = new char[itemlen[i]];
			length = length + itemlen[i];
		}
		offset = EXIST_INDEX;
	}
	char* getRecord(){
		char* res = new char[length+1];
		char* temp = dataUtility::data_to_char<UINT>(isNull);
		dataUtility::bytefillbyte(res, temp, 0, sizeof(UINT));
		delete[] temp;

		int index = sizeof(UINT);
		for (int i = 0; i < num; i++) {
			dataUtility::bytefillbyte(res, item[i],index);
			index = index + itemlen[i];
		}
		
		temp = dataUtility::data_to_char<TYPE_OFFSET>(offset);
		dataUtility::bytefillbyte(res, temp, index, SIZE_OFFSET);
		res[length] = 0;
		delete[] temp;
		return res;
	}
};

struct dbPage {
	pageHeader header;
	char data[PAGE_SIZE];
	dbPage() {
		memset(data, MEMSET_NUM, PAGE_SIZE);
	}
	dbPage(TYPE_ID fileID){
		memset(data, MEMSET_NUM, PAGE_SIZE);
		header.pageId = ATTR_PAGE_NUM;
		header.fileId = fileID;
		header.firstFreeOffset = sizeof(tableAttr);
		header.freeCount = PAGE_SIZE-sizeof(tableAttr);
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
	Node(TYPE_ID fileID, TYPE_ID pageID){
		dirty = true;
		page = new dbPage();
		page->header.fileId = fileID;
		page->header.pageId = pageID;
		page->header.firstFreeOffset = 0;
		page->header.freeCount = PAGE_SIZE;
	}
	~Node(){
		delete page;
	}
};

class FileBuffer{
public:
	FileBuffer();
	~FileBuffer();
	void pop();
	void push(TYPE_ID FileID, TYPE_ID PageID, Node* buffer);
	void flush();
	Node* findPage(TYPE_ID FileID, TYPE_ID PageID);
	Node* readPage(TYPE_ID pageId, char* path);
	void insertData(char* tablename, recordEntry record);
	void deleteData(char* tablename, TYPE_ID pageid, TYPE_OFFSET offset, int recordlength);
	void updateData(char* tablename, TYPE_ID pageid, TYPE_OFFSET offset, recordEntry record);
	void searchData(char* tablename);
	static void writePageToFile(TYPE_ID pageid, dbPage* pageinfo, char* filename);
	static int readPageFromFile(TYPE_ID pageid, dbPage* pageinfo, char* filename);
private:
	map<rowID, Node*> bufmap;
};
#endif