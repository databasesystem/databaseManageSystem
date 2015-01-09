#include "systemManage.h"

SysManager::SysManager(){
	//new database
	for(int i = 0; i < ID_NUM_SCALE; i++){
		idPool[i] = false;
	}
	m_nSysobjects = 0;
	m_nSyscolumns = 0;
}

SysManager::~SysManager(){
	flush();
	//clear pointers only, since map can be automatically handled
	for(UINT i = 0; i < m_nSysobjects; i++)
		delete m_pSysobjects[i];
	for(UINT i = 0; i < m_nSyscolumns; i++)
		delete m_pSyscolumns[i];
}

void SysManager::loadSysfile(){
	FILE* fileStream;
	fileStream = fopen(path.c_str(),"rb");
	if(fileStream){
		//write everything in sysManager to sysFile
		char sysFile[SIZE_NAME];
		memset(sysFile, 0, SIZE_NAME);
		fread(sysFile, sizeof(BYTE), 1, fileStream);
		BYTE numTable = dataUtility::char_to_data<BYTE>(sysFile);
		for(BYTE numTable = dataUtility::char_to_data<BYTE>(sysFile); numTable; numTable--){
			//table ID
			fread(sysFile, SIZE_ID, 1, fileStream);
			TYPE_ID tableID = dataUtility::char_to_data<TYPE_ID>(sysFile);
			idPool[tableID] = true;
			//table name
			fread(sysFile, SIZE_NAME, 1, fileStream);
			string tableName(sysFile);
			SysObject *table = new SysObject(tableID, tableName);
			m_pSysobjects[m_nSysobjects++] = table;
			sysobjMap[tableName] = tableID;
			//column information
			fread(sysFile, sizeof(USRT), 1, fileStream);
			for(USRT colNum = dataUtility::char_to_data<USRT>(sysFile); colNum > 0; colNum--){
				//column ID
				fread(sysFile, SIZE_ID, 1, fileStream);
				TYPE_ID colID = dataUtility::char_to_data<TYPE_ID>(sysFile);
				idPool[colID] = true;
				table->vecCols.push_back(colID);
				//column name
				fread(sysFile, SIZE_NAME, 1, fileStream);
				string colName(sysFile);
				//column nullable
				fread(sysFile, sizeof(bool), 1, fileStream);
				bool nullable = dataUtility::char_to_data<bool>(sysFile);
				//column type
				fread(sysFile, sizeof(BYTE), 1, fileStream);
				BYTE xtype = dataUtility::char_to_data<BYTE>(sysFile);
				//column length
				fread(sysFile, SIZE_OFFSET, 1, fileStream);
				TYPE_OFFSET length = dataUtility::char_to_data<TYPE_OFFSET>(sysFile);
				//column offset
				fread(sysFile, SIZE_OFFSET, 1, fileStream);
				TYPE_OFFSET index = dataUtility::char_to_data<TYPE_OFFSET>(sysFile);
				SysColumn *column = new SysColumn(tableID, colID, colName, nullable, xtype, length, index);
				m_pSyscolumns[m_nSyscolumns++] = column;
				syscolMap[NameIndexPair(colName,tableID)] = colID;
			}
		}
		fclose(fileStream);
	}
}

bool SysManager::createTable(string name){
	if( m_nSysobjects == TABLE_NUM_SCALE ){
		cout << "Table num has reached maximum. Please clear rooms for table " << name << "." << endl;
		return false;
	}
	if( sysobjMap.find(name) == sysobjMap.end() ){
		sysobjMap[name] = getNewID();
		SysObject* newTable = new SysObject(sysobjMap[name], name);
		m_pSysobjects[m_nSysobjects++] = newTable;
		return true;
	} else {
		cout << "Table " << name << " has already existed" << endl;
		return false;
	}
}

SysObject *SysManager::findTable(string name){
	auto it = sysobjMap.find(name);
	if(it == sysobjMap.end()){
		cout << "Table " << name << " does not exist." << endl;
		return NULL;
	} else {
		for( UINT i = 0; i < m_nSysobjects; i++){
			if( it->second == m_pSysobjects[i]->id ){
				return m_pSysobjects[i];
			}
		}
	}
	cout << "Unknown error in finding table " << name << endl;
	return NULL;
}

string SysManager::findTableName(TYPE_ID tableID){
	for(auto it : sysobjMap){
		if(it.second == tableID){
			return it.first;
		}
	}
	return NULL;
}

