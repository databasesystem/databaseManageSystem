#include "fileManage.h"
#include "bufferManage.h"
#include "recordManage.h"

int main()
{
	// test buffer
	FileBuffer fb;
	DBStorage testdb("studentManage", 0, 1);
	tableAttr tableAttribute(3);

	tableAttribute.coltype[0] = 0;
	tableAttribute.coltype[1] = 5;
	tableAttribute.coltype[2] = 5;

	tableAttribute.collen[0] = 4;  //int(10) 10 is the max show length
	tableAttribute.collen[1] = 100;
	tableAttribute.collen[2] = 3;

	tableAttribute.colname[0] = "id";
	tableAttribute.colname[1] = "name";
	tableAttribute.colname[2] = "nation";

	tableAttribute.colIsNull[0] = false;
	tableAttribute.colIsNull[1] = false;
	tableAttribute.colIsNull[2] = true;

	tableAttribute.primaryId = 0;
	tableAttribute.pagenum = 1;
	testdb.createTable("studentinfo", "studentManage", tableAttribute);
	
	recordEntry onedata(3, tableAttribute.collen);
	onedata.isdeleted = 0;
	onedata.isNull[0] = 0;
	onedata.isNull[1] = 0;
	onedata.isNull[2] = 0;
	onedata.item[0] = dataUtility::int_to_char(100008);
	onedata.item[1] = "Oxbow Books Limited";
	onedata.item[2] = "PRC";
	int* t = dataUtility::char_to_int(onedata.item[0]);
	cout << *t << endl;
	cout << onedata.item[1] << endl;
	cout << onedata.item[2] << endl;
	cout << "DATA SIZE = " << onedata.length << endl;

	// test data   from insert to delete
	for(int i = 0; i < 800; i++) {
		cout << "first insert index data: " << i <<endl;
		fb.insertData("studentinfo", onedata);
	}
	fb.flush();
	testdb.printFreeList("studentinfo", 1, onedata.length);
	for (int i = 0; i < 30 ; i++) {
		cout << " delete index data: " << i << endl;
		fb.deleteData("studentinfo", 1, onedata.length*i*2, onedata.length);
	}
	fb.flush();
	testdb.printFreeList("studentinfo", 1, onedata.length);
	for (int i = 0; i < 20 ; i++) {
		cout << " delete index data: " << i << endl;
		fb.deleteData("studentinfo", 2, onedata.length*i*3, onedata.length);
	}
	fb.flush();
	testdb.printFreeList("studentinfo", 2, onedata.length);
	for (int i = 0; i < 15 ; i++) {
		cout << " delete index data: " << i << endl;
		fb.deleteData("studentinfo", 3, onedata.length*i*4, onedata.length);
	}
	fb.flush();
	testdb.printFreeList("studentinfo", 3, onedata.length);
	/*for (int i = 0; i < 40; i++) {
		cout << "second insert index data: " << i <<endl;
		testdb.insertData("studentinfo", onedata);
	}*/
	//100008,'Oxbow Books Limited','PRC'
	////test B+ tree index
	//int agedata[10];
	//node * root;
	//root = NULL;
	//BPlusTree test;
	//for(int i = 0; i < 10; i++) 
	//{
	//	root = test.insert_node(root,i, i);
	//	test.print_tree(root);
	//}
	//test.find_and_print(root, 4, true);
	
	system("pause");
	return 0;
}
