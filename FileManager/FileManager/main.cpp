#include <iostream>
#include <fstream>

#include <stdio.h>
#include <direct.h>
#include <stdlib.h>
#include <string.h>

using namespace std;
/*��¼����
ÿһ�����ݿ⽨��һ���ļ��У�������ݿ������ÿһ�����Ӧ����ļ��е��ļ����ļ���һҳ����д������ԡ��ڶ�ҳ֮��ʼд���ݡ�
���ݸ�ʽ��deleted��1byte��+ boolean[0] ��data[0]�Ƿ���null��+ data[0] + boolean[1] + data[1]����
ΪʲôҪ��¼��������ǲ���null����Ϊ�����ַ������ԣ����ַ�����null�ڴ洢��ʱ�򳤶ȶ���0,����Ҫ����
���Ը�ʽ��
class attr{
int num; //�еĸ���
string[] colname; //�е�����
int[] coltype; //�������� 0-int 1-double 2-float 3-char 4-string 5-varchar 6-boolean
int[] collen; //���ݳ���
boolean[] colIsNull; //ÿ�������Ƿ����Ϊ��
}*/
class attr{
private:
	int num; //�еĸ���
	string* colname;//�е�����
	int* coltype;//�������� 0-int 1-double 2-float 3-char 4-string 5-varchar 6-boolean
	int* collen; //���ݳ���
	bool* colIsNull; //ÿ�������Ƿ����Ϊ��
	
	//attr(attr* newAttr){this = newAttr;}
public:
	attr(int _num, string* _colname, int* _coltype, int * _collen, bool* _colIsNull){
		num = _num; colname = _colname; coltype = _coltype; collen = _collen; colIsNull = _colIsNull;
	}
	void init(){}//����

	/*getters and setters*/
	int getNum(){
		return num;
	}
	string* getcolname(){
		return colname;
	}
	int* getcoltype(){
		return coltype;
	}
	int* getcollen(){
		return collen;
	}
	bool* getcolIsNull(){
		return colIsNull;
	}
};

class table{
private:
	int num;//��¼����Ŀ
	attr* head;//��ͷ�������
	char** analyse(char* command){//�������ݿ���������
		
	}
public:
	table(int _num, attr* _head){
		num = _num;
		head = _head;
	}
	void creat(char* command){//��ʼ�������ڽ���д���ļ���
		char** temp = analyse(command);
		char* databaseName = temp[0];
		char* tableName = temp[1];
		int status = creatDB(databaseName);
		if (status != 0){
			cout << "Failed to creat the database." << endl;
			return;
		}
		ifstream inf;
		ofstream ouf;
		inf.open(tableName,ios::out);
		if(inf == 0)
		{
			inf.close();
			ouf.open(tableName);
			if(ouf)
				cout << "Creat table successfully!"<<endl;
		}
		else
			cout << "The file is existed !" << endl;
		ouf.close();
		/*�����Զ����л�ȡ����ֵ��д���ļ�*/
		int _Num = head->getNum();
		string* _colname = head->getcolname();
		int* _coltype = head->getcoltype();
		int* _collen = head->getcollen();
		bool* _colIsNull = head->getcolIsNull();
		ofstream fout(tableName);
		FILE* in = new FILE();
		for(int k = 0; k < _Num; k++){
			fseek(in, 100L, SEEK_CUR);
			fout << _colname[k].c_str();
			fseek(in, 8L, SEEK_CUR);
			fout << _coltype[k];
			fseek(in, 1L, SEEK_CUR);
			fout<< _colIsNull[k];
		}
		fout.close();
	}
	int insert(char* command){
		
	}
	int del(char* command){
		
	}
	int update(char* command){
		
	}
	int getNum(){
		return num;
	}
};

int creatDB(char* DBName){//�������ݿ�
	int status = _mkdir(DBName);
	if (status == 0)
		printf("%s was successfully deleted.\n", DBName);
	else
		printf("%s was fail to ben deleted.\n", DBName);
	return status;
}

int deleteFilefolder(char* DBName) {
	int status = _rmdir(DBName);
	if (status == 0) {
		printf("%s was successfully deleted.\n", DBName);
	} else {
		printf("%s was fail to ben deleted.\n", DBName);;
	}
	return status;
}

int creatTable(){
	//analyse();
	int _num;
	string* _colname;
	int* _coltype;
	int * _collen;
	bool* _colIsNull;

	attr* head = new attr(_num, _colname, _coltype, _collen, _colIsNull);
	table* newTable = new table(0, head);
}

int delTable(){
	
}

int main(){

	return 0;
}
