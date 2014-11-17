#ifndef _FILEMANAGE_H
#define _FILEMANAGE_H
#include "globalVariable.h"
class FileManage {
public:
	static void writePageToFile(int pageid, char data[PAGE_SIZE], char* filename);
	static void readPageFromFile(int pageid, char data[PAGE_SIZE], char* filename);
};
#endif