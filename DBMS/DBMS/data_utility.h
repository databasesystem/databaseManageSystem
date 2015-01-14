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
		static T char_to_data(char* data){
			return *((T*)data);
		}
		
		template<typename T>
		static char* data_to_char(T ori){
			char* data = new char[sizeof(T)+1];
			memcpy(data, &ori, sizeof(T));
			data[sizeof(T)] = 0;
			return data;
		}

		static void string_fill_char(char* dst, string ori, int index, int size);
		static void string_to_char (char* dst, string ori, int index, int stringLength, int dstLength);
		static char* getbyte(char* src, int start, int length);
		static void bytefillbyte(char* dst, BYTE* src, int start, BYTE length);   //some characters in src are replaced by in dst.
		static bool stringOptstring(string leftstr, int opt, string rightstr);
		static bool intOptint(int leftvalue, int opt, int rightvalue);
		static string toUpper(string s);
		static bool isCmp(char c);
		static bool isEnglishAlphabet(char c);
		static bool isDig(char c);
};
#endif /* DATA_UTILITY_H_ */