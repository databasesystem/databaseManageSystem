#include "wq_parser.h"

void parser::testParse(){
	char filename[100];
	while(true) {
		printf("please input the command filename. If you want to quit, please input quit.\n");
		scanf("%s", filename);
		if (strcmp(filename, "quit") == 0)
			break;
		else
			BatchSqlInFile(filename);
	}
}

void parser::BatchSqlInFile(char* filename) {
	FILE* fin = fopen(filename, "r");
	if (fin == NULL)
	{
		printf("This file does not exist.\n");
		return;
	}
	char* command = new char[1000];
	while(!feof(fin)) {
		fgets(command, 1000,fin);
		printf("%s", command);
	}
}