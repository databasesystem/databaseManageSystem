#include <iostream>
#include <fstream>

#include <stdio.h>
#include <direct.h>
#include <stdlib.h>
#include <string.h>

using namespace std;
/*记录管理
每一个数据库建立一个文件夹，这个数据库里面的每一个表对应这个文件夹的文件。文件第一页用来写表的属性。第二页之后开始写数据。
数据格式：deleted（1byte）+ boolean[0] （data[0]是否是null）+ data[0] + boolean[1] + data[1]……
为什么要记录这个数据是不是null，因为对于字符串而言，空字符串和null在存储的时候长度都是0,所以要区分
属性格式：
class attr{
int num; //列的个数
string[] colname; //列的名字
int[] coltype; //数据类型 0-int 1-double 2-float 3-char 4-string 5-varchar 6-boolean
int[] collen; //数据长度
boolean[] colIsNull; //每列数据是否可以为空
}*/
class attr{
private:
	int num; //列的个数
	string* colname;//列的名字
	int* coltype;//数据类型 0-int 1-double 2-float 3-char 4-string 5-varchar 6-boolean
	int* collen; //数据长度
	bool* colIsNull; //每列数据是否可以为空
	
	//attr(attr* newAttr){this = newAttr;}
public:
	attr(int _num, string* _colname, int* _coltype, int * _collen, bool* _colIsNull){
		num = _num; colname = _colname; coltype = _coltype; collen = _collen; colIsNull = _colIsNull;
	}
	void init(){}//待定

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
	int num;//记录的数目
	attr* head;//表头存放属性
	char** analyse(char* command){//返回数据库名、表名
		
	}
public:
	table(int _num, attr* _head){
		num = _num;
		head = _head;
	}
	void creat(char* command){//初始化，用于将表写入文件夹
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
		/*从属性对象中获取到的值，写入文件*/
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

int creatDB(char* DBName){//创建数据库
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
