#include "fileManage.h"
#include "globalVariable.h"
#include <iostream>
#include <stdio.h>
using namespace std;

void FileManage::writePageToFile(int pageid, char updatedata[PAGE_SIZE], char* filename){
	FILE* originfilestream;
	FILE* updatefilestream;
	const char* updatefilename = "newfile.txt";
	originfilestream = fopen(filename,"r");
	updatefilestream = fopen(updatefilename, "w+");
	if (originfilestream == NULL) {
		fseek(updatefilestream, pageid*PAGE_SIZE, SEEK_SET);
		fwrite(updatedata,PAGE_SIZE, 1, updatefilestream);
		fclose(updatefilestream);
		rename(updatefilename, filename);
		return;
	}
	int pageno = -1;
	char data[PAGE_SIZE];
	memset(data, '\0', PAGE_SIZE);
	while(fread(data, sizeof(char), PAGE_SIZE, originfilestream)) {
		pageno = pageno + 1;
		if (pageid == pageno)
			fwrite(updatedata,PAGE_SIZE, 1, updatefilestream);
		else
			fwrite(data, sizeof(char), PAGE_SIZE, updatefilestream);
		memset(data, '\0', PAGE_SIZE);
	}
	if (pageid > pageno) 
	{
		fseek(updatefilestream, pageid*PAGE_SIZE, SEEK_SET);
		fwrite(updatedata,PAGE_SIZE, 1, updatefilestream);
	}
	fclose(originfilestream);
	fclose(updatefilestream);
	remove(filename);
	rename(updatefilename, filename);
}
void FileManage::readPageFromFile(int pageid, char data[PAGE_SIZE], char* filename){
	FILE* filestream;
	filestream = fopen(filename,"r");
	int status = fseek(filestream, pageid*PAGE_SIZE, SEEK_SET);
	if (status == 0) {
		fread(data, PAGE_SIZE, 1, filestream);
		cout << data << endl;
	} else {
		cout << "Read page to file errors." << endl;
	}
	fclose(filestream);
}
