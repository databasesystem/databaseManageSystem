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

int getChar();//解析输入的字符串，得到关键字，列名和表名
int parser();//解析SQL语句的主方法，需要调用下列方法
int getColumnName(cName * m_cName);//返回列名
int getTableName(table * m_table);//返回表名
int connectTable(table * m_table, table * s_table);//实现两表连接
//int dealCommand( table * des_table, int dealType);
//解析where语句，包括操作类型（operand），查询条件(op和len)，关系运算符(flag)
int getOperand(cName *operand, unsigned char *op,unsigned char *len,unsigned char *flag);
//执行update操作时得到所操作的表的列名(setColumn)和要更新的值(len)
int getSetColumn(cName *setColumn, unsigned char *len);
//执行create table命令时创建一个列，参数依次为：列名，列的值的类型，该类型的长度（varchar类型需要指出），能否为空
int getColumn(cName *column,unsigned char *type, unsigned int *len,bool *nullable);
//执行insert命令时插入一条记录
int getValues(RecordEntry *values);
//对数据库文件进行操作，执行解析出来的查询语句
int getData(char *fileName, DBStorage &testDB);

int dealCommand( table * srcTable,	//进行操作的原始表格 
				 table * desTable, //得到的结果
				int dealType,			//操作的类型，可以是select,update等
				cName *operand=NULL,// where操作的左右操作数，可以不用此操作数
				int *op = NULL,        //where操作的操作符号，可以不用
				cName *setColumn=NULL //set操作的左右操作数
				);
//用于处理select语句中的两表连接
int dealSelect( table * fromTable, table *fromTable2,
			   cName *operand,
			   unsigned char *op,
			   unsigned char *len, DBStorage &testDB);
#endif