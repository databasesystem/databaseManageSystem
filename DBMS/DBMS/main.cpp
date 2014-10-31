#include "node.h"

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
	system("pause");
	return 0;
}