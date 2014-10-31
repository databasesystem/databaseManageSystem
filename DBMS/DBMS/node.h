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


//���ÿ���ļ�һ������
class FileBuffer{
	Node *header, *trailor;
	char* filename;
	int maxpagenum;
	int usepagenum;
	map<int, Node*> bufmap;		//�ܹ�����pageid�����ҵ�Node
public:
	FileBuffer(int max);
	Node* remove();				//�Ӷ��е�β��ɾ��������map��ɾ����usepagenum --
	void insert(Node* cur);		//������ף�����map��usepagenum ++
	Node* find(int pageid);		//���ڣ����ؽڵ㣻���򣬷���NULL
};


#endif