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

char* dataUtility::bytefillbyte(char* src, char* dst, int start) {
	if (start > strlen(src))
		return "";
	char* res = new char[start+strlen(dst)];
	for(int i = 0; i<start; i++) {
		res[i] = src[i];
	}
	for(int i = 0; i<strlen(dst); i++) {
		res[start+i] = dst[i];
	}
	return res;
}
char* dataUtility::getbyte(char* src, int start, int length) {
	if (start+length > strlen(src))
		return "";
	char* res = new char[length+1];
	for(int i = 0; i<length; i++) {
		res[i] = src[start+i];
	}
	res[length-1] ='\0';
	return res;
}

void dataUtility::printChars(char* data) {
	cout << "char* data: ";
	for (int i = 0; i < strlen(data); i++)
	{
		cout << &(data[i]);
	}
	cout << endl;
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