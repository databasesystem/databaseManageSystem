#include "fileManage.h"
#include "data_utility.h"
#include <iostream>
#include <stdio.h>
#include <direct.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

void FileManage::writePageToFile(int pageid, dbPage* pagedata, char* filename){
	FILE* originfilestream;
	FILE* updatefilestream;
	char* updatefilename = new char[strlen(filename)+5];
	strcpy(updatefilename, filename);
	strcat(updatefilename, ".tmp");
	
	originfilestream = fopen(filename,"r");
	updatefilestream = fopen(updatefilename, "w+");
	if (originfilestream == NULL) {
		fseek(updatefilestream, pageid*sizeof(dbPage), SEEK_SET);
		fwrite(pagedata,sizeof(dbPage), 1, updatefilestream);
	} else {
		int pageno = -1;
		char data[sizeof(dbPage)];
		memset(data, 0, sizeof(dbPage));
		while(fread(data, sizeof(char), sizeof(dbPage), originfilestream)) {
			pageno = pageno + 1;
			if (pageid == pageno) {
				fwrite(pagedata,sizeof(dbPage),1, updatefilestream);
				cout << "update the page pageno: "  << pageno << endl;
			}
			else {
				fwrite(data, sizeof(char), sizeof(dbPage), updatefilestream);
				cout << "copy origin file pageno " << pageno << endl;
			}
			memset(data, 0, sizeof(dbPage));
		}
		if (pageid > pageno) 
		{
			fseek(updatefilestream, pageid*sizeof(dbPage), SEEK_SET);
			fwrite(pagedata,sizeof(char), sizeof(dbPage), updatefilestream);
		}

		fclose(originfilestream);
		remove(filename);

	}
	fclose(updatefilestream);
	rename(updatefilename, filename);
	delete[] updatefilename;
}

void FileManage::readPageFromFile(int pageid, dbPage* pageinfo, char* filename){
	FILE* filestream;
	filestream = fopen(filename,"r");
	int status = fseek(filestream, pageid*sizeof(dbPage), SEEK_SET);
	if (status == 0) {
		fread(pageinfo, sizeof(dbPage), 1, filestream);
	} else {
		cout << "Read page from file error: " << pageid << " filename: " << filename << endl;
		pageinfo->header.fileId = 0;
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
