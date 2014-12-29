/*
 * data_utility.h
 *
 *  Created on: 23 Nov 2014
 *      Author: Jason Wong
 */

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
			char* data = (char*) malloc(sizeof(T));
			memcpy(data, &ori, sizeof(T));
			cout << "size:  " << sizeof(T)  << " data size " << strlen(data)<< endl;
			return data;
		};

		template<typename T> 
		static char* data_to_char(T ori, int length){
			char* data = (char*) malloc(length);
			memcpy(data, &ori, length);
			return data;
		};

		template<typename T> 
		static T* char_to_class(char* data) {
			T* pageinfo = (T*) malloc(sizeof(T));
			memcpy(pageinfo, data, sizeof(T));
			return pageinfo;
		}	

		static char* int_to_char(int data);
		static int* char_to_int(char* data);
		static int char2int(char* data);
		static double* char_to_double(char* data);
		static float* char_to_float(char* data);

		static char* getbyte(char* src, int start, int length);
		static void printChars(char* data);
		static void bytefillbyte(char* src, char* dst, int index);
		static void bytefillbyte(char* src, char* dst, int start, int length);   //some characters in src are replaced by in dst.
		static char bool_to_byte(bool data);
};
#endif /* DATA_UTILITY_H_ */
