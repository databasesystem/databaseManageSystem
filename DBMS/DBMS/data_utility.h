/*
 * data_utility.h
 *
 *  Created on: 23 Nov 2014
 *      Author: Jason Wong
 */

#ifndef DATA_UTILITY_H_
#define DATA_UTILITY_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pageManage.h"


 
class dataUtility {
public: 
		template<typename T> 
		static char* data_to_char(T ori){
			char* data = (char*) malloc(sizeof(T));
			memcpy(data, &ori, sizeof(T));
			return data;
		};
		template<typename T> 
		static char* data_to_char(T ori, int length){
			char* data = (char*) malloc(length);
			memcpy(data, &ori, length);
			return data;
		};
		static int* char_to_int(char* data);
		static double* char_to_double(char* data);
		static float* char_to_float(char* data);

		template<typename T> 
		static T* char_to_class(char* data) {
			T* pageinfo = (T*) malloc(sizeof(T));
			memcpy(pageinfo, data, sizeof(T));
			return pageinfo;
		}	

		static char* bytefillbyte(char* src, char* dst, int start);
		static char* getbyte(char* src, int start, int length);

		static void printChars(char* data);
};



#endif /* DATA_UTILITY_H_ */
