#ifndef _GLOBALVARIABLE_H
#define _GLOBALVARIABLE_H

#include <iostream>
#include <cstring>
using namespace std;
typedef unsigned int	UINT;
typedef unsigned short	USRT;
typedef unsigned char	BYTE;

#define TYPE_ID		USRT
#define TYPE_OFFSET USRT
#define TYPE_NAME	char*
#define SIZE_ID		sizeof(USRT)
#define SIZE_OFFSET sizeof(USRT)
#define SIZE_NAME	30

#define DB_PGSIZE				8192 // size of page
#define DB_PGSIZE_USRT			4096 //DB_PGSIZE/(sizeof (USRT))
#define DB_MAX_BUFFER_SIZE		1000 // the size of maximum buffer page
#define TABLE_MAX_FILE_SIZE		40
#define TABLE_NUM_SCALE			100
#define COLUMN_NUM_SCALE		10000
#define ID_NUM_SCALE			10200 //(COLUMN_NUM_SCALE+2*TABLE_NUM_SCALE)


#define PAGE_SIZE	 DB_PGSIZE-sizeof(pageHeader)
#define MEMSET_NUM	 255
#define SYSFILE_MAX_SIZE 383401
#define EXIST_INDEX  65535

#define INT_TYPE	 30
#define DOUBLE_TYPE	 1
#define FLOAT_TYPE	 2
#define CHAR_TYPE	 3
#define STRING_TYPE	 4
#define VARCHAR_TYPE 31
#define BOOLEAN_TYPE 6



#endif