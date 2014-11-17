#include "node.h"
#include "fileManage.h"
#include <fstream>

FileBuffer::FileBuffer(int max){
	header = new Node();
	trailor = new Node();
	header -> next = trailor;
	trailor -> pre = header;
	
	maxpagenum = max;
	usepagenum = 0;
}


Node* FileBuffer::remove(){
	if (usepagenum <= 0) return NULL;

	Node* cur = trailor -> pre;
	(cur -> pre) -> next = trailor;
	trailor -> pre = cur -> pre;
	
	if (cur -> dirty) FileManage::writePageToFile(cur -> pageid, cur -> data, filename);

	bufmap.erase(cur -> pageid);
	usepagenum --;

	return cur;
}

void FileBuffer::insert(Node* cur){
	if (usepagenum == maxpagenum) {
		cout << "buffer is full" << endl;
		remove();
		insert(cur);
		return;
	}

	cur -> next = header -> next;
	cur -> pre = header;
	header -> next = cur;
	(cur -> next) -> pre = cur;

	bufmap.insert(map<int, Node*>::value_type(cur -> pageid, cur));

	usepagenum ++;
}

Node* FileBuffer::find(int pageid){
	Node* cur = NULL;
	map<int, Node*>::iterator it = bufmap.find(pageid);
	if(it != bufmap.end())
		cur = it ->second;
	return cur;
}