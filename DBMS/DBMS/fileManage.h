#ifndef _FILEMANAGE_H
#define _FILEMANAGE_H
#include "pageManage.h"
#include <string>
class FileManage {
public:
	static void writePageToFile(int pageid, dbPage pageinfo, char* filename);
	static void readPageFromFile(int pageid, dbPage* pageinfo, char* filename);
	static int createFileFolder(char* filefoldername);
	static int deleteFileFolder(char* filefoldername);
};
#endif