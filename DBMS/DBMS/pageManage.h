#ifndef _PAGEMANAGE_H
#define _PAGEMANAGE_H

#include "systemManage.h"
#include <fstream>
#include <direct.h>
using namespace std;

typedef pair<TYPE_ID,TYPE_ID> rowID;	//<FileID, PageID>

struct pageHeader {
	TYPE_ID fileId;
	TYPE_ID pageId;
	TYPE_OFFSET firstFreeOffset;
	TYPE_OFFSET freeCount;
};

struct RecordEntry{
	BYTE **item;	// array of every item in bytes
	BYTE *length;
};

struct Page {
	pageHeader header;
	char data[PAGE_SIZE];
	Page() {
		memset(data, MEMSET_NUM, PAGE_SIZE);
	}
};

struct Node{
	bool dirty;
	Page* page;
	Node() : dirty(false),page(NULL){}
	Node(Page* _page) : dirty(false), page(_page){}
	Node(TYPE_ID fileID, TYPE_ID pageID) : dirty(true){
		page = new Page();
		page->header.fileId = fileID;
		page->header.pageId = pageID;
		page->header.firstFreeOffset = 0;
		page->header.freeCount = PAGE_SIZE;
	}
	~Node(){
		delete page;
	}
};

class BufManager{
public:
	BufManager();
	~BufManager();
	void pop();
	void push(TYPE_ID FileID, TYPE_ID PageID, Node* buffer);
	void flush();
	Node* findPage(TYPE_ID FileID, TYPE_ID PageID);
	Node* readPage(TYPE_ID pageId, const char* path);
	void insertData(string tablename, RecordEntry &record);
	void deleteData(string tablename, TYPE_ID pageid, TYPE_OFFSET offset, int recordlength);
	void updateData(string tablename, TYPE_ID pageid, TYPE_OFFSET offset, RecordEntry record);
	void writePageToFile(TYPE_ID pageid, Page* page, const char* filename);
	int	readPageFromFile(TYPE_ID pageid, Page* page, const char* filename);
private:
	map<rowID, Node*> bufmap;
};
#endif