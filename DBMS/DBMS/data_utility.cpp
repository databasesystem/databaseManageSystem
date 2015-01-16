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
void dataUtility::bytefillbyte(char* dst, BYTE* src, int index, int length) {
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

bool dataUtility::intOptint(int a, int opt, int b){
	if (opt == EQUAL) { 
		if (a==b)
			return true;
		else
			return false;
	} else if (opt == NOTEQUAL) {
		if (a!=b)
			return true;
		else 
			return false;
	}else if (opt == MORE) {
		if (a>b)
			return true;
		else 
			return false;
	}else if (opt == MOREEQUAL) {
		if (a>=b)
			return true;
		else 
			return false;
	}else if (opt == LESS) {
		if (a<b)
			return true;
		else 
			return false;
	}else if (opt == LESSEQUAL) {
		if (a<=b)
			return true;
		else 
			return false;
	}
	return false;
}
bool dataUtility::stringOptstring(string leftstr, int opt, string rightstr){
	if (opt == EQUAL) { 
		if (leftstr.compare(rightstr)==0)
			return true;
		else
			return false;
	} else if (opt == NOTEQUAL) {
		if (leftstr.compare(rightstr) != 0)
			return true;
		else 
			return false;
	}else if (opt == MORE) {
		if (leftstr.compare(rightstr) > 0)
			return true;
		else 
			return false;
	}else if (opt == MOREEQUAL) {
		if (leftstr.compare(rightstr) >= 0)
			return true;
		else 
			return false;
	}else if (opt == LESS) {
		if (leftstr.compare(rightstr) < 0)
			return true;
		else 
			return false;
	}else if (opt == LESSEQUAL) {
		if (leftstr.compare(rightstr) <= 0)
			return true;
		else 
			return false;
	}
	return false;
}

string dataUtility::toUpper(string s) {
	for (int i = 0; i < s.length(); i++) {
		if (!isEnglishAlphabet(s.at(i)))
			return "";
		if (s.at(i) >= 'a' && s.at(i) <= 'z') {
			s.at(i) = s.at(i) -32;
		} 
	}
	return s;
}

bool dataUtility::isCmp(char k) {
	return (k == '>' || k == '<' || k == '=');
}
bool dataUtility::isEnglishAlphabet(char k) {
	return (('a' <= k && k <= 'z') || ('A' <= k && k <= 'Z'));
}
bool dataUtility::isDig(char k) {
	return '0' <= k && k <= '9';
}



