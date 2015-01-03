#include "data_utility.h"

int* dataUtility::char_to_int(char* data) {
	int* ori = (int*) malloc(sizeof(int));
	memcpy(ori, data, sizeof(int));
	return ori;
}

double* dataUtility::char_to_double(char* data) {
	double* ori = (double*) malloc(sizeof(double));
	memcpy(ori, data, sizeof(double));
	return ori;
}

float* dataUtility::char_to_float(char* data) {
	float* ori = (float*) malloc(sizeof(float));
	memcpy(ori, data, sizeof(float));
	return ori;
}

bool dataUtility::char_to_bool(char c) {
	if (c=='1')
		return true;
	else
		return false;
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

void dataUtility::printChars(char* data) {
	cout << "char* data: ";
	for (int i = 0; i < strlen(data); i++)
	{
		cout << &(data[i]);
	}
	cout << endl;
}

char dataUtility::bool_to_byte(bool data) {
	char res;
	if (data)
		res='1';
	else
		res='0';
	return res;
}

char* dataUtility::int_to_char(int data) {
	char* res = (char*)malloc(sizeof(int));
	for (int i = 0; i <sizeof(int); i++) 
		res[3-i] = (char)((data>>(3-i)*8)&0xff);
	res[sizeof(int)] ='\0';
	return res;
}


int dataUtility::char2int(char* data) {
	return *((int*) data);
}

//int main() {
//	int a = 312786452;
//	float b = 651.25;
//	double c = 561321.526214;
//	//integer
//	char* data = data_to_char(a);
//	for (unsigned int i = 0; i < sizeof(int); i++) {
//		printf("%c", data[i]);
//	}
//	int* ori_a = char_to_int(data);
//	printf("\n%d", *ori_a);
//	free(data);
//	free(ori_a);
//	printf("\n");
//
//	//float
//	data = data_to_char(b);
//	for (unsigned int i = 0; i < sizeof(float); i++) {
//		printf("%c", data[i]);
//	}
//	float* ori_b = char_to_float(data);
//	printf("\n%f", *ori_b);
//	free(data);
//	free(ori_b);
//	printf("\n");
//
//	//double
//	data = data_to_char(c);
//	for (unsigned int i = 0; i < sizeof(double); i++) {
//		printf("%c", data[i]);
//	}
//	double* ori_c = char_to_double(data);
//	printf("\n%f", *ori_c);
//	free(data);
//	free(ori_c);
//	return 0;
//}
