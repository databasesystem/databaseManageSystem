#include "pageManage.h"

BufManager::BufManager(){
}

BufManager::~BufManager(){
	//flush();
}
/*
void BufManager::flush(){
	while(bufmap.size() > 0)
		pop();
}

void BufManager::pop(){
	if (bufmap.empty()) return;
	auto it = bufmap.begin();
	Node* bufPage = it->second;
	if ( bufPage->dirty ) {
		//BufManager::writePageToFile(it->first.second, bufPage -> page, bufPage->page->header.fileId );
		string tableName = SysManager::findTableName(bufPage->page->header.fileId);
		BufManager::writePageToFile(it->first.second, bufPage->page, TESTTABLEPATH );
	}
	delete bufPage;
	bufmap.erase(it);
}

void BufManager::push(TYPE_ID FileID, TYPE_ID PageID, Node* buffer){
	if ( bufmap.size() == DB_MAX_BUFFER_SIZE ) 
		pop();
	bufmap[pair<TYPE_ID, TYPE_ID>(FileID, PageID)] = buffer;
}

Node* BufManager::findPage(TYPE_ID FileID, TYPE_ID PageID){
	auto it = bufmap.find(pair<TYPE_ID,TYPE_ID>(FileID,PageID));
	return (it==bufmap.end())?NULL:it->second;
}

Node* BufManager::readPage(TYPE_ID pageId, const char* path){
	//need amendments after finishing SysObject (linking fileID and file name)
	Node* pageNode = findPage(TESTFILEID, pageId);
	if( pageNode )
		return pageNode;

	Page* newPage = new Page();
	if(!BufManager::readPageFromFile(pageId, newPage, path)){
		delete newPage;
		return NULL;
	}

	Node* newNode = new Node(newPage);
	//need amendments after finishing SysObject (linking fileID and file name)
	push(((pageHeader*) newPage)->fileId,pageId,newNode);
	return newNode;
}

void BufManager::insertData(string tablename, RecordEntry &record) {
	//char* path = getTablePath(tablename);  need amendments after finishing SysObject (linking fileID and file name)
	char* path = TESTTABLEPATH;
	Node* attrPage = readPage( ATTR_PAGE_NUM, path ), *dataPage;
	TYPE_ID fileid = attrPage->page->header.fileId;
	tableAttr* tableAttribute = (tableAttr*)(attrPage->page->data);
	TYPE_ID pageid = ATTR_PAGE_NUM + 1;
	
	while(true) {
		if (tableAttribute->numPage <= pageid){
			dataPage = new Node(fileid, pageid);
			push(fileid, pageid, dataPage);
			tableAttribute->numPage++;
			attrPage->dirty = true;
			cout << "*****Opening a new page " << pageid << " *****" << endl;
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
	char* data = new char[record.length+1];
	data = record.getRecord();
	Page* des = dataPage->page;
	int firstoffset = dataUtility::char2short(des->data+des->header.firstFreeOffset+record.length-SIZE_OFFSET);
	dataUtility::bytefillbyte(des->data, data, des->header.firstFreeOffset, record.length);
	des->header.freeCount -= record.length;
	if (firstoffset == EXIST_INDEX) {
		if (des->header.freeCount < record.length)
			des->header.firstFreeOffset = EXIST_INDEX;
		else
			des->header.firstFreeOffset = PAGE_SIZE - des->header.freeCount;
	} else {
		des->header.firstFreeOffset = firstoffset;
	}
	delete[] data;
}

void BufManager::deleteData(string tablename, TYPE_ID pageid, TYPE_OFFSET offset, int recordlength) {
	if (offset > PAGE_SIZE)
		return;
	//char* path = getTablePath(tablename);  need amendments after finishing SysObject (linking fileID and file name)
	char* path = TESTTABLEPATH;
	Node* dataPage = readPage(pageid, path);
	if( dataPage == NULL ){
		cout << "************Error in reading page with pageID: " << pageid << " and Action aborted**************" << endl;
		return;
	}
	Page* des = dataPage->page;
	TYPE_OFFSET firstOffset = des->header.firstFreeOffset;
	if( firstOffset == EXIST_INDEX ){			//Full page
		des->header.firstFreeOffset = offset;
	} else {
		if( firstOffset < offset ){		//Handle linked list sequence
			TYPE_OFFSET linkedOffset;
			while ( firstOffset < offset && firstOffset != EXIST_INDEX ){
				linkedOffset = firstOffset;
				firstOffset = dataUtility::char2short(des->data+firstOffset+recordlength-SIZE_OFFSET);
			};
			char *offsetChar = dataUtility::data_to_char<TYPE_OFFSET>(offset);
			dataUtility::bytefillbyte(des->data, offsetChar, linkedOffset+recordlength-SIZE_OFFSET, SIZE_OFFSET);
			delete[] offsetChar;
		} else {						//Removed data become first free slot
			des->header.firstFreeOffset = offset;
			if( firstOffset == PAGE_SIZE - des->header.freeCount ){
				firstOffset = EXIST_INDEX;
			}
		}
		char *offsetChar = dataUtility::data_to_char<TYPE_OFFSET>(firstOffset);
		dataUtility::bytefillbyte(des->data, offsetChar, offset+recordlength-SIZE_OFFSET, SIZE_OFFSET);
		delete[] offsetChar;
	}
	des->header.freeCount += recordlength;
	dataPage->dirty = true;
	//cout << " after update firstoffset is : " << dataPage->page->header.firstFreeOffset << endl;
}

void BufManager::updateData(string tablename, TYPE_ID pageid, TYPE_OFFSET offset, RecordEntry record) {
	//char* path = getTablePath(tablename);  need amendments after finishing SysObject (linking fileID and file name)
	char*path = TESTTABLEPATH;
	Node* dataPage = readPage(pageid, path);
	dataUtility::bytefillbyte(dataPage->page->data, record.getRecord(), offset, record.length);
	dataPage->dirty = true;
}

void BufManager::writePageToFile(TYPE_ID pageid, Page* pagedata, const char* filename){
	FILE* fileStream;
	fileStream = fopen(filename,"rb+");
	if (fileStream) {
		fseek(fileStream, pageid*DB_PGSIZE, SEEK_SET);
	} else {
		fileStream = fopen(filename, "wb+");
	}
	fwrite(pagedata, DB_PGSIZE, sizeof(char), fileStream);
	fclose(fileStream);
}*/

int BufManager::readPageFromFile(TYPE_ID pageid, Page* pageinfo, const char* filename){
	FILE* fileStream;
	fileStream = fopen(filename,"rb");
	if (fseek(fileStream, pageid*DB_PGSIZE, SEEK_SET)) {
		cout << "Read page from file error: " << pageid << " filename: " << filename << endl;
		fclose(fileStream);
		return 0;
	} else {
		fread(pageinfo, DB_PGSIZE, sizeof(char), fileStream);
		fclose(fileStream);
		return 1;
	}
}