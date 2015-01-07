#ifndef _PARSER_H_
#define _PARSER_H_

#include "dbManage.h"
#include <iostream> 
#include <cstdio>
#include <memory>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <cctype>
using namespace std;

#define ENDOFCOMMAND 0
#define ENDOFFILE 1
#define SELECT 2
#define FROM 3
#define WHERE 4
/*
#define OP_AND 5
#define OP_OR 6
#define EQUAL 7
#define LESS 8
#define MORE 9
#define NOTEQUAL 10
*/
#define UPDATE 11
#define SET 12
#define DROP 13
#define TABLE 14
#define DELETE 15
/*
#define LIKE 16
#define IS 17
*/
#define CREATE 18
/*
#define VARCHAR 19
#define NORMALCHAR 20
#define NORMALINT 21
*/
#define ISNULL 22
#define NOTNULL 23
#define NUMBER 24
#define INSERT 25
#define ISTINTO 26
#define VALUES 27
#define SELECT_STAR 28
/*
#define LESSEQUAL 28
#define MOREEQUAL 29
*/

struct cName{
	char *name[20];		//at most 20 columns,and each column's name 30 chars at most
	int num;			//the number of actual columns
};

class table{
private:
	 char name[30];		//the name of the table, at most 30 chars
	 int table_id;		//the id of the table
	 int dbid;			//which db is the table include
	 int columnNum;		//how many columns the table has
	 cName colName;		//the columns of the table
public:
	char* getName()	{	return name;		}
	int getId()		{	return table_id;	}
	int getDbid()	{	return dbid;		}
	int getColumnNum(){	return columnNum;	}
	cName getColumnName() {	return colName;	}
	void setName(char *c) { strcpy(name,c);	}
	void setid(int i){		table_id = i;	}
	void setdbid(int i = -1){dbid = i;		}
	void setColumnNum(int i){	columnNum = i;	}
	void setColumnName(cName name){	colName = name;	}
};

int getChar();//����������ַ������õ��ؼ��֣������ͱ���
int parser();//����SQL��������������Ҫ�������з���
int getColumnName(cName * m_cName);//��������
int getTableName(table * m_table);//���ر���
int connectTable(table * m_table, table * s_table);//ʵ����������
//int dealCommand( table * des_table, int dealType);
//����where��䣬�����������ͣ�operand������ѯ����(op��len)����ϵ�����(flag)
int getOperand(cName *operand, unsigned char *op,unsigned char *len,unsigned char *flag);
//ִ��update����ʱ�õ��������ı������(setColumn)��Ҫ���µ�ֵ(len)
int getSetColumn(cName *setColumn, unsigned char *len);
//ִ��create table����ʱ����һ���У���������Ϊ���������е�ֵ�����ͣ������͵ĳ��ȣ�varchar������Ҫָ�������ܷ�Ϊ��
int getColumn(cName *column,unsigned char *type, unsigned int *len,bool *nullable);
//ִ��insert����ʱ����һ����¼
int getValues(RecordEntry *values);
//�����ݿ��ļ����в�����ִ�н��������Ĳ�ѯ���
int getData(char *fileName, DBStorage &testDB);

int dealCommand( table * srcTable,	//���в�����ԭʼ��� 
				 table * desTable, //�õ��Ľ��
				int dealType,			//���������ͣ�������select,update��
				cName *operand=NULL,// where���������Ҳ����������Բ��ô˲�����
				int *op = NULL,        //where�����Ĳ������ţ����Բ���
				cName *setColumn=NULL //set���������Ҳ�����
				);
//���ڴ���select����е���������
int dealSelect( table * fromTable, table *fromTable2,
			   cName *operand,
			   unsigned char *op,
			   unsigned char *len, DBStorage &testDB);
#endif