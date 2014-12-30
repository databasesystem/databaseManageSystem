#include "parser.h"

int getChar(){
	string buf, item;
	vector<string> query;
	cin >> buf;
	istringstream sin(buf);
	while(!sin.eof()){
		sin >> item;
		if(!item.empty() || item != " ")
			query.push_back(item);
	}
	return 1;
}

/*int parser(){
	vector<string> query = getChar();
	for(vector<string>::iterator it = query.begin(); it != query.end(); it++){
		if( tolower(it->c_str) == 
	}
}*/