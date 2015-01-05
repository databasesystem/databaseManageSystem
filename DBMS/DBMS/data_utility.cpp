#include "data_utility.h"

int dataUtility::char2short(char* data) {
	return *((unsigned short*) data);
}

void dataUtility::bytefillbyte(char* src, char* dst, int index) {
	for (int i = 0; i < strlen(dst); i++)
	{
		src[index+i] = dst[i];
	}
}

void dataUtility::bytefillbyte(char* dst, char* src, int index, int length) {
	char* temp = new char[length];
	memcpy(temp, src, length);
	for (int i = 0; i < length; i++) 
	{
		dst[index+i] = temp[i];
	}
}

char* dataUtility::getbyte(char* src, int start, int length) {
	char* data = (char*) malloc(length+1);
	for (int i = 0; i < length; i++)
	{
		data[i] = src[start+i];
	}
	data[length] = 0;
	return data;
}

char dataUtility::bool_to_byte(bool data) {
	char res;
	if (data)
		res='1';
	else
		res='0';
	return res;
}