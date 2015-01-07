#ifndef _GLOBALVARIABLE_H
#define _GLOBALVARIABLE_H

typedef unsigned int	UINT;
typedef unsigned short	USRT;
typedef unsigned char	BYTE;

#define TYPE_ID		USRT
#define TYPE_OFFSET USRT
#define SIZE_ID		sizeof(USRT)
#define SIZE_OFFSET sizeof(USRT)

#define DB_PGSIZE				8192 // size of page
#define DB_PGSIZE_USRT			4096 //DB_PGSIZE/(sizeof (USRT))
#define DB_MAX_BUFFER_SIZE		1000 // the size of maximum buffer page
#define DB_MAX_FILE_NUM			255  //((1<<8*(sizeof (USRT)))-1)
#define DB_DEFAULT_FILE_SIZE	1048576 //(1024*1024)
#define DB_MAX_TABLE_NUM		1024
#define TABLE_NUM_SCALE			100
#define COLUMN_NUM_SCALE		10000
#define ID_NUM_SCALE			10200 //(COLUMN_NUM_SCALE+2*TABLE_NUM_SCALE)


#define PAGE_SIZE	 DB_PGSIZE-sizeof(pageHeader)
#define INDEX_SYSCOL 0
#define MEMSET_NUM	 255
#define EXIST_INDEX  65535

#define INT_TYPE	 0
#define DOUBLE_TYPE	 1
#define FLOAT_TYPE	 2
#define CHAR_TYPE	 3
#define STRING_TYPE	 4
#define VARCHAR_TYPE 5
#define BOOLEAN_TYPE 6

#endif