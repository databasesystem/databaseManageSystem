#include "dbManage.h"

DBManager::DBManager(string dbname) {
	this->dbName = dbname;
	sysManager.setName(dbname);
	if(_mkdir(dbName.c_str()) == -1){
		sysManager.loadSysfile();
	}
}

DBManager::~DBManager() {
	flush();
}

void DBManager::flush(){
	while(bufmap.size() > 0)
		pop();
}

void DBManager::createDataBase(string databaseName) {
	if(_mkdir(databaseName.c_str()) == -1){
		cout << "Database " << databaseName << " has already existed!" << endl;
		return;
	}
}

bool DBManager::switchDataBase(string databaseName){
	if(_chdir(databaseName.c_str())){
		cout << "Database " << databaseName << " does not exist." << endl;
		return false;
	}
	_chdir("../");
	flush();
	sysManager.flush();
	this->dbName = databaseName;
	sysManager.setName(databaseName);
	sysManager.loadSysfile();
	return true;
}

void DBManager::dropDataBase(string databaseName){
	string name = dbName;
	if( databaseName != dbName ){
		if(!switchDataBase(databaseName)){
			return;
		}
	}
	if(_rmdir(databaseName.c_str())){
		if(_chdir(databaseName.c_str())){
			cout << "Database " << databaseName << " does not exist." << endl;
		} else {
			_chdir("../");
			//handle every file after getting all table name from SysObject
			vector<string> tables = sysManager.getAllTable();
			for(auto table : tables){
				remove(getTablePath(table).c_str());
			}
			remove(getTablePath("_sysFile").c_str());
			remove(getTablePath("_sysFile.tmp").c_str());
			_rmdir(databaseName.c_str());
		}
	}
	cout << "Database " << databaseName << " has been successfully deleted." << endl;
	sysManager.drop();
	if( databaseName != name ){
		switchDataBase(name);
	}
}

void DBManager::createTable(string tablename, UINT colNum, string colName[], BYTE type[], TYPE_OFFSET length[], bool nullable[]){
	if(!sysManager.createTable(tablename)){
		return;
	}
	SysObject* table = sysManager.findTable(tablename);
	for(UINT i = 0, j = 1; i < colNum; j = j + 1 + length[i], i++){
		sysManager.insertColumn(colName[i], tablename, type[i], length[i], nullable[i], j);
	}
}

void DBManager::dropTable(string tableName){
	if(sysManager.dropTable(tableName))
		remove(getTablePath(tableName).c_str());
}

bool DBManager::insertRecord(RecordEntry *input, string colName[], string tableName){
	//get full record
	char record[PAGE_SIZE];
	memset(record, MEMSET_NUM, PAGE_SIZE);
	int index = 0;
	SysObject* table = sysManager.findTable(tableName);
	for( UINT i = 0; i < table->vecCols.size(); i++ ){
		SysColumn* column = sysManager.findColumn(colName[i], tableName);
		if( input->item[i] != NULL ){//!isNull
			memset(record+column->index-1, 0, 1);
			if( column->xtype == 4 ){	//string
				memset(record+column->index, 0, column->length);
			}
			dataUtility::bytefillbyte(record, input->item[i], column->index, input->length[i]);
		}
		if(index < column->index + column->length){
			index = column->index + column->length;
		}
	}
	dataUtility::data_fill_char<TYPE_OFFSET>(record, EXIST_INDEX, index, SIZE_OFFSET);
	index += SIZE_OFFSET;
	return insertData(table, record, index);
}

bool DBManager::updateRecord(string tableName,BYTE **Value,string *colName,BYTE *type,BYTE *len,BYTE *op, BYTE condCnt){
	SysObject* table = sysManager.findTable(tableName);
	/*
	Node* dataPage = readPage(table->id, );
	dataUtility::bytefillbyte(dataPage->page->data, record.getRecord(), offset, record.length);
	dataPage->dirty = true;*/
	return false;
}

bool DBManager::deleteRecord(string tableName,BYTE **Value,string *colName,BYTE *type,BYTE *len,BYTE *op, BYTE condCnt){
	SysObject* table = sysManager.findTable(tableName);
	return false;
}

string *DBManager::getColName(string tableName, USRT &colNum){
	SysObject* table = sysManager.findTable(tableName);
	SysColumn* column = sysManager.findColumn(table->vecCols[colNum]);
	return &(column->name);
}

vector<RecordEntry*> findRecord(string tableName,BYTE **Value,string *colName, BYTE *type, BYTE *len,BYTE *op, BYTE condCnt){
	vector<RecordEntry*> result;
	return result;
}

void DBManager::printDatabase(){
	sysManager.print();
}

void DBManager::printTable(string tableName){
	sysManager.printTable(tableName);
}

void DBManager::printTables(){
	sysManager.printTables(dbName);
}

vector<SysColumn*> DBManager::getTableAttr(string tableName) {
	return sysManager.getTableAttr(tableName);
}

string DBManager::getTablePath(string tableName) {
	return dbName + "/" + tableName;
}

