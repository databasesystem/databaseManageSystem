#include "dbManage.h"

DBManager::DBManager(string dbname) {
	this->dbName = dbname;
	sysManager.setName(dbname);
	if(_mkdir(dbName.c_str()) == -1){
		sysManager.loadSysfile();
	}
}

DBManager::~DBManager() {
}

void DBManager::createDataBase(string databaseName) {
	if(_mkdir(databaseName.c_str()) == -1){
		cout << "Database " << databaseName << " has already existed!" << endl;
		return;
	}
}

void DBManager::switchDataBase(string databaseName){
	/*if(_chdir(databaseName.c_str())){
		cout << "Database " << databaseName << " does not exist." << endl;
	} 
	_chdir("../");
	bufManager.flush();
	this->dbName = databaseName;
	const char* path = getTablePath("_sysfile");
	if(	bufManager.readPage(INDEX_SYSOBJECT, path)&&
		bufManager.readPage(INDEX_SYSINDEX , path)&&
		bufManager.readPage(INDEX_SYSCOLUMN, path)) {
		cout << "Current database is " << dbName << "." << endl;
	} else {
		cout << "Error in opening database " << dbName << " due to sysfile damages. Please delete the whole folder and start again." << endl;
		exit(0);
	}*/
}

void DBManager::dropDataBase(string databaseName){
	if(_rmdir(databaseName.c_str())){
		if(_chdir(databaseName.c_str())){
			cout << "Database " << databaseName << " does not exist." << endl;
			return;
		} else {
			_chdir("../");
			//handle every file after getting all table name from SysObject
			//for(auto table : SysManager.
			_rmdir(databaseName.c_str());
		}
	}
	cout << "Database " << databaseName << " has been successfully deleted." << endl;
}

void DBManager::createTable(string tablename, UINT colNum, string colName[], BYTE type[], TYPE_OFFSET length[], bool nullable[]){
	if(!sysManager.createTable(tablename)){
		return;
	}
	SysObject* table = sysManager.findTable(tablename);
	for(UINT i = 0, j = 0; i < colNum; j+=length[i], i++){
		sysManager.insertColumn(colName[i], tablename, type[i], length[i], nullable[i], j);
	}
	//Page* sysCol = new Page(table->id);
	//const char* path = getTablePath(tablename);
	//memcpy(attrPage->data, &attribute, sizeof(tableAttr));
	//BufManager::writePageToFile(attrPage->header.pageId, attrPage, path);
	//delete attrPage;
}

void DBManager::dropTable(string tableName){
	const char* path = getTablePath(tableName);
	if(sysManager.dropTable(tableName))
		remove(path);
}

void DBManager::printFreeList(string tablename, int pageid, int recordlength) {
}

void DBManager::insertRecord(RecordEntry *input, string colName[], string tableName){
}

void DBManager::deleteRecord(string tableName,BYTE **Value,string *colName,BYTE *type,BYTE *len,BYTE *op, BYTE condCnt){
}	

void DBManager::flushBuffer(){
	//bufManager.flush();
}

void DBManager::print(){
	sysManager.print();
}

const char* DBManager::getTablePath(string tableName) {
	string path = "./" + dbName + "/" + tableName;
	return path.c_str();
}
