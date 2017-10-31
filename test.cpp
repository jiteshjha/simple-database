#include<cstdio>
#include<cstring>
#include<cstdlib>
#include<iostream>
#include<string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <map>
#include <vector>
#include "generate_token.cpp"

using namespace std;

class create_table {
	public:
		static string table_name;		
		static map<string,string> fields;
		static vector<string> order;
};



string create_table::table_name = "";
map<string,string> create_table::fields;
vector<string> create_table::order;

void create_table_reset() {
    create_table::table_name = "";
    create_table::fields.clear();
	create_table::order.clear();
}

int main(int argc, char * argv[]) {
    
    return 0;
}