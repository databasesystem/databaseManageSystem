#include <Windows.h>
#include "DBPageType.h"
#include "DBFile.h"
#include "DBFBuf.h"
#include "DBPageManager.h"
#include "DBIndexManager.h"
#include "DBError.h"
#include "RecordManage.h"

#include <stdio.h>
#include <direct.h>
#include <stdlib.h>
#include <string.h>

int createFileFolder(char* filefoldername) {
int status = _mkdir(filefoldername);
if (status == 0) {
printf("%s was successfully created.\n", filefoldername);
} else {
printf("%s was fail to be created.\n", filefoldername);
}
return status;
}

int deleteFilefolder(char* filefoldername) {
int status = _rmdir(filefoldername);
if (status == 0) {
printf("%s was successfully deleted.\n", filefoldername);
} else {
printf("%s was fail to ben deleted.\n", filefoldername);;
}
return status;
}

void createDatabase(char* databaseName) {
char* databasename = "StudentManageSystem";
int t = createFileFolder(databasename);
}


void deleteDatabase(char* databaseName){
char* databasename = "StudentManageSystem";
int t = deleteFilefolder(databasename);
}


void createTable(char* databaseName, char* tableName, char* command){
char* tablepath = "./StudentManageSystem/StudentInfo";
char* newcommand = "CREATE TABLE customer (id int, name char(255), rank int)";
int pt = createFileFolder(tablepath);
int error;
char* filepath = "./StudentManageSystem/StudentInfo/attr.txt";
DB_PAGE* buf;
int pageid;
int fileid = 0;
char* temp = "id";
int startindex = 2;
int i = 0;
if((error = DBCreateFile(filepath))!=DB_OK)
{
DBPrintError(error);
}
if((error = DBAllocPage(fileid, &pageid, &buf))!=DB_OK)
{
DBPrintError(error);
}
buf->pgheader.pageID = pageid;
buf->pgheader.objID = fileid;
buf->pgdata[0] = 3;
buf->pgdata[1] = 3;

for(i = 0; i< strlen(temp); i++) {
buf->pgdata[i+startindex] = temp[i];
}
buf->pgdata[startindex+strlen(temp)] = 0;    //0 int  , 1 double , 2 char 

startindex = startindex+strlen(temp) +1;
temp = "name";
for(i = 0; i< strlen(temp); i++) {
buf->pgdata[i+startindex] = temp[i];
}
buf->pgdata[startindex+strlen(temp)] = 2;    //0 int  , 1 double , 2 char 

startindex = startindex+strlen(temp) +1;
temp = "rank";
for(i = 0; i< strlen(temp); i++) {
buf->pgdata[i+startindex] = temp[i];
}
buf->pgdata[startindex+strlen(temp)] = 0;    //0 int  , 1 double , 2 char 
DBWriteFile(fileid, pageid, buf);
}


void insertData(char* databaseName, char* tableName, char* command){

}


void deleteData(char* databaseName, char* tableName, char* command){

}


void searchData(char* databaseName, char* tableName, char* command){

}