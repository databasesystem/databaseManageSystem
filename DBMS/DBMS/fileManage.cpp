#include "fileManage.h"
#include "globalVariable.h"
#include "data_utility.h"
#include <iostream>
#include <stdio.h>
#include <direct.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

void FileManage::writePageToFile(int pageid, dbPage pagedata, char* filename){
	FILE* originfilestream;
	FILE* updatefilestream;
	char* writedata = dataUtility::data_to_char<dbPage>(pagedata);
	writedata[sizeof(dbPage)-1]='\0';
	const char* updatefilename = "newfile.txt";
	originfilestream = fopen(filename,"r");
	updatefilestream = fopen(updatefilename, "w+");
	if (originfilestream == NULL) {
		fseek(updatefilestream, pageid*sizeof(dbPage), SEEK_SET);
		fwrite(writedata,sizeof(dbPage), 1, updatefilestream);
		fclose(updatefilestream);
		rename(updatefilename, filename);
		return;
	}
	int pageno = -1;
	char data[sizeof(dbPage)];
	memset(data, '\0', sizeof(dbPage));
	while(fread(data, sizeof(char), sizeof(dbPage), originfilestream)) {
		pageno = pageno + 1;
		if (pageid == pageno)
			fwrite(writedata,sizeof(dbPage), 1, updatefilestream);
		else
			fwrite(data, sizeof(char), sizeof(dbPage), updatefilestream);
		memset(data, '\0', sizeof(dbPage));
	}
	if (pageid > pageno) 
	{
		fseek(updatefilestream, pageid*sizeof(dbPage), SEEK_SET);
		fwrite(writedata,sizeof(dbPage), 1, updatefilestream);
	}
	fclose(originfilestream);
	fclose(updatefilestream);
	remove(filename);
	rename(updatefilename, filename);
}
void FileManage::readPageFromFile(int pageid, dbPage* pageinfo, char* filename){
	FILE* filestream;
	char* data = new char[sizeof(dbPage)];
	filestream = fopen(filename,"r");
	int status = fseek(filestream, pageid*sizeof(pageinfo), SEEK_SET);
	if (status == 0) {
		fread(data, PAGE_SIZE, 1, filestream);
		*pageinfo = *dataUtility::char_to_class<dbPage>(data);
	} else {
		cout << "Read page to file errors." << endl;
	}
	fclose(filestream);
}


int FileManage::createFileFolder(char* filefoldername) {
	int status = _mkdir(filefoldername);
	if (status == 0) {
		printf("%s was successfully created.\n", filefoldername);
	} else {
		printf("%s was fail to be created.\n", filefoldername);
	}
	return status;
}
int FileManage::deleteFileFolder(char* filefoldername) {
	int status = _rmdir(filefoldername);
	if (status == 0) {
		printf("%s was successfully deleted.\n", filefoldername);
	} else {
		printf("%s was fail to ben deleted.\n", filefoldername);;
	}
	return status;
}