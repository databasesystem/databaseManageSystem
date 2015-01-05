#include "pageManage.h"

FileBuffer::FileBuffer(){
}

FileBuffer::~FileBuffer(){
	flush();
}

void FileBuffer::flush(){
	while(bufmap.size() > 0)
		pop();
}

void FileBuffer::pop(){
	if (bufmap.empty()) return;

	map<rowID, Node*>::iterator it = bufmap.begin();
	Node* bufPage = it->second;

	if ( bufPage->dirty ) {
		//need amendments after finishing SysObject (linking fileID and file name)
		//FileBuffer::writePageToFile(it->first.second, bufPage -> page, bufPage->page->header.fileId );
		FileBuffer::writePageToFile(it->first.second, bufPage->page, TESTTABLEPATH );
	}

	delete bufPage;
	bufmap.erase(it);
}

void FileBuffer::push(TYPE_ID FileID, TYPE_ID PageID, Node* buffer){
	if ( bufmap.size() == DB_MAX_BUFFER_SIZE ) 
		pop();
	bufmap[pair<TYPE_ID, TYPE_ID>(FileID, PageID)] = buffer;
}

Node* FileBuffer::findPage(TYPE_ID FileID, TYPE_ID PageID){
	map<rowID, Node*>::iterator it = bufmap.find(pair<TYPE_ID,TYPE_ID>(FileID,PageID));
	return (it==bufmap.end())?NULL:it->second;
}

Node* FileBuffer::readPage(TYPE_ID pageId, char* path){
	//need amendments after finishing SysObject (linking fileID and file name)
	Node* pageNode = findPage(TESTFILEID, pageId);
	if( pageNode )
		return pageNode;

	dbPage* newPage = new dbPage();
	if(!FileBuffer::readPageFromFile(pageId, newPage, path)){
		delete newPage;
		return NULL;
	}

	Node* newNode = new Node(newPage);
	push(((pageHeader*) newPage)->fileId,((pageHeader*) newPage)->pageId,newNode);
	return newNode;
}

void FileBuffer::insertData(char* tablename, recordEntry record) {
	//char* path = getTablePath(tablename);  need amendments after finishing SysObject (linking fileID and file name)
	char* path = TESTTABLEPATH;
	Node* attrPage = readPage( ATTR_PAGE_NUM, path ), *dataPage;
	TYPE_ID fileid = attrPage->page->header.fileId;
	tableAttr* tableAttribute = (tableAttr*)(attrPage->page->data);
	TYPE_ID pageid = ATTR_PAGE_NUM + 1;
	
	while(true) {
		if (tableAttribute->pageNum <= pageid){
			dataPage = new Node(fileid, pageid);
			push(fileid, pageid, dataPage);
			tableAttribute->pageNum++;
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
	int firstoffset = dataUtility::char2short(dataPage->page->data+dataPage->page->header.firstFreeOffset+record.length-SIZE_OFFSET);
	dataUtility::bytefillbyte(dataPage->page->data, data, dataPage->page->header.firstFreeOffset, record.length);
	dataPage->page->header.freeCount -= record.length;
	if (firstoffset == EXIST_INDEX) {
		if (dataPage->page->header.freeCount < record.length)
			dataPage->page->header.firstFreeOffset = EXIST_INDEX;
		else
			dataPage->page->header.firstFreeOffset = PAGE_SIZE - dataPage->page->header.freeCount;
	} else {
		dataPage->page->header.firstFreeOffset = firstoffset;
	}
	delete[] data;
}

void FileBuffer::deleteData(char* tablename, TYPE_ID pageid, TYPE_OFFSET offset, int recordlength) {
	if (offset > PAGE_SIZE)
		return;
	//char* path = getTablePath(tablename);  need amendments after finishing SysObject (linking fileID and file name)
	char* path = TESTTABLEPATH;
	Node* dataPage = readPage(pageid, path);
	if( dataPage == NULL ){
		cout << "************Error in reading page with pageID: " << pageid << " and Action aborted**************" << endl;
		return;
	}
	TYPE_OFFSET firstOffset = dataPage->page->header.firstFreeOffset;
	if( firstOffset == EXIST_INDEX ){			//Full page
		dataPage->page->header.firstFreeOffset = offset;
	} else {
		if( firstOffset < offset ){		//Handle linked list sequence
			TYPE_OFFSET linkedOffset;
			while ( firstOffset < offset && firstOffset != EXIST_INDEX ){
				linkedOffset = firstOffset;
				firstOffset = dataUtility::char2short(dataPage->page->data+firstOffset+recordlength-SIZE_OFFSET);
			};
			char *offsetChar = dataUtility::data_to_char<TYPE_OFFSET>(offset);
			dataUtility::bytefillbyte(dataPage->page->data, offsetChar, linkedOffset+recordlength-SIZE_OFFSET, SIZE_OFFSET);
			delete[] offsetChar;
		} else {						//Removed data become first free slot
			dataPage->page->header.firstFreeOffset = offset;
			if( firstOffset == PAGE_SIZE - dataPage->page->header.freeCount ){
				firstOffset = EXIST_INDEX;
			}
		}
		char *offsetChar = dataUtility::data_to_char<TYPE_OFFSET>(firstOffset);
		dataUtility::bytefillbyte(dataPage->page->data, offsetChar, offset+recordlength-SIZE_OFFSET, SIZE_OFFSET);
		delete[] offsetChar;
	}
	dataPage->page->header.freeCount += recordlength;
	dataPage->dirty = true;
	//cout << " after update firstoffset is : " << dataPage->page->header.firstFreeOffset << endl;
}

void FileBuffer::updateData(char* tablename, TYPE_ID pageid, TYPE_OFFSET offset, recordEntry record) {
	//char* path = getTablePath(tablename);  need amendments after finishing SysObject (linking fileID and file name)
	char*path = TESTTABLEPATH;
	Node* dataPage = readPage(pageid, path);
	dataUtility::bytefillbyte(dataPage->page->data, record.getRecord(), offset, record.length);
	dataPage->dirty = true;
}

void FileBuffer::writePageToFile(TYPE_ID pageid, dbPage* pagedata, char* filename){
	FILE* fileStream;
	fileStream = fopen(filename,"rb+");
	if (fileStream) {
		fseek(fileStream, pageid*DB_PGSIZE, SEEK_SET);
	} else {
		fileStream = fopen(filename, "wb+");
	}
	fwrite(pagedata, DB_PGSIZE, sizeof(char), fileStream);
	fclose(fileStream);
}

int FileBuffer::readPageFromFile(TYPE_ID pageid, dbPage* pageinfo, char* filename){
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