void DBManager::pop(){
	if (bufmap.empty()) return;
	auto it = bufmap.begin();
	Node* bufPage = it->second;
	if ( bufPage->dirty ) {
		DBManager::writePageToFile(it->first.second, bufPage->page, sysManager.findTableName(it->first.first) );
	}
	delete bufPage;
	bufmap.erase(it);
}

void DBManager::push(TYPE_ID FileID, TYPE_ID PageID, Node* buffer){
	if ( bufmap.size() == DB_MAX_BUFFER_SIZE ) 
		pop();
	bufmap[pair<TYPE_ID, TYPE_ID>(FileID, PageID)] = buffer;
}

Node* DBManager::findPage(TYPE_ID FileID, TYPE_ID PageID){
	auto it = bufmap.find(pair<TYPE_ID,TYPE_ID>(FileID,PageID));
	return (it==bufmap.end())?NULL:it->second;
}

Node* DBManager::readPage(TYPE_ID FileID, TYPE_ID PageID){
	Node* pageNode = findPage(FileID, PageID);
	if( pageNode )
		return pageNode;

	Page* newPage = new Page();
	if(!DBManager::readPageFromFile(PageID, newPage, sysManager.findTableName(FileID))){
		delete newPage;
		return NULL;
	}
	Node* newNode = new Node(newPage);
	push(FileID,PageID,newNode);
	return newNode;
}

bool DBManager::insertData(SysObject* table, char record[], int index){
	//insert record
	Node* dataPage = NULL;
	TYPE_ID pageID = 0;
	while(pageID < TABLE_MAX_FILE_SIZE) {
		dataPage = readPage(table->id, pageID);
		if( dataPage == NULL ){
			dataPage = new Node(table->id, pageID);
			push(table->id, pageID, dataPage);
			break;
		} else {
			if (dataPage->page->header.freeCount > index){
				dataPage->dirty = true;
				break;
			} else {
				pageID++;
			}
		}
	}
	if( pageID == TABLE_MAX_FILE_SIZE ){
		cout << "Records in table " << table->name << " has reached maximum." << endl;
		return false;
	}
	Page* des = dataPage->page;
	TYPE_OFFSET firstoffset = dataUtility::char_to_data<TYPE_OFFSET>(des->data+des->header.firstFreeOffset+index-SIZE_OFFSET);
	dataUtility::bytefillbyte(des->data, (BYTE*)record, des->header.firstFreeOffset, index);
	des->header.freeCount -= index;
	if (firstoffset == EXIST_INDEX) {
		if (des->header.freeCount < index)
			des->header.firstFreeOffset = EXIST_INDEX;
		else
			des->header.firstFreeOffset = PAGE_SIZE - des->header.freeCount;
	} else {
		des->header.firstFreeOffset = firstoffset;
	}
	return true;
}

bool DBManager::deleteData(SysObject* table, TYPE_ID pageid, TYPE_OFFSET offset, int recordlength) {
	if (offset > PAGE_SIZE)
		return false;
	Node* dataPage = readPage(table->id, pageid);
	if( dataPage == NULL ){
		cout << "************Error in reading page with pageID: " << pageid << " and Action aborted**************" << endl;
		return false;
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
				TYPE_OFFSET firstoffset = dataUtility::char_to_data<TYPE_OFFSET>(des->data+firstOffset+recordlength-SIZE_OFFSET);
			}
			char *offsetChar = dataUtility::data_to_char<TYPE_OFFSET>(offset);
			dataUtility::bytefillbyte(des->data, (BYTE*)offsetChar, linkedOffset+recordlength-SIZE_OFFSET, SIZE_OFFSET);
			delete[] offsetChar;
		} else {						//Removed data become first free slot
			des->header.firstFreeOffset = offset;
			if( firstOffset == PAGE_SIZE - des->header.freeCount ){
				firstOffset = EXIST_INDEX;
			}
		}
		char *offsetChar = dataUtility::data_to_char<TYPE_OFFSET>(firstOffset);
		dataUtility::bytefillbyte(des->data, (BYTE*)offsetChar, offset+recordlength-SIZE_OFFSET, SIZE_OFFSET);
		delete[] offsetChar;
	}
	des->header.freeCount += recordlength;
	dataPage->dirty = true;
	return true;
}

void DBManager::writePageToFile(TYPE_ID pageid, Page* pagedata, string tablename){
	FILE* fileStream;
	fileStream = fopen(getTablePath(tablename).c_str(),"rb+");
	if (fileStream) {
		fseek(fileStream, pageid*DB_PGSIZE, SEEK_SET);
	} else {
		fileStream = fopen(getTablePath(tablename).c_str(), "wb+");
	}
	fwrite(pagedata, DB_PGSIZE, sizeof(char), fileStream);
	fclose(fileStream);
}

bool DBManager::readPageFromFile(TYPE_ID pageid, Page* pagedata, string tablename){
	FILE* fileStream;
	fileStream = fopen(getTablePath(tablename).c_str(),"rb");
	if( fileStream ){
		if (fseek(fileStream, DB_PGSIZE*pageid, SEEK_SET) || !fread(pagedata, DB_PGSIZE, sizeof(char), fileStream)) {
			fclose(fileStream);
			return false;
		} else {
			fclose(fileStream);
			return true;
		}
	} else {
		return false;
	}
}