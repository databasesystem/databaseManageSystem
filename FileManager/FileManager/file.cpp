//�ж��ļ��Ƿ���ڣ�������ڣ��򿪣���������ڣ��½���
//���������������ļ�ʱ�Ƿ�ɹ������з���ֵ��������Ϊ�ļ���ʧ���������ļ���������ɵģ�
//������ڴ��ļ�ʱ������ļ����������½�������ļ�������ɾ��ԭ�ļ����½�һ��
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
	char filename[] = "..."; // �˴�д���ļ��� 
	ofstream fout(filename);
	fout << ....; // fout�÷���coutһ��, ������д���ļ�����ȥ
}