bool SysManager::dropTable(string name){
	auto it = sysobjMap.find(name);
	if(it == sysobjMap.end()){
		cout << "Table " << name << " does not exist." << endl;
		return false;
	} 
	//Get particular table
	TYPE_ID tableID = it->second;
	sysobjMap.erase(it);
	idPool[tableID] = false;
	SysObject* table = NULL;
	for( UINT i = 0; i < m_nSysobjects; i++){
		if( tableID == m_pSysobjects[i]->id ){
			table = m_pSysobjects[i];
			m_pSysobjects[i] = m_pSysobjects[--m_nSysobjects];
			break;
		}
	}
	//Columns deletion
	vector<TYPE_ID> columns;
	for(map<NameIndexPair,TYPE_ID>::iterator it = syscolMap.begin(); it != syscolMap.end(); ){
		if( it->first.second == tableID ){
			columns.push_back(it->second);
			idPool[it->second] = false;
			auto temp = it++;
			syscolMap.erase(temp);
		} else {
			it++;
		}
	}
	for(auto col : columns){
		for( UINT i = 0; i < m_nSyscolumns; i++){
			if( tableID == m_pSyscolumns[i]->colid ){
				delete m_pSyscolumns[i];
				m_pSyscolumns[i] = m_pSyscolumns[--m_nSyscolumns];
				break;
			}
		}
	}
	//Table deletion
	delete table;
	return true;
}

bool SysManager::insertColumn(string name,string tableName,BYTE xtype,TYPE_OFFSET length,bool nullable,TYPE_OFFSET index){
	if( m_nSyscolumns == COLUMN_NUM_SCALE ){
		cout << "There are already " << COLUMN_NUM_SCALE << " columns in database. Please delete some before creating new columns." << endl;
		return false;
	}
	SysObject* table = findTable(tableName);
	auto it = syscolMap.find(NameIndexPair(table->name,table->id));
	if(it == syscolMap.end()){
		UINT colID = getNewID();
		syscolMap[NameIndexPair(name,table->id)] = colID;
		table->vecCols.push_back(colID);
		SysColumn* newCol = new SysColumn(table->id, colID, name, nullable, xtype, length, index);
		m_pSyscolumns[m_nSyscolumns++] = newCol;
		return true;
	} else {
		cout << "Column " << name << " has already existed in Table " << tableName << "." << endl;
		return false;
	}
}

SysColumn *SysManager::findColumn(string name, string tableName){
	SysObject* table = findTable(tableName);
	auto it = syscolMap.find(NameIndexPair(name,table->id));
	if(it == syscolMap.end()){
		cout << "Column " << name << " does not exist in Table " << tableName << "." << endl;
		return NULL;
	} else {
		for( UINT i = 0; i < m_nSyscolumns; i++ ){
			if( it->second == m_pSyscolumns[i]->colid ){
				return m_pSyscolumns[i];
			}
		}
	}
	cout << "Unknown error in finding column " << name << " in table " << tableName << endl;
	return NULL;
}

SysColumn *SysManager::findColumn(TYPE_ID colID){
	for(UINT i = 0; i < m_nSyscolumns; i++){
		if(m_pSyscolumns[i]->colid == colID){
			return  m_pSyscolumns[i];
		}
	}
	cout << "Column with ID " << colID << " does not exist." << endl;
	return NULL;
}

TYPE_OFFSET SysManager::getRecordLength(string tableName){
	TYPE_OFFSET index = 0;
	SysObject* table = findTable(tableName);
	for( UINT i = 0; i < table->vecCols.size(); i++ ){
		SysColumn* column = findColumn(table->vecCols[i]);
		if(index < column->index + column->length){
			index = column->index + column->length;
		}
	}
	index += SIZE_OFFSET;
	return index;
}

void SysManager::setName(string dbname){
	path = "./" + dbname + "/_sysFile";
}

void SysManager::print(){
	if(!sysobjMap.size()){
		cout << "Empty database." << endl;
		return;
	}
	cout << "There are " << sysobjMap.size() << " tables in database." << endl;
	int i = 1;
	for(auto it : sysobjMap){
		cout << "********************* table " << i++ << " *********************" << endl;
		printTable(it.first);
	}
}
vector<SysColumn*> SysManager::getTableAttr(string tableName) {
	vector<SysColumn*> radix;
	SysObject* table = findTable(tableName);
	if(table == NULL)
		return radix;
	for(auto col : table->vecCols){
		radix.push_back(findColumn(col));
	}
	return radix;
}
void SysManager::printTable(string tableName){
	SysObject* table = findTable(tableName);
	if(table == NULL)
		return;
	vector<SysColumn*> radix;
	for(auto col : table->vecCols){
		radix.push_back(findColumn(col));
	}
	cout << "=================== table: " << tableName << " ====================" << endl;
	int i = 1;
	for(auto col : radix){
		string space = "\t";
		for( UINT j = 3; j > col->name.size()/4; j-- ){
			space = space + "\t";
		}
		if(col->nullable)
			cout << i++ << " column: " <<  col->name << space << "|Nullable: T|\tType: " << col->xtype << "|\tLength " << col->length << endl;
		else
			cout << i++ << " column: " <<  col->name << space << "|Nullable: F|\tType: " << //parser::getKeyWords(col->xtype) << 
			"|\tLength " << col->length << endl;
	}
	cout << "============================================================" << endl;
}

