#include<cstdio>
#include<cstring>
#include<cstdlib>
#include "generate_token.cpp"

using namespace std;

void Program(FILE *);
void Create(FILE *);
void Create_database(FILE *);
void Use_database(FILE *);
void Create_table(FILE *);
void Create_table_field(FILE *);
void Create_table_field_ext(FILE *);

/* Data structure [Start] */



/* Data structure [End] */

/* Helper functions [Start] */
int is_identifier(char * str) {
	int len = strlen(str);

	if(len < 1) {
		return 0;
	}

	int i = 1;

	if(!((str[0] >= 'A' && str[0] <= 'Z') || ( str[0] >= 'a' && str[0] <= 'z'))) {
		return 0;
	}

	while(i < len) {
		if(!((str[0] >= 'A' && str[0] <= 'Z') || ( str[0] >= 'a' && str[0] <= 'z'))){
			return 0;
		}
		i += 1;
	}

	return 1;
}

int is_string(char * str) {
	int len = strlen(str);

	if(len < 1) {
		return 0;
	}

	if(str[0] == '"' && str[len-1] == '"') {
		return 1;
	}

	return 0;
}

int is_integer(char * str) {
	
	int len = strlen(str);

	if(len < 1) {
		return 0;
	}
	
	int i = 0;

	while(i < len) {
		if(str[i] < '0' || str[i] > '9') {
			return 0;
		}
		i += 1;
	}

	return 1;

}

int is_datatype(char * str) {
	// returns 1 for int and 2 for string
	// returns 0 for mismatch
	const char * data_type[2] = {"int", "string"};
	int flag = 0, i;

	for (i = 0; i < 2; i++) {
		if (strcmp(str, data_type[i]) == 0) {
			flag = i+1;
			break;
		}
	}

	return flag;
}
/*Helper functions [End] */


void Program(FILE * fp1) {
	int check;
	char token[50];
	check = next_token(fp1, token);
	
	if(check == 1) {
		if(strcmp(token, "create") == 0) {
			printf("Matched %s\n", token);
			Create(fp1);
		}
		else if(strcmp(token, "use") == 0) {
			printf("Matched %s\n", token);
			Use_database(fp1);
		}
		else {
			printf("Expected create | select | insert | update | use | delete\n"); exit(0);
		}

	} else {
		printf("Incomplete query\n");
		exit(EXIT_SUCCESS);
	}
}

void Create(FILE *fp1) {
	int check;
	char token[50];
	check = next_token(fp1, token);

	if(check == 1) {
		if(strcmp(token, "database") == 0) {
			printf("Matched %s\n", token);
			Create_database(fp1);
		}
		else if(strcmp(token, "table") == 0) {
			printf("Matched %s\n", token);
			Create_table(fp1);
		}		
		else {
			printf("Expected database | table\n"); exit(0);
		}

	} else {
		printf("Incomplete query\n");
		exit(EXIT_SUCCESS);
	}
}

void Create_database(FILE *fp1) {
	int check;
	char token[50];
	check = next_token(fp1, token);

	if(check == 1) {
		if(is_identifier(token) != 0) {
			printf("Matched %s\n", token);
		}		
		else {
			printf("Expected database name(identifier)\n"); exit(0);
		}

	} else {
		printf("Incomplete query\n");
		exit(EXIT_SUCCESS);
	}
}

void Use_database(FILE *fp1) {
	int check;
	char token[50];
	check = next_token(fp1, token);

	if(check == 1) {
		if(is_identifier(token) != 0) {
			printf("Matched %s\n", token);
		}		
		else {
			printf("Expected database name(identifier)\n"); exit(0);
		}

	} else {
		printf("Incomplete query\n");
		exit(EXIT_SUCCESS);
	}
}

void Create_table(FILE *fp1) {
	int check;
	char token[50];
	check = next_token(fp1, token);

	if(check == 1) {
		if(is_identifier(token) != 0) {
			printf("Matched %s\n", token);

			check = next_token(fp1, token);
			if(check == 1) {
				if(strcmp(token, "(") == 0) {
					printf("Matched %s\n", token);
					Create_table_field(fp1);

					check = next_token(fp1, token);

					if(check == 1) {
						if(strcmp(token, ")") == 0) {
							printf("Matched %s\n", token);							
						}
						else {
							printf("Expected )\n"); exit(0);
						}

					} else {
						printf("Incomplete query\n");
						exit(EXIT_SUCCESS);
					}
				}
				else {
					printf("Expected (\n"); exit(0);
				}

			} else {
				printf("Incomplete query\n");
				exit(EXIT_SUCCESS);
			}
		}		
		else {
			printf("Expected table name(identifier)\n"); exit(0);
		}

	} else {
		printf("Incomplete query\n");
		exit(EXIT_SUCCESS);
	}
}

void Create_table_field(FILE *fp1) {
	int check;
	char token[50];
	check = next_token(fp1, token);

	if(check == 1) {
		if(is_identifier(token) != 0) {
			printf("Matched %s\n", token);

			check = next_token(fp1, token);

			if(check == 1) {
				if(is_datatype(token) != 0) {
					printf("Matched %s\n", token);
					
					Create_table_field_ext(fp1);
				}		
				else {
					printf("Expected data type(int or string)\n"); exit(0);
				}

			} else {
				printf("Incomplete query\n");
				exit(EXIT_SUCCESS);
			}
		}		
		else {
			printf("Expected attribute name(identifier)\n"); exit(0);
		}

	} else {
		printf("Incomplete query\n");
		exit(EXIT_SUCCESS);
	}
}

void Create_table_field_ext(FILE *fp1) {
	int check;
	char token[50];
	check = next_token(fp1, token);

	if(check == 1) {
		if(strcmp(token, ",") == 0) {
			printf("Matched %s\n", token);

			check = next_token(fp1, token);
			if(check == 1) {
				if(is_identifier(token) != 0) {
					printf("Matched %s\n", token);

					check = next_token(fp1, token);

					if(check == 1) {
						if(is_datatype(token) != 0) {
							printf("Matched %s\n", token);

							Create_table_field_ext(fp1);
							
						}		
						else {
							printf("Expected data type(int or string)\n"); exit(0);
						}

					} else {
						printf("Incomplete query\n");
						exit(EXIT_SUCCESS);
					}
				}		
				else {
					printf("Expected attribute name(identifier)\n"); exit(0);
				}

			} else {
				printf("Incomplete query\n");
				exit(EXIT_SUCCESS);
			}
		}		
		else {
			fseek(fp1, (-1 * strlen(token)), SEEK_CUR);
		}

	} else {
		printf("Incomplete query\n");
		exit(EXIT_SUCCESS);
	}
}



int main(int argc, char * argv[]) {
	char token[50];

	int check;

	FILE *fp1 = fopen(argv[1], "r");

	if(fp1 == NULL) {
	 	printf("File cant be open %s\n", argv[1]);
	 	exit(0);
	}

	Program(fp1);

	/*check = next_token(fp1, token);

	while(check == 1) { 
		printf("%s\n", token);
		check = next_token(fp1, token);
	}*/

	return 1;

}
