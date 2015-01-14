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
	if(table == NULL)
		return false;
	for( UINT i = 0; i < table->vecCols.size(); i++ ){
		SysColumn* column = sysManager.findColumn(colName[i], tableName);
		string temp(dataUtility::getbyte((char*)(input->item[i]),0,input->length[i]));
		if(dataUtility::toUpper(temp).compare("NULL") !=0 ){//!isNull
			memset(record+column->index-1, 0, 1);
			if( column->xtype == STRING_TYPE ){	//string
				memset(record+column->index, 0, column->length);
			}
			dataUtility::bytefillbyte(record, input->item[i], column->index, input->length[i]);
			record[column->index+input->length[i]]='\0';
		} else {
			memset(record+column->index-1, 1, 1);
			record[column->index]='\0';
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
	//int=123: string:'123';
	SysObject* table = sysManager.findTable(tableName);
	if (table == NULL)
		return true;
	vector<SysColumn*> sysColumns = sysManager.getTableAttr(tableName);
	string* colNames = new string[sysColumns.size()];
	for (int i = 0; i < sysColumns.size(); i++)
		colNames[i] = sysColumns[i]->name;
	TYPE_ID pageid = 0; 
	Node* dataPage = readPage(table->id, pageid); 
	TYPE_OFFSET recordLength = sysManager.getRecordLength(tableName);
	bool updateFlag = true;
	SysColumn* col;
	while (dataPage != NULL && pageid < TABLE_MAX_FILE_SIZE) {
		//cout << "pageid: " << pageid << endl;
		int t = PAGE_SIZE;
		for (TYPE_OFFSET offset = 0 ; offset < (t/recordLength); offset++){
			//cout << "offset " << offset << endl;
			if (!checkRecordAvaliable(tableName, offset, pageid))
				continue;
			updateFlag = true;
			for (int i = 0; i < condCnt; i++) {
				if (op[i] == SET)
					continue;
				if (!checkTableColumn(tableName,colName[i])) {
					updateFlag = false;
					break;
				}
				col = getTableColumn(tableName, colName[i]);
				if(col->xtype == INT_TYPE){
					int data = dataUtility::char_to_data<int>(dataPage->page->data+offset*recordLength+col->index);
					int comdata = atoi((char*)Value[i]);
					if (!dataUtility::intOptint(data, op[i], comdata)){
						updateFlag = false;
						break;
					}
				}
				else if(col->xtype == VARCHAR_TYPE){
					string data(dataUtility::getbyte(dataPage->page->data,offset*recordLength+col->index,col->length));
					string comData(dataUtility::getbyte((char*)Value[i], 0, (int)len[i]));
					if (!dataUtility::stringOptstring(data, op[i], comData)){
						updateFlag = false;
						break;
					}
				}
			}
			if (updateFlag == true) {
				//cout << "update onedata pageid: "  << pageid << "offset: " << offset << endl;
				for (int i = 0; i < condCnt; i++) {
					if (op[i] != SET)
						continue;
					col = getTableColumn(tableName, colName[i]);
					if (col->xtype == INT_TYPE) {
						len[i] = 4;
						char* temp = new char[4];
						temp = dataUtility::data_to_char<int>(atoi((char*)Value[i]));
						dataUtility::bytefillbyte(dataPage->page->data, (unsigned char*)temp, offset*recordLength+col->index, len[i]);
						delete[] temp;
					}else {
						//char* temp = new char[col->length-len[i]];
						dataUtility::bytefillbyte(dataPage->page->data, (unsigned char*)Value[i], offset*recordLength+col->index, len[i]);
						dataPage->page->data[offset*recordLength+col->index+len[i]] = '\0';
						//dataUtility::bytefillbyte(dataPage->page->data, (unsigned char*)temp, offset*recordLength+col->index+len[i], col->length-len[i]);
						//delete[] temp;
					}
					dataPage->dirty = true;
				}
				//printRecord(tableName,sysColumns.size(),colNames, offset,pageid);
			}
		}
		pageid++;
		dataPage = readPage(table->id, pageid);
	}
	return true;
}

void DBManager::combine(const vector<RecordEntry*> &A, const vector<RecordEntry*> &B, string tableAName, string tableBName, 
		vector<tableJoinRequire> joinReq, string *showAColName, int showANum, string *showBColName, int showBNum) {
			cout << "***************select join data show**********************" << endl;
			int count = 0;
			for(int i = 0; i < A.size(); i++) {
				for (int j = 0; j < B.size(); j++) {
					if (checkJoinOk(A[i], B[j], tableAName, tableBName, joinReq))
					{
						count++;
						printJoinRes(A[i], B[j], tableAName, tableBName, showAColName, showANum, showBColName,showBNum);
					}
				}
			}
			cout << "***************select join num is " << count << "**********************" << endl;
}
bool DBManager::checkJoinOk(RecordEntry* A, RecordEntry* B, string tableAName, string tableBName, vector<tableJoinRequire> joinReq){
	SysColumn* Acol = new SysColumn[1];
	SysColumn* Bcol = new SysColumn[1];
	int AcolIndex = -1;
	int BcolIndex = -1;
	for (int i = 0; i < joinReq.size(); i++) {
		AcolIndex = -1;
		BcolIndex = -1;
		if (checkTableColumn(tableAName, joinReq[i].table1ColumnName))
			Acol = getTableColumn(tableAName, joinReq[i].table1ColumnName);
		if (checkTableColumn(tableBName, joinReq[i].table2ColumnName))
			Bcol = getTableColumn(tableBName, joinReq[i].table2ColumnName);
		if (Acol->xtype != Bcol->xtype) //if type is not same, join wrong
			return false;
		AcolIndex = getColumeIndex(tableAName, joinReq[i].table1ColumnName);
		BcolIndex = getColumeIndex(tableBName, joinReq[i].table2ColumnName);
		if (AcolIndex == -1 || BcolIndex == -1)
			return false;
		if (Acol->xtype == INT_TYPE) {
			int Adata = dataUtility::char_to_data<int>((char*)(A->item[AcolIndex]));
			int Bdata = dataUtility::char_to_data<int>((char*)(B->item[BcolIndex]));
			if (!dataUtility::intOptint(Adata, joinReq[i].op, Bdata))
				return false;
		} else if (Acol->xtype == VARCHAR_TYPE) {
			string Adata(dataUtility::getbyte((char*)(A->item[AcolIndex]), 0, A->length[AcolIndex]));
			string Bdata(dataUtility::getbyte((char*)(B->item[BcolIndex]), 0, B->length[BcolIndex]));
			if (!dataUtility::stringOptstring(Adata, joinReq[i].op, Bdata))
				return false;
		}
	}
	return true;
}
int DBManager::getColumeIndex(string tableName, string columnName) {
	vector<SysColumn*> syscolumns = getTableAttr(tableName);
	for (int i = 0; i < syscolumns.size(); i++) {
		if (syscolumns[i]->name.compare(columnName)==0)
			return i;
	}
	return -1;
}
void DBManager::printJoinRes(RecordEntry* A, RecordEntry* B, string tableAName, string tableBName,
							 string *showAColName, int showANum, string *showBColName, int showBNum){
								 int colIndex = -1;
								 
								 for (int i = 0; i < showANum; i++) {
									 colIndex = getColumeIndex(tableAName, showAColName[i]);
									 if (colIndex != -1) {
										 cout << i << " " << tableAName.c_str() << "." << showAColName[i] << " :";
										 if (getTableColumn(tableAName, showAColName[i])->xtype == VARCHAR_TYPE) {
											 string data(dataUtility::getbyte((char*)(A->item[colIndex]), 0, A->length[colIndex]));
											 cout << "\"" << data.c_str() << "\"" << endl;
										 } else if (getTableColumn(tableAName, showAColName[i])->xtype == INT_TYPE) {
											 int data = dataUtility::char_to_data<int>((char*)(A->item[colIndex]));
											 cout << data << endl;
										 }
									 }
								 }
								 for (int i = 0; i < showBNum; i++) {
									 colIndex = getColumeIndex(tableBName, showBColName[i]);
									 if (colIndex != -1) {
										 cout << showANum+i << " " << tableBName.c_str() << "." << showBColName[i] << " :";
										 if (getTableColumn(tableBName, showBColName[i])->xtype == VARCHAR_TYPE) {
											 string data(dataUtility::getbyte((char*)(B->item[colIndex]), 0, B->length[colIndex]));
											 cout << "\"" << data.c_str() << "\"" << endl;
										 } else if (getTableColumn(tableBName, showBColName[i])->xtype == INT_TYPE) {
											 int data = dataUtility::char_to_data<int>((char*)(B->item[colIndex]));
											 cout << data << endl;
										 }
									 }
								 }
}
vector<RecordEntry*> DBManager::getFindRecord(string tableName,BYTE **Value,string *colName, BYTE *type, BYTE *len,BYTE *op, BYTE condCnt){
	vector<RecordEntry*> res;
	SysObject* table = sysManager.findTable(tableName);
	if (table == NULL)
		return res;
	TYPE_ID pageid = 0; 
	Node* dataPage = readPage(table->id, pageid); 
	TYPE_OFFSET recordLength = sysManager.getRecordLength(tableName);
	bool printFlag = true;
	SysColumn* col;
	while (dataPage != NULL && pageid < TABLE_MAX_FILE_SIZE) {
		int t = PAGE_SIZE;
		for (TYPE_OFFSET offset = 0 ; offset < (t/recordLength); offset++){
			printFlag = true;
			for (int i = 0; i < condCnt; i++) {
				if (!checkTableColumn(tableName,colName[i])) {
					printFlag = false;
					break;
				}
				col = getTableColumn(tableName, colName[i]);
				if(col->xtype == INT_TYPE){
					int data = dataUtility::char_to_data<int>(dataPage->page->data+offset*recordLength+col->index);
					int comdata = atoi((char*)Value[i]);
					if (!dataUtility::intOptint(data, op[i], comdata)){
						printFlag = false;
						break;
					}
				}
				else if(col->xtype == VARCHAR_TYPE){
					string data(dataUtility::getbyte(dataPage->page->data,offset*recordLength+col->index,col->length));
					string comData(dataUtility::getbyte((char*)Value[i], 0, (int)len[i]));
					if (!dataUtility::stringOptstring(data, op[i], comData)){
						printFlag = false;
						break;
					}
				}
			}
			if (printFlag == true) {
				//cout << "select onedata pageid: "  << pageid << "offset: " << offset << endl;
				if (checkRecordAvaliable(tableName, offset, pageid))
					res.push_back(getRecord(tableName, offset, pageid));
			}
		}
		pageid++;
		dataPage = readPage(table->id, pageid);
	}
	return res;
};
vector<RecordEntry*> DBManager::findRecord(string tableName,BYTE **Value,string *colName, BYTE *type, BYTE *len,BYTE *op, BYTE condCnt, string *showColName, int showNum){
	cout << "***************select data show**********************" << endl;
	int count = 0;
	vector<RecordEntry*> res;
	SysObject* table = sysManager.findTable(tableName);
	if (table == NULL)
		return res;
	vector<SysColumn*> sysColumns = sysManager.getTableAttr(tableName);
	string* colNames = new string[sysColumns.size()];
	for (int i = 0; i < sysColumns.size(); i++)
		colNames[i] = sysColumns[i]->name;
	TYPE_ID pageid = 0; 
	Node* dataPage = readPage(table->id, pageid); 
	TYPE_OFFSET recordLength = sysManager.getRecordLength(tableName);
	bool printFlag = true;
	SysColumn* col;
	while (dataPage != NULL && pageid < TABLE_MAX_FILE_SIZE) {
		//cout << "pageid: " << pageid << endl;
		int t = PAGE_SIZE;
		for (TYPE_OFFSET offset = 0 ; offset < (t/recordLength); offset++){
			//cout << "offset " << offset << endl;
			if (!checkRecordAvaliable(tableName, offset, pageid))
				continue;
			printFlag = true;
			for (int i = 0; i < condCnt; i++) {
				if (!checkTableColumn(tableName,colName[i])) {
					printFlag = false;
					break;
				}
				col = getTableColumn(tableName, colName[i]);
				if(col->xtype == INT_TYPE){
					int data = dataUtility::char_to_data<int>(dataPage->page->data+offset*recordLength+col->index);
					int comdata = atoi((char*)Value[i]);
					if (!dataUtility::intOptint(data, op[i], comdata)){
						printFlag = false;
						break;
					}
				}
				else if(col->xtype == VARCHAR_TYPE){
					string data(dataUtility::getbyte(dataPage->page->data,offset*recordLength+col->index,col->length));
					string comData(dataUtility::getbyte((char*)Value[i], 0, (int)len[i]));
					if (!dataUtility::stringOptstring(data, op[i], comData)){
						printFlag = false;
						break;
					}
				}
			}
			if (printFlag == true) {
				//cout << "select onedata pageid: "  << pageid << "offset: " << offset << endl;
				count++;
				printRecord(tableName, showNum , showColName, offset,pageid);
			}
		}
		pageid++;
		dataPage = readPage(table->id, pageid);
	}
	cout << "***************select num is  "<< count << "*******************" << endl;
	return res;
}
bool DBManager::deleteRecord(string tableName,BYTE **Value,string *colName,BYTE *type,BYTE *len,BYTE *op, BYTE condCnt){
	//int=123: string:'123';
	SysObject* table = sysManager.findTable(tableName);
	if (table == NULL)
		return true;
	vector<SysColumn*> sysColumns = sysManager.getTableAttr(tableName);
	string* colNames = new string[sysColumns.size()];
	for (int i = 0; i < sysColumns.size(); i++)
		colNames[i] = sysColumns[i]->name;
	TYPE_ID pageid = 0; 
	Node* dataPage = readPage(table->id, pageid); 
	TYPE_OFFSET recordLength = sysManager.getRecordLength(tableName);
	bool deleteFlag = true;
	SysColumn* col;
	while (dataPage != NULL && pageid < TABLE_MAX_FILE_SIZE) {
		//cout << "pageid: " << pageid << endl;
		int t = PAGE_SIZE;
		for (TYPE_OFFSET offset = 0 ; offset < (t/recordLength); offset++){
			//cout << "offset " << offset << endl;
			if (!checkRecordAvaliable(tableName, offset, pageid))
				continue;
			deleteFlag = true;
			for (int i = 0; i < condCnt; i++) {
				if (!checkTableColumn(tableName,colName[i])) {
					deleteFlag = false;
					break;
				}
				col = getTableColumn(tableName, colName[i]);
				if(col->xtype == INT_TYPE){
					int data = dataUtility::char_to_data<int>(dataPage->page->data+offset*recordLength+col->index);
					int comdata = atoi((char*)Value[i]);
					if (!dataUtility::intOptint(data, op[i], comdata)){
						deleteFlag = false;
						break;
					}
				}
				else if(col->xtype == VARCHAR_TYPE){
					string data(dataUtility::getbyte(dataPage->page->data,offset*recordLength+col->index,col->length));
					string comData(dataUtility::getbyte((char*)Value[i], 0, (int)len[i]));
					if (!dataUtility::stringOptstring(data, op[i], comData)){
						deleteFlag = false;
						break;
					}
				}
			}
			if (deleteFlag == true) {
				//cout << "delete onedata pageid: "  << pageid << "offset: " << offset << endl;
				deleteData(table, pageid, offset*recordLength, recordLength);
				printRecord(tableName,sysColumns.size(),colNames, offset,pageid);
				//cout << "continue" << endl;
			}
		}
		pageid++;
		dataPage = readPage(table->id, pageid);
	}
	return true;
}

string *DBManager::getColName(string tableName, USRT &colNum){
	SysObject* table = sysManager.findTable(tableName);
	SysColumn* column = sysManager.findColumn(table->vecCols[colNum]);
	return &(column->name);
}


RecordEntry* DBManager::getRecord(string tableName, TYPE_OFFSET offset, TYPE_ID pageid) {
	RecordEntry* res = new RecordEntry[1];
	vector<SysColumn*> syscolumns;
	SysObject* table = sysManager.findTable(tableName);
	if(table == NULL)
		return res;
	TYPE_OFFSET recordLength = sysManager.getRecordLength(tableName);
	if( offset >= PAGE_SIZE / recordLength){
		return res;
	}
	Node* dataPage = readPage(table->id, pageid);

	if (checkRecordAvaliable(tableName, offset, pageid)) {
		syscolumns = sysManager.getTableAttr(tableName);
		res->length = new BYTE[syscolumns.size()];
		res->item = new BYTE*[syscolumns.size()];
		for (int i = 0; i < syscolumns.size(); i++) {
			//int data = dataUtility::char_to_data<int>(dataPage->page->data+offset*recordLength+col->index);
			res->length[i]=syscolumns[i]->length;
			res->item[i] = new BYTE[syscolumns[i]->length];
			dataUtility::bytefillbyte((char*)(res->item[i]), (BYTE*)(dataPage->page->data+offset*recordLength+syscolumns[i]->index), 0, res->length[i]);
		}
	}
	return res;
}
bool DBManager::checkRecordAvaliable(string tableName, TYPE_OFFSET offset, TYPE_ID pageid) {
	SysObject* table = sysManager.findTable(tableName);
	if(table == NULL)
		return false;
	TYPE_OFFSET recordLength = sysManager.getRecordLength(tableName);
	if( offset >= PAGE_SIZE / recordLength){
		return false;
	}
	
	Node* dataPage = readPage(table->id, pageid);

	TYPE_OFFSET freeOffset = dataPage->page->header.firstFreeOffset;
	TYPE_OFFSET deletedItem = 0;
	while(freeOffset < offset*recordLength){
		++deletedItem;
		freeOffset = dataUtility::char_to_data<TYPE_OFFSET>(dataPage->page->data+freeOffset+recordLength-SIZE_OFFSET);
	}
	if( freeOffset == EXIST_INDEX && dataPage->page->header.freeCount >= recordLength){
		//Deleted linked list ends before the selected item, check for initialized value
		TYPE_OFFSET realFreeCount = dataPage->page->header.freeCount - deletedItem * recordLength;
		if( PAGE_SIZE - realFreeCount <= offset*recordLength ){ //offset is not valid since it is still in default state
			//cout << "Record " << offset+1 << " in Page " << pageid << " has not been used." << endl;
			return false;
		}
	}
	else if(freeOffset == offset*recordLength){//item is deleted or first free
		if( PAGE_SIZE - dataPage->page->header.freeCount == freeOffset ){ //offset is not valid since it is still in default state
			//cout << "Record " << offset+1 << " in Page " << pageid << " has not been used." << endl;
			return false;
		}
		//cout << "Record " << offset+1 << " in Page " << pageid << " has been deleted." << endl;
		return false;
	}
	return true;
}
void DBManager::printRecord(string tableName,BYTE colnum,string *colName,TYPE_OFFSET offset, TYPE_ID pageid){
	if (checkRecordAvaliable(tableName, offset, pageid) == false)
		return;
	SysObject* table = sysManager.findTable(tableName);
	if(table == NULL)
		return;
	TYPE_OFFSET recordLength = sysManager.getRecordLength(tableName);
	if( offset >= PAGE_SIZE / recordLength){
		return;
	}
	vector<SysColumn*> column;
	for(BYTE i = 0 ; i < colnum; i++){
		column.push_back(sysManager.findColumn(colName[i], tableName));
	}
	Node* dataPage = readPage(table->id, pageid);

	cout << "------- Record " << offset+1 << " in Page " << pageid << " -------" << endl;
	int i = 1;
	for(auto col : column){
		cout << i << " column \"" << col->name << "\" : ";
		if(col->xtype == INT_TYPE){
			int data = dataUtility::char_to_data<int>(dataPage->page->data+offset*recordLength+col->index);
			cout << data << endl;
		}
		else if(col->xtype == VARCHAR_TYPE){
			if ((int)((char)dataUtility::getbyte(dataPage->page->data,offset*recordLength+col->index-1,1)[0]) == 1)
				cout << "NULL" << endl;
			else {
				string data(dataUtility::getbyte(dataPage->page->data,offset*recordLength+col->index,col->length));
				cout << data << endl;
			}
		}
		i++;
	}
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

bool DBManager::checkTableColumn(string tableName, string columnName) {
	vector<SysColumn*> sysColumns = getTableAttr(tableName);
	for (int i = 0; i < sysColumns.size(); i++) {
		if (sysColumns[i]->name == columnName)
			return true;
	}
	return false;
}
SysColumn* DBManager::getTableColumn(string tableName, string columnName) {
	vector<SysColumn*> sysColumns = getTableAttr(tableName);
	if (checkTableColumn(tableName, columnName)) {
		for (int i = 0; i < sysColumns.size(); i++) {
		if (sysColumns[i]->name == columnName)
			return sysColumns[i];
		}
	}
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
	//cout << "read page" << FileID << " " << PageID << endl;
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
				firstOffset = dataUtility::char_to_data<TYPE_OFFSET>(des->data+firstOffset+recordlength-SIZE_OFFSET);
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