void SysManager::printTables(string dbName){
	cout << "++++++++++++++++++++ Database: " << dbName << " +++++++++++++++++++++" << endl;
	int i = 1;
	for(auto it : sysobjMap){
		cout << i++ << " table: " << it.first << endl;
	}
}

void SysManager::flush(){
	if(m_nSysobjects == 0){
		return;
	}
	FILE* fileStream;
	fileStream = fopen(path.c_str(),"rb+");
	if (fileStream){	//if file exists, create backup of sysFile
		fclose(fileStream);
		remove((path+".tmp").c_str());
		rename(path.c_str(), (path+".tmp").c_str());
	}
	fileStream = fopen(path.c_str(), "wb+");
	//write everything in sysManager to sysFile
	char sysFile[SYSFILE_MAX_SIZE];
	memset(sysFile, 0, SYSFILE_MAX_SIZE);
	int sysSize = 0;
	//Num of tables
	dataUtility::data_fill_char<UINT>(sysFile, m_nSysobjects, sysSize, sizeof(BYTE));
	sysSize += sizeof(BYTE);
	//write tableInfo, follow by each column
	for(auto it : sysobjMap){
		SysObject* table = findTable(it.first);
		//table ID
		dataUtility::data_fill_char<TYPE_ID>(sysFile, table->id, sysSize, SIZE_ID);
		sysSize += SIZE_ID;
		//table name
		dataUtility::string_fill_char(sysFile, table->name, sysSize, SIZE_NAME);
		sysSize += SIZE_NAME;
		//column number
		dataUtility::data_fill_char<USRT>(sysFile, table->vecCols.size(), sysSize, sizeof(USRT));
		sysSize += sizeof(USRT);
		for(auto col : table->vecCols){
			SysColumn *column = findColumn(col);
			//column ID
			dataUtility::data_fill_char<TYPE_ID>(sysFile, column->colid, sysSize, SIZE_ID);
			sysSize += SIZE_ID;
			//column name
			dataUtility::string_fill_char(sysFile, column->name, sysSize, SIZE_NAME);
			sysSize += 30;
			//column nullable
			dataUtility::data_fill_char<bool>(sysFile, column->nullable, sysSize, sizeof(bool));
			sysSize += sizeof(bool);
			//column type
			dataUtility::data_fill_char<BYTE>(sysFile, column->xtype, sysSize, sizeof(BYTE));
			sysSize += sizeof(BYTE);
			//column length
			dataUtility::data_fill_char<TYPE_OFFSET>(sysFile, column->length, sysSize, SIZE_OFFSET);
			sysSize += SIZE_OFFSET;
			//column offset
			dataUtility::data_fill_char<TYPE_OFFSET>(sysFile, column->index, sysSize, SIZE_OFFSET);
			sysSize += SIZE_OFFSET;
		}
	}
	fwrite(sysFile, sysSize, sizeof(BYTE), fileStream);
	fclose(fileStream);
	drop();
}

void SysManager::drop(){
	sysobjMap.clear();
	syscolMap.clear();
	for(UINT i = 0; i < m_nSysobjects; i++)
		delete m_pSysobjects[i];
	for(UINT i = 0; i < m_nSyscolumns; i++)
		delete m_pSyscolumns[i];
	m_nSysobjects = m_nSyscolumns = 0;
	for(int i = 0; i < ID_NUM_SCALE; i++){
		idPool[i] = false;
	}
}

vector<string> SysManager::getAllTable(){
	vector<string> tables;
	for(auto it : sysobjMap){
		tables.push_back(it.first);
	}
	return tables;
}

TYPE_ID SysManager::getNewID(){
	for(int i = 0; i < ID_NUM_SCALE; i++){
		if( idPool[i] == false ){
			idPool[i] = true;
			return i;
		}
	}
	cout << "**************idPool is full**************" << endl;
	return ID_NUM_SCALE;
}