#include "systemManage.h"

SystemManager::SystemManager(){
	//new database
	for(int i = 0; i < ID_NUM_SCALE; i++){
		idPool[i] = 0;
	}
	m_nSysobjects = 0;
	m_nSysindexes = 0;
	m_nSyscolumns = 0;
}

SystemManager::~SystemManager(){
	//save sysFile

	//clear pointer
	for(UINT i = 0; i < m_nSysobjects; i++){
		delete m_pSysobjects[i];
	}
	for(UINT i = 0; i < m_nSyscolumns; i++){
		delete m_pSyscolumns[i];
	}
}

bool SystemManager::createTable(string name){
	if( m_nSysobjects == TABLE_NUM_SCALE ){
		cout << "Table num has reached maximum. Please clear rooms for table " << name << "." << endl;
		return false;
	}
	if(sysobjMap.find(name) == sysobjMap.end()){
		sysobjMap[name] = getNewID();
		SysObject* newTable = new SysObject(sysobjMap[name], name);
		m_pSysobjects[m_nSysobjects++] = newTable;
		return true;
	} else {
		cout << "Table " << name << " has already existed" << endl;
		return false;
	}
}

SysObject* SystemManager::findTable(string name){
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

bool SystemManager::dropTable(string name){
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

bool SystemManager::insertColumn(string name,string tableName,BYTE xtype,TYPE_OFFSET length,bool nullable,TYPE_OFFSET index){
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
		SysColumn* newCol = new SysColumn(table->id, colID, name, xtype, length, nullable, index);
		m_pSyscolumns[m_nSyscolumns++] = newCol;
		return true;
	} else {
		cout << "Column " << name << " has already existed in Table " << tableName << "." << endl;
		return false;
	}
}

SysColumn* SystemManager::findColumn(string name, string tableName){
	SysObject* table = findTable(tableName);
	auto it = syscolMap.find(NameIndexPair(name,table->id));
	if(it == syscolMap.end()){
		cout << "Column " << name << " does not exist in Table " << tableName << "." << endl;
		return NULL;
	} else {
		for( UINT i = 0; i < m_nSyscolumns; i++){
			if( it->second == m_pSyscolumns[i]->colid ){
				return m_pSyscolumns[i];
			}
		}
	}
	cout << "Unknown error in finding column " << name << " in table " << tableName << endl;
	return NULL;
}

void SystemManager::print(){
	if(!sysobjMap.size()){
		cout << "Empty database." << endl;
		return;
	}
	cout << "There are " << sysobjMap.size() << " tables in database." << endl;
	map<TYPE_ID, vector<pair<TYPE_OFFSET,string>> > radix;
	for( UINT i = 0; i < m_nSyscolumns; i++ ){
		radix[m_pSyscolumns[i]->id].push_back(pair<TYPE_OFFSET,string>(m_pSyscolumns[i]->index,m_pSyscolumns[i]->name));
	}
	int i = 1;
	for(auto it : sysobjMap){
		cout << "===============" << i << " table: " << it.first << "===============" << endl;
		sort(radix[it.second].begin(),radix[it.second].end());
		for(UINT j = 0; j < radix[it.second].size(); j++){
			cout << j+1 << " column: " <<  radix[it.second][j].second << endl;
		}
		i++;
	}
}

void SystemManager::flush(){
	//save all data
}

TYPE_ID SystemManager::getNewID(){
	for(int i = 0; i < ID_NUM_SCALE; i++){
		if( idPool[i] == false ){
			idPool[i] = true;
			return i;
		}
	}
	cout << "**************idPool is full**************" << endl;
	return ID_NUM_SCALE;
}