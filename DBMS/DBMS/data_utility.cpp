#include "data_utility.h"

void dataUtility::string_fill_char(char* dst, string ori, int index, int size){
	char* data = new char[size];
	memset(data, 0, size);
	memcpy(data, ori.c_str(), ori.size());
	for (int i = 0; i < size; i++){
		dst[index+i] = data[i];
	}
	delete data;
}

void dataUtility::bytefillbyte(char* src, char* dst, int index) {
	for (unsigned int i = 0; i < strlen(dst); i++)
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