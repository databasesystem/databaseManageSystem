#include "dbManage.h"

int main()
{
	// test buffer
	DBStorage testdb("studentManage");
	string colName[] = {"University","Departmant","Year","Name","Student ID"};
	BYTE type[] = {VARCHAR_TYPE,VARCHAR_TYPE,INT_TYPE,VARCHAR_TYPE,INT_TYPE};
	TYPE_OFFSET length[] = {20,20,4,20,4};
	bool nullable[] = {false,false,false,false,false};
	
	testdb.print();
	testdb.createTable("student", 5, colName, type, length, nullable);
	testdb.print();
	testdb.dropTable("student");
	testdb.print();

	system("pause");
	return 0;
}
