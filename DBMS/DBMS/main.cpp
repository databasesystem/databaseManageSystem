#include "node.h"
#include "fileManage.h"
#include "globalVariable.h"

int main()
{
	FileBuffer* fb = new FileBuffer(12);
	Node *p1 = new Node("aaa", 1), *p2 = new Node("bbb", 2);
	fb -> insert(p1);
	fb -> insert(p2);
	Node *p3 = fb -> find(1);
	if (p3) cout << p3 -> data << endl;
	fb -> remove();
	p3 = fb -> find(1);
	if (!p3) cout << "null" << endl;

	char data[PAGE_SIZE];
	for (int i = 0; i < PAGE_SIZE-1; i++) 
	{
		data[i]='a';
	}
	data[PAGE_SIZE-1]='\0';
	FileManage::writePageToFile(2, data, "hello.txt");
	for (int i = 0; i < PAGE_SIZE-1; i++) 
	{
		data[i]='b';
	}
	data[PAGE_SIZE-1]='\0';
	FileManage::writePageToFile(1, data, "hello.txt");
	for (int i = 0; i < PAGE_SIZE-1; i++) 
	{
		data[i]='c';
	}
	data[PAGE_SIZE-1]='\0';
	FileManage::writePageToFile(0, data, "hello.txt");
	char result[PAGE_SIZE];
	FileManage::readPageFromFile(0,result, "hello.txt");
	cout << result << endl;
	system("pause");
	return 0;
}