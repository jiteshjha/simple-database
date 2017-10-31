#include<cstdio>
#include<cstring>
#include<cstdlib>
#include<iostream>
#include<string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <map>
#include "generate_token.cpp"

using namespace std;

class create_table {
	public:
		static bool started;
		static bool ended;
		static string table_name;		
		static map<string,string> fields;

};

bool create_table::started = false;
bool create_table::ended = false;
string create_table::table_name = "";
map<string,string> create_table::fields;

int main(int argc, char * argv[]) {
    create_table::fields["sd"] = "sdds";
    
    cout<<create_table::fields["sd"];

    if(create_table::fields.count("sd") != 0) {
        cout<<"ddg";
    }
    return 0;
}