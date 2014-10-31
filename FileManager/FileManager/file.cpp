//判断文件是否存在，如果存在，打开，如果不存在，新建。
//利用了输入流打开文件时是否成功都会有返回值（这里认为文件打开失败是由于文件不存在造成的）
//输出流在打开文件时，如果文件不存在则新建，如果文件存在则删除原文件再新建一个
#include <iostream>
#include <fstream>
using namespace std;
int main()
{
 ifstream inf;
 ofstream ouf;
 inf.open("f:\\text_5.txt",ios::out);
 if(inf==0)
 {
  inf.close();
  cout<<"Failed!"<<endl;
  ouf.open("f:\\text_5.txt");
  if(ouf)
   cout<<"Created successfully!"<<endl;
 }
 else
  cout<<"The file is exited !\n";
 return 0;
}

#include <iostream>
#include <fstream>
using namespace std;

int main()
{
	char filename[] = "..."; // 此处写入文件名 
	ofstream fout(filename);
	fout << ....; // fout用法和cout一致, 不过是写到文件里面去
}