#ifndef _GLOBALVARIABLE_H
#define _GLOBALVARIABLE_H

typedef unsigned int UINT;
typedef unsigned short USRT;
typedef unsigned char BYTE;

#define PAGE_SIZE 8192-sizeof(pageHeader)
#define INT_TYPE 0
#define DOUBLE_TYPE 1
#define FLOAT_TYPE 2
#define CHAR_TYPE 3
#define STRING_TYPE 4
#define VARCHAR_TYPE 5
#define BOOLEAN_TYPE 6

#endif