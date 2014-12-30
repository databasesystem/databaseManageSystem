#include "fileManage.h"
#include "bufferManage.h"
#include "recordManage.h"

int main()
{
	// test buffer
	FileBuffer* fb = new FileBuffer();
	Node *p1 = new Node(new dbPage("Jason"))
		, *p2 = new Node(new dbPage("Alexia"));
	fb -> insert(rowID(0,1), p1);
	fb -> insert(rowID(0,2), p2);
	Node *p3 = fb -> find(rowID(0,1));
	if (p3) cout << p3 -> page -> data << endl;
	p3 = fb->find(rowID(0,2));
	if (p3) cout << p3 -> page -> data;
	fb -> remove();
	if(p3 = fb->find(rowID(0,2)))
		cout <<  " still here" << endl;
	else
		cout << " is gone" << endl;
	p3 = fb -> find(rowID(0,1));
	if (p3) 
		cout << p3->page->data << " still here" << endl;
	else
		cout << "Jason is gone" << endl;


	// test file read or write by page
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
	onedata.item[1] = "Oxbow Books Limited\0";
	onedata.item[2] = "PRC\0";
	int* t = dataUtility::char_to_int(onedata.item[0]);
	cout << *t << endl;
	cout << onedata.item[1] << endl;
	cout << onedata.item[2] << endl;

	for(int i = 0; i < 3; i++) {
		cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ " << i << " ^^^^^^^^^^^^^^^^^" << endl;
		testdb.insertData("studentinfo", onedata);
	}


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
