#include "dbManage.h"

int main()
{
	// test buffer
	DBManager testdb("studentManage");
	string colName[] = {"University","Departmant","Year","Name","Student ID"};
	BYTE type[] = {VARCHAR_TYPE,VARCHAR_TYPE,INT_TYPE,VARCHAR_TYPE,INT_TYPE};
	TYPE_OFFSET length[] = {20,20,4,20,4};
	bool nullable[] = {false,false,false,false,false};
	
	testdb.print();
	string file = "student";
	for(int i = 0; i < 100; i++){
		char num[10];
		sprintf(num, "%02i", i);
		testdb.createTable(file+num, 5, colName, type, length, nullable);
		//testdb.dropTable(file+num);
	}
	testdb.print();

	system("pause");
	return 0;
}
