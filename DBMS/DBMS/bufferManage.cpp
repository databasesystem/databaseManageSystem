#include "bufferManage.h"

FileBuffer::FileBuffer(){
	numPage = 0;
}

Node* FileBuffer::remove(){
	if (!numPage) return NULL;

	map<rowID, Node*>::iterator it = bufmap.end();
	Node* bufPage = (--it)->second;

	if ( bufPage->dirty ) {
		//FileManage::writePageToFile(it->first.first, bufPage -> page
		//	, bufPage->page->header.fileId );
	}

	bufmap.erase(it);
	numPage--;
	return bufPage;
}

Node* FileBuffer::readPage(int pageId, char* path){
	//add find rowID after finishing fileid mapping
	dbPage* newPage = new dbPage();
	FileManage::readPageFromFile(pageId, newPage, path);
	pageHeader* header = &(newPage->header);
	if(header->fileId == 0){
		delete newPage;
		return NULL;
	}
	Node* newNode = new Node(newPage);
	insert(pair<UINT,UINT>(header->fileId,header->pageId),newNode);
	return newNode;
}

void FileBuffer::insert(rowID id, Node* buffer){
	if ( numPage == DB_MAX_BUFFER_SIZE ) {
		cout << "buffer is full" << endl;
		remove();
	}
	bufmap[id] = buffer;
	numPage++;
}

Node* FileBuffer::find(rowID id){
	map<rowID, Node*>::iterator it = bufmap.find(id);
	return (it==bufmap.end())?NULL:it->second;
}

Node* FileBuffer::find(UINT FileID, UINT PageID){
	return find(pair<UINT,UINT>(FileID,PageID));
}