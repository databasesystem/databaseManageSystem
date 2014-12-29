#ifndef _GLOBALVARIABLE_H
#define _GLOBALVARIABLE_H
typedef unsigned int UINT;
typedef unsigned char BYTE;
typedef unsigned short int USRT;

#define DB_PGSIZE 8192
#define DB_PGSIZE_USRT 8192/sizeof(short)
#define DB_MAX_BUFFER_SIZE 1000
#define DB_MAX_FILE_NUM	((1<<8*(sizeof (short)))-1)
#define DB_DEFAULT_FILE_SIZE 1048576  //(1024*1024)
#define DB_MAX_TABLE_NUM 1024


#define INT_TYPE 0
#define DOUBLE_TYPE 1
#define CHAR_TYPE 2
#endif