#include "dbManage.h"
#include "wq_parser.h"
int main()
{
	// test buffer
	FileBuffer fb;
	DBStorage testdb("studentManage", 0, 1);
	tableAttr tableAttribute(3);

	tableAttribute.colType[0] = 0;
	tableAttribute.colType[1] = 4;
	tableAttribute.colType[2] = 4;

	tableAttribute.colLen[0] = 4;  //int(10) 10 is the max show length
	tableAttribute.colLen[1] = 50;
	tableAttribute.colLen[2] = 3;

	tableAttribute.colName[0] = "id";
	tableAttribute.colName[1] = "name";
	tableAttribute.colName[2] = "nation";

	tableAttribute.colIsNull = (0 << 2 )+ (1 << 1 )+ 1;

	tableAttribute.pageNum = 1;
	testdb.createTable("studentinfo", "studentManage", tableAttribute);
	
	recordEntry onedata(3, tableAttribute.colLen);
	onedata.isNull = 0;
	onedata.item[0] = dataUtility::data_to_char<int>(100008);
	onedata.item[1] = "Oxbow Books Limited";
	onedata.item[2] = "PRC";
	cout << dataUtility::char_to_class_reference<int>(onedata.item[0]) << endl;
	cout << onedata.item[1] << endl;
	cout << onedata.item[2] << endl;
	cout << "DATA SIZE = " << onedata.length << endl;

	// test data   from insert to delete
	for(int i = 0; i < 800; i++) {
		fb.insertData("studentinfo", onedata);
	}
	fb.flush();
	testdb.printFreeList("studentinfo", 1, onedata.length);
	for (int i = 0; i < 30 ; i++) {
		fb.deleteData("studentinfo", 1, onedata.length*i*2, onedata.length);
	}
	fb.flush();
	testdb.printFreeList("studentinfo", 1, onedata.length);
	for (int i = 0; i < 20 ; i++) {
		fb.deleteData("studentinfo", 2, onedata.length*i*3, onedata.length);
	}
	fb.flush();
	testdb.printFreeList("studentinfo", 2, onedata.length);
	for (int i = 0; i < 15 ; i++) {
		fb.deleteData("studentinfo", 3, onedata.length*i*4, onedata.length);
	}
	fb.flush();
	testdb.printFreeList("studentinfo", 3, onedata.length);
	
	parser testParser;
	testParser.testParse();
	system("pause");
	return 0;
}
