#ifndef _PAGEMANAGE_H
#define _PAGEMANAGE_H

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

struct column {
	string name;
	int type;
	int length;
	bool null;    //1 : is null 0: not null
	bool primaryKey; // 1: is primary key ; 0 :not

	void printColumn() {
		cout << "name: " << name.c_str() << endl;
		cout << "type: " << type << endl;
		cout << "length: " << length << endl;
		cout << "null: " << null << endl;
		cout << "primary key: " << primaryKey << endl;
	}
	column(){
		name = "";
		type = -1;
		length = 0;
		null = 1;
		primaryKey = 0;
	}
};

struct tableColumn {
	USRT colNum;
	column* colInfo;
	tableColumn(USRT num) {
		colNum = num;
		colInfo = new column[num];
	}
	tableColumn() {
		colNum = 0;
	}
	int checkColName(string checkname) {
		for (int i = 0; i < colNum; i++) {
			if (colInfo[i].name.compare(checkname) == 0)
				return i;
		}
		return -1;
	}
};
#endif