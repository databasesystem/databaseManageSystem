#ifndef _FILEMANAGE_H
#define _FILEMANAGE_H
#include "pageManage.h"
#include <direct.h>

class FileManage {
public:
	static void writePageToFile(TYPE_ID pageid, dbPage* pageinfo, char* filename);
	static void readPageFromFile(TYPE_ID pageid, dbPage* pageinfo, char* filename);
	static int createFileFolder(char* filefoldername);
	static int deleteFileFolder(char* filefoldername);
};
#endif