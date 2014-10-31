#ifndef __NODE__H__
#define __NODE__H__

#include <cstdlib>
#include <iostream>
#include <map>

using namespace std;

class FileBuffer;
struct Node{
	char* data;
	int pageid;
	bool dirty;
	Node* next;
	Node* pre;
	Node(){
		data = NULL;
		pageid = 0;
		dirty = false;
		next = NULL;
		pre = NULL;
	}
	Node(char* _data, int _pageid) : data(_data), pageid(_pageid){
		dirty = false;
		next = NULL;
		pre = NULL;
	}
};


//针对每个文件一个缓存
class FileBuffer{
	Node *header, *trailor;
	char* filename;
	int maxpagenum;
	int usepagenum;
	map<int, Node*> bufmap;		//能够根据pageid快速找到Node
public:
	FileBuffer(int max);
	Node* remove();				//从队列的尾部删掉，并从map中删掉，usepagenum --
	void insert(Node* cur);		//插入队首，插入map，usepagenum ++
	Node* find(int pageid);		//存在，返回节点；否则，返回NULL
};


#endif