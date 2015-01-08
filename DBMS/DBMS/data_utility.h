#ifndef DATA_UTILITY_H_
#define DATA_UTILITY_H_
#include "globalVariable.h"
#include <iostream>
#include <string>

using namespace std;
 
class dataUtility {
public: 
		template<typename T> 
		static void data_fill_char(char* dst, T ori, int index, int size){
			char* data = new char[size];
			memcpy(data, &ori, size);
			for (int i = 0; i < size; i++){
				dst[index+i] = data[i];
			}
			delete data;
		};

		template<typename T>
		static T char_to_class(char* data){
			return *((T*)data);
		}

		static void string_fill_char(char* dst, string ori, int index, int size);
		static char* getbyte(char* src, int start, int length);
		static void bytefillbyte(char* src, char* dst, int index);
		static void bytefillbyte(char* dst, char* src, int start, int length);   //some characters in src are replaced by in dst.
};
#endif /* DATA_UTILITY_H_ */