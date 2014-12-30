#ifndef __NODE__H__
#define __NODE__H__

#include "fileManage.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>

using namespace std;

typedef pair<UINT,UINT> rowID;

class FileBuffer;
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


//针对每个文件一个缓存
class FileBuffer{
public:
	FileBuffer();
	Node* remove();				//从队列的尾部删掉，并从map中删掉，usepagenum --
	void insert(rowID id, Node* buffer);		//插入队首，插入map，usepagenum ++
	Node* find(rowID id);		//存在，返回节点；否则，返回NULL
	Node* find(UINT FileID, UINT PageID);
private:
	int numPage;
	map<rowID, Node*> bufmap;		//能够根据rowid快速找到Node
};


#endif