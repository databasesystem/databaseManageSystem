#ifndef _PAGEMANAGE_H
#define _PAGEMANAGE_H
#include "globalVariable.h"
class pageHeader {
public:
	int pageId;
	int fileId;
	int rowCount;
	int firstFreeOffset;
	int freeCount;
};

class dbPage {
public:
	pageHeader header;
	char data[PAGE_SIZE];
};
#endif