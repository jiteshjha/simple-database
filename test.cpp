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

int number_lines_file(string filename) {
	int number_of_lines = 0;
    string line;
    ifstream myfile(filename);

    while (getline(myfile, line))
        ++number_of_lines;
    
	return number_of_lines;
}

int main(int argc, char * argv[]) {
    
	struct stat st = {0};

	string tablename = "xyz/sdss";

	const char *cstr = tablename.c_str();


	if (stat(cstr, &st) == -1) {
		cout<<"No";
	}
	else {
		cout<<"Yes";
	}

	return 0;
}