#include<cstdio>
#include<cstring>
#include<cstdlib>
#include<iostream>
#include<string>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <map>
#include <vector>
#include "generate_token.cpp"

using namespace std;

string string_join_on_comma(vector<string> vec_str) {
	string str = "";

	int i = 0;

	for (vector<string>::const_iterator iter = vec_str.begin(); 
		iter != vec_str.end(); ++iter) {

			if(i != 0) {
				str += ",";
			}

			str += *iter;
			i += 1;
		}	


		return str;
}

int main(int argc, char * argv[]) {
    
	vector<string> str;

	str.push_back("hey");
	str.push_back("sds");
	str.push_back("sdsdsd");

	cout<<string_join_on_comma(str);
	return 0;
}