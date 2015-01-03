#include "fileManage.h"
#include "bufferManage.h"
#include "recordManage.h"

int main()
{
	// test buffer
	FileBuffer fb;
	DBStorage testdb("studentManage", 0, 1);
	attr tableAttr(3);

	tableAttr.coltype[0] = 0;
	tableAttr.coltype[1] = 5;
	tableAttr.coltype[2] = 5;

	tableAttr.collen[0] = 4;  //int(10) 10 is the max show length
	tableAttr.collen[1] = 100;
	tableAttr.collen[2] = 3;

	tableAttr.colname[0] = "id";
	tableAttr.colname[1] = "name";
	tableAttr.colname[2] = "nation";

	tableAttr.colIsNull[0] = false;
	tableAttr.colIsNull[1] = false;
	tableAttr.colIsNull[2] = true;

	tableAttr.primaryId = 0;
	tableAttr.pagenum = 1;
	testdb.createTable("studentinfo", "studentManage", tableAttr);
	
	recordEntry onedata(3, tableAttr.collen);
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

	// test data   from insert to delete
	for(int i = 0; i < 50; i++) {
		cout << "first insert index data: " << i <<endl;
		fb.insertData("studentinfo", onedata);
	}
	fb.refresh();
	testdb.printFreeList("studentinfo", 1, 115);
	for (int i = 0; i < 20; i++) {
		cout << " delete index data: " << i << endl;
		testdb.deleteData("studentinfo", 1, 115*i*2, 115);
	}
	fb.refresh();
	testdb.printFreeList("studentinfo", 1, 115);
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
