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


void dataUtility::string_to_char(char* dst, string ori, int index, int strLen, int dstLen) {
	if (dstLen < strLen)
		return;
	char* data = new char[dstLen];
	memset(data, 0, dstLen);
	memcpy(data, ori.c_str(), ori.size());
	for (int i = 0; i < strLen; i++){
		dst[index+i] = data[i];
	}
	delete data;
}
void dataUtility::bytefillbyte(char* dst, BYTE* src, int index, BYTE length) {
	for (int i = 0; i < length; i++) 
	{
		dst[index+i] = src[i];
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