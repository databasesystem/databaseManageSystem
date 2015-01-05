#ifndef DATA_UTILITY_H_
#define DATA_UTILITY_H_
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;
 
class dataUtility {
public: 
		template<typename T> 
		static char* data_to_char(T ori){
			char* data = (char*) malloc(sizeof(T)+1);
			memcpy(data, &ori, sizeof(T));
			data[sizeof(T)] = 0;
			return data;
		};

		template<typename T> 
		static T* char_to_class(char* data) {
			T* pageinfo = (T*) malloc(sizeof(T));
			memcpy(pageinfo, data, sizeof(T));
			return pageinfo;
		};

		template<typename T>
		static T char_to_class_reference(char* data){
			return *((T*)data);
		}

		static int char2short(char* data);
		static char* getbyte(char* src, int start, int length);
		static void bytefillbyte(char* src, char* dst, int index);
		static void bytefillbyte(char* dst, char* src, int start, int length);   //some characters in src are replaced by in dst.
		static char bool_to_byte(bool data);
};
#endif /* DATA_UTILITY_H_ */
