#include<cstdio>
#include<cstring>
#include<cstdlib>

using namespace std;

int next_token(FILE *fp1, char * temp) {
	
	char c;
	int i, j = 0;

	memset(temp, '\0', sizeof(char)*50);

	c = fgetc(fp1); 
	if(c == ' ' || c == '\n') {
		c = fgetc(fp1);
		while(c == ' ' || c == '\n') {
			c = fgetc(fp1);
		}
	}
	
	if(c == ';') {
		return 0;
	}

	//Generate token for string tokens
	if(c == '"') {
		temp[j++] = c;
		c = fgetc(fp1);

		while(c != '"' && c != ';') {
			temp[j++] = c;
			c = fgetc(fp1);
		}

		if(c == '"') {
			temp[j++] = c;
		} else {
			fseek(fp1, -1, SEEK_CUR);
		}

		temp[j] = '\0';

	} 
	// Generate token for tokens not string
	else {
		
		while((c >= 'A' && c <= 'Z') || ( c >= 'a' && c <= 'z') || ( c >= '0' && c <= '9') && c != ';') {
			temp[j++] = c;
			c = fgetc(fp1);
		}

		// Terminate string
		temp[j] = '\0';

		if (j != 0) {
			fseek(fp1, -1, SEEK_CUR);
		}
		else {
			if(c == '(' || c == ')' || c == ',' || c == '*' || c == '=') {
				temp[0] = c;
				temp[1] = '\0';
			}
			else {
				printf("Add new characters to tokenizer\n");
				exit(0);
			}
		}
	} 


	return 1;
}


