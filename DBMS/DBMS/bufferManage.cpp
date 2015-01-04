#include "bufferManage.h"

FileBuffer::FileBuffer(){
}

FileBuffer::~FileBuffer(){
	flush();
}

void FileBuffer::pop(){
	if (bufmap.empty()) return;

	map<rowID, Node*>::iterator it = bufmap.begin();
	Node* bufPage = it->second;

	if ( bufPage->dirty ) {
		//need amendments after finishing SysObject (linking fileID and file name)
		//FileManage::writePageToFile(it->first.second, bufPage -> page, bufPage->page->header.fileId );
		FileManage::writePageToFile(it->first.second, bufPage->page, TESTTABLEPATH );
	}

	delete bufPage->page;
	bufmap.erase(it);
}

void FileBuffer::push(rowID id, Node* buffer){
	if ( bufmap.size() == DB_MAX_BUFFER_SIZE ) {
		pop();
	}
	bufmap[id] = buffer;
}

void FileBuffer::push(TYPE_ID FileID, TYPE_ID PageID, Node* buffer){
	push(pair<TYPE_ID, TYPE_ID>(FileID, PageID), buffer);
}

void FileBuffer::flush(){
	while(bufmap.size() > 0){
		pop();
	}
}

Node* FileBuffer::findPage(rowID id){
	map<rowID, Node*>::iterator it = bufmap.find(id);
	return (it==bufmap.end())?NULL:it->second;
}

Node* FileBuffer::findPage(TYPE_ID FileID, TYPE_ID PageID){
	return findPage(pair<TYPE_ID,TYPE_ID>(FileID,PageID));
}

Node* FileBuffer::readPage(TYPE_ID pageId, char* path){
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
	push(pair<TYPE_ID,TYPE_ID>(header->fileId,header->pageId),newNode);
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
	Node* attrPage = readPage( ATTR_PAGE_NUM, path ), *dataPage;
	TYPE_ID fileid = attrPage->page->header.fileId;
	tableAttr* tableAttribute = (tableAttr*)(attrPage->page->data);
	TYPE_ID pageid = ATTR_PAGE_NUM + 1;
	
	while(true) {
		if (tableAttribute->pagenum <= pageid){
			dataPage = new Node();
			dataPage->dirty = true;
			dataPage->page = new dbPage();
			dataPage->page->header.pageId = pageid; //Çì½ã¸øÄã¹òÁË=.=
			dataPage->page->header.fileId = fileid;
			dataPage->page->header.rowCount = 0;
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

	char* temp = dataUtility::getbyte(dataPage->page->data, dataPage->page->header.firstFreeOffset+record.length-SIZE_OFFSET, SIZE_OFFSET);
	int firstoffset = dataUtility::char2short(temp);
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
	cout << "write into page id: " << pageid << " after update firstoffset is " << dataPage->page->header.firstFreeOffset << endl;
	//cout << "************************End Insert Data**********************" << endl;
	delete[] temp;
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
				char* srcTemp = dataUtility::getbyte(dataPage->page->data, firstOffset+recordlength-SIZE_OFFSET, SIZE_OFFSET);
				linkedOffset = firstOffset;
				firstOffset = dataUtility::char2short(srcTemp);
				delete[] srcTemp;
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
	cout << " after update firstoffset is : " << dataPage->page->header.firstFreeOffset << endl;
}

void FileBuffer::updateData(char* tablename, TYPE_ID pageid, TYPE_OFFSET offset, recordEntry record) {
	//char* path = getTablePath(tablename);  need amendments after finishing SysObject (linking fileID and file name)
	char*path = TESTTABLEPATH;
	Node* dataPage = readPage(pageid, path);
	dataUtility::bytefillbyte(dataPage->page->data, record.getRecord(&record), offset, record.length);
	dataPage->dirty = true;
}