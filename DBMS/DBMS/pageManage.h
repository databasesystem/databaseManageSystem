#ifndef _PAGEMANAGE_H
#define _PAGEMANAGE_H

#include "globalVariable.h"
#define PAGE_SIZE DB_PGSIZE-sizeof(pageHeader)

class pageHeader {
public:
	int pageId;
	int fileId;
	int rowCount;
	int freeCount;
	int firstFreeOffset;
};

class dbPage {
public:
	pageHeader header;
	char data[PAGE_SIZE];
	dbPage() {
		for (int i = 0; i < PAGE_SIZE; i++)
		{
			data[i]=' ';
		}
		data[PAGE_SIZE-1]='\0';//?
	}
};

class dbIndex {
public:
	pageHeader header;
	char data[PAGE_SIZE];
};
#endif