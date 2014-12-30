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


//���ÿ���ļ�һ������
class FileBuffer{
public:
	FileBuffer();
	Node* remove();				//�Ӷ��е�β��ɾ��������map��ɾ����usepagenum --
	void insert(rowID id, Node* buffer);		//������ף�����map��usepagenum ++
	Node* find(rowID id);		//���ڣ����ؽڵ㣻���򣬷���NULL
	Node* find(UINT FileID, UINT PageID);
private:
	int numPage;
	map<rowID, Node*> bufmap;		//�ܹ�����rowid�����ҵ�Node
};


#endif