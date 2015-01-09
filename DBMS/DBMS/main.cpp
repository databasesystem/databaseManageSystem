#include "dbManage.h"
#include "wq_parser.h"

int main()
{
	// test buffer
	DBManager testdb("studentManage");
	string colName[] = {"University","Departmant","Year","Name","Student ID"};
	BYTE type[] = {VARCHAR_TYPE,VARCHAR_TYPE,INT_TYPE,VARCHAR_TYPE,INT_TYPE};
	TYPE_OFFSET length[] = {20,20,4,20,4};
	bool nullable[] = {false,false,false,false,false};

	string file = "student";
	testdb.createTable(file, 5, colName, type, length, nullable);
	testdb.print();
	RecordEntry record;
	BYTE item1[] = "HKUST";
	BYTE item2[] = "CSE";
	BYTE* item3 = (BYTE*)dataUtility::data_to_char<int>(3);
	BYTE item4[] = "Wong Kam Kwai";
	BYTE* item5 = (BYTE*)dataUtility::data_to_char<int>(2014110099);
	BYTE rlength[] = {5,3,4,13,4};
	record.length = rlength;
	BYTE *ritem[] = {item1, item2, item3, item4, item5};
	record.item = ritem;

	for(int i = 0 ; i < 1000 ; i++){
		testdb.insertRecord(&record, colName,"student");
	}

	parser testParser("test");
	testParser.testParse();

	system("pause");
	return 0;
}
