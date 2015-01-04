#include "bufferManage.h"

FileBuffer::FileBuffer(){
	numPage = 0;
}

FileBuffer::~FileBuffer(){
	while(numPage > 0){
		pop();
	}
}

int FileBuffer::size(){
	return bufmap.size();
}

void FileBuffer::pop(){
	if (!numPage) return;

	map<rowID, Node*>::iterator it = bufmap.begin();
	Node* bufPage = it->second;

	if ( bufPage->dirty ) {
		//need amendments after finishing SysObject (linking fileID and file name)
		//FileManage::writePageToFile(it->first.second, bufPage -> page, bufPage->page->header.fileId );
		FileManage::writePageToFile(it->first.second, bufPage->page, TESTTABLEPATH );
	}

	delete bufPage->page;
	bufmap.erase(it);
	numPage--;
}

void FileBuffer::push(rowID id, Node* buffer){
	if ( numPage == DB_MAX_BUFFER_SIZE ) {
		pop();
	}
	bufmap[id] = buffer;
	numPage++;
}

void FileBuffer::push(int FileID, int PageID, Node* buffer){
	push(pair<int, int>(FileID, PageID), buffer);
}

void FileBuffer::flush(){
	while(numPage > 0){
		pop();
	}
}

Node* FileBuffer::findPage(rowID id){
	map<rowID, Node*>::iterator it = bufmap.find(id);
	return (it==bufmap.end())?NULL:it->second;
}

Node* FileBuffer::findPage(int FileID, int PageID){
	return findPage(pair<int,int>(FileID,PageID));
}

Node* FileBuffer::readPage(int pageId, char* path){
	//need amendments after finishing SysObject (linking fileID and file name)
	Node* pageNode = findPage(TESTFILEID, pageId);
	if( pageNode ){
		return pageNode;
	}

	dbPage* newPage = new dbPage();
	FileManage::readPageFromFile(pageId, newPage, path);
	pageHeader* header = &(newPage->header);
	if(header->fileId == 0){
		delete newPage;
		return NULL;
	}

	Node* newNode = new Node(newPage);
	push(pair<int,int>(header->fileId,header->pageId),newNode);
	return newNode;
}

void FileBuffer::insertData(char* tablename, recordEntry record) {
	/*
	*INSERT INTO publisher VALUES 
	*(100008,'Oxbow Books Limited','PRC');
	*/

	//cout << "************************Start Insert Data**********************" << endl;
	//cout << "insertData--data length: " << record.length << endl;
	//cout << "Data length" << record.length << endl;
	
	//char* path = getTablePath(tablename);  need amendments after finishing SysObject (linking fileID and file name)
	char* path = TESTTABLEPATH;
	Node* attrPage = readPage(0,path), *dataPage;
	int fileid = attrPage->page->header.fileId;
	tableAttr* tableAttribute = (tableAttr*)(attrPage->page->data);
	int pageid = 1;
	
	while(true) {
		if (tableAttribute->pagenum <= pageid){
			dataPage = new Node();
			dataPage->dirty = true;
			dataPage->page = new dbPage();
			dataPage->page->header.pageId = pageid; //Çì½ã¸øÄã¹òÁË=.=
			dataPage->page->header.fileId = fileid;
			dataPage->page->header.firstFreeOffset = 0;
			dataPage->page->header.freeCount = PAGE_SIZE;
			push(fileid, pageid, dataPage);

			tableAttribute->pagenum++;
			attrPage->dirty = true;
			break;
		} else {
			dataPage = readPage(pageid, path);
			if (dataPage->page->header.freeCount >= record.length){
				dataPage->dirty = true;
				break;
			} else {
				pageid++;
			}
		}
	}
	char* data = new char[record.length];
	data = record.getRecord(&record);

	char* temp = dataUtility::getbyte(dataPage->page->data, dataPage->page->header.firstFreeOffset+record.length-sizeof(int), sizeof(int));
	int firstoffset = dataUtility::char2int(temp);
	dataUtility::bytefillbyte(dataPage->page->data, data, dataPage->page->header.firstFreeOffset, record.length);
	dataPage->page->header.freeCount -= record.length;
	if (firstoffset == -1) {
		if (dataPage->page->header.freeCount < record.length)
			dataPage->page->header.firstFreeOffset = -1;
		else
			dataPage->page->header.firstFreeOffset = PAGE_SIZE - dataPage->page->header.freeCount;
	} else {
		dataPage->page->header.firstFreeOffset = firstoffset;
	}
	cout << "write into page id: " << pageid << " after update firstoffset is " << dataPage->page->header.firstFreeOffset << endl;
	//cout << "************************End Insert Data**********************" << endl;
	delete[] temp;
	delete[] data;
}

void FileBuffer::deleteData(char* tablename, int pageid, int offset, int recordlength) {
	if (offset > PAGE_SIZE)
		return;
	//char* path = getTablePath(tablename);  need amendments after finishing SysObject (linking fileID and file name)
	char* path = TESTTABLEPATH;
	Node* dataPage = readPage(pageid, path);
	if( dataPage == NULL ){
		cout << "************Error in reading page with pageID: " << pageid << " and Action aborted**************" << endl;
		return;
	}
	int firstOffset = dataPage->page->header.firstFreeOffset;
	if( firstOffset == -1 ){			//Full page
		dataPage->page->header.firstFreeOffset = offset;
	} else {
		if( firstOffset < offset ){		//Handle linked list sequence
			int linkedOffset;
			while ( firstOffset < offset && firstOffset != -1 ){
				char* srcTemp = dataUtility::getbyte(dataPage->page->data, firstOffset+recordlength-sizeof(int), sizeof(int));
				linkedOffset = firstOffset;
				firstOffset = dataUtility::char2int(srcTemp);
				delete[] srcTemp;
			};
			char *offsetChar = dataUtility::data_to_char<int>(offset);
			dataUtility::bytefillbyte(dataPage->page->data, offsetChar, linkedOffset+recordlength-sizeof(int), sizeof(int));
			delete[] offsetChar;
		} else {						//Removed data become first free slot
			dataPage->page->header.firstFreeOffset = offset;
			if( firstOffset == PAGE_SIZE - dataPage->page->header.freeCount ){
				firstOffset = -1;
			}
		}
		char *offsetChar = dataUtility::data_to_char<int>(firstOffset);
		dataUtility::bytefillbyte(dataPage->page->data, offsetChar, offset+recordlength-sizeof(int), sizeof(int));
		delete[] offsetChar;
	}
	dataPage->page->header.freeCount += recordlength;
	dataPage->dirty = true;
	cout << " after update firstoffset is : " << dataPage->page->header.firstFreeOffset << endl;
}

void FileBuffer::updateData(char* tablename, int pageid, int offset, recordEntry record) {
	//char* path = getTablePath(tablename);  need amendments after finishing SysObject (linking fileID and file name)
	char*path = TESTTABLEPATH;
	Node* dataPage = readPage(pageid, path);
	dataUtility::bytefillbyte(dataPage->page->data, record.getRecord(&record), offset, record.length);
	dataPage->dirty = true;
}