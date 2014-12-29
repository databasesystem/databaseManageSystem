#include "node.h"

FileBuffer::FileBuffer(int max){
	head = new Node();
	pointer = new Node();
	head -> next = pointer;
	pointer -> pre = head;
	
	maxPage = max;
	usedPage = 0;
}

Node* FileBuffer::remove(){
	if (!usedPage) return NULL;

	Node* cur = pointer -> pre;
	(cur -> pre) -> next = pointer;
	pointer -> pre = cur -> pre;
	
	if (cur -> dirty) {
		//FileManage::writePageToFile(cur -> pageid, cur -> data, filename);
	}
	bufmap.erase(cur -> pageid);
	usedPage--;
	return cur;
}

void FileBuffer::insert(Node* cur){
	if ( usedPage == maxPage ) {
		cout << "buffer is full" << endl;
		remove();
		insert(cur);
		return;
	}

	cur -> next = head -> next;
	cur -> pre = head;
	head -> next = cur;
	(cur -> next) -> pre = cur;

	bufmap.insert(map<int, Node*>::value_type(cur -> pageid, cur));
	usedPage++;
}

Node* FileBuffer::find(int pageid){
	Node* cur = NULL;
	map<int, Node*>::iterator it = bufmap.find(pageid);
	if(it != bufmap.end())
		cur = it ->second;
	return cur;
}