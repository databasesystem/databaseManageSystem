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