#include "dbManage.h"

DBStorage::DBStorage(string dbname) {
	this->dbName = dbname;
	_mkdir(dbName.c_str());
}

DBStorage::~DBStorage() {
}

void DBStorage::createDataBase(string databaseName) {
	if(_mkdir(databaseName.c_str()) == -1){
		cout << "Database " << databaseName << " has already existed!" << endl;
		return;
	}
	//initialize sys files
}

void DBStorage::switchDataBase(string databaseName){
	/*if(_chdir(databaseName.c_str())){
		cout << "Database " << databaseName << " does not exist." << endl;
	} 
	_chdir("../");
	bufferManager.flush();
	this->dbName = databaseName;
	const char* path = getTablePath("_sysfile");
	if(	bufferManager.readPage(INDEX_SYSOBJECT, path)&&
		bufferManager.readPage(INDEX_SYSINDEX , path)&&
		bufferManager.readPage(INDEX_SYSCOLUMN, path)) {
		cout << "Current database is " << dbName << "." << endl;
	} else {
		cout << "Error in opening database " << dbName << " due to sysfile damages. Please delete the whole folder and start again." << endl;
		exit(0);
	}*/
}

void DBStorage::dropDataBase(string databaseName){
	if(_rmdir(databaseName.c_str())){
		if(_chdir(databaseName.c_str())){
			cout << "Database " << databaseName << " does not exist." << endl;
			return;
		} else {
			_chdir("../");
			//handle every file after getting all table name from SysObject
			//for(auto table : systemManager.
			_rmdir(databaseName.c_str());
		}
	}
	cout << "Database " << databaseName << " has been successfully deleted." << endl;
}

void DBStorage::createTable(string tablename, UINT colNum, string colName[], BYTE type[], TYPE_OFFSET length[], bool nullable[]){
	if(!systemManager.createTable(tablename)){
		return;
	}
	SysObject* table = systemManager.findTable(tablename);
	for(UINT i = 0, j = 0; i < colNum; j+=length[i], i++){
		systemManager.insertColumn(colName[i], tablename, type[i], length[i], nullable[i], j);
	}
	//Page* sysCol = new Page(table->id);
	//const char* path = getTablePath(tablename);
	//memcpy(attrPage->data, &attribute, sizeof(tableAttr));
	//FileBuffer::writePageToFile(attrPage->header.pageId, attrPage, path);
	//delete attrPage;
}

void DBStorage::dropTable(string tableName){
	const char* path = getTablePath(tableName);
	if(systemManager.dropTable(tableName))
		remove(path);
}

void DBStorage::printFreeList(string tablename, int pageid, int recordlength) {
}

void DBStorage::insertRecord(RecordEntry *input, string colName[], string tableName){
}

void DBStorage::deleteRecord(string tableName,BYTE **Value,string *colName,BYTE *type,BYTE *len,BYTE *op, BYTE condCnt){
}	

void DBStorage::flushBuffer(){
	//bufferManager.flush();
}

void DBStorage::print(){
	systemManager.print();
}

const char* DBStorage::getTablePath(string tableName) {
	string path = "./" + dbName + "/" + tableName;
	return path.c_str();
}
