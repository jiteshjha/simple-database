#include<cstdio>
#include<cstring>
#include<cstdlib>
#include<iostream>
#include<string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "generate_token.cpp"

using namespace std;



int main(int argc, char * argv[]) {
    
    char token[] = "abcd";

    string t = token;
    string str = "hey " + t;
    cout<<str;

    return 0;
}