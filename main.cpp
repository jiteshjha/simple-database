#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <map>
#include <vector>
#include <fstream>
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

class database_session {
    public:
		static bool is_set;
		static string database_name;
};

bool database_session::is_set = false;
string database_session::database_name = "";

//-------------------------------------------------

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

//---------------------------------------------------




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
		if(strcmp(token, "use") == 0) {
			printf("Matched %s\n", token);
			Use_database(fp1);
		}
		else if(strcmp(token, "create") == 0) {
			printf("Matched %s\n", token);
			Create(fp1);
		}
		else if(database_session::is_set == false){
			printf("Select a database to use first!\n");
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

			// create database [id]

			// If the database doesn't exist, create a directory
			// else display an Error
			// Reference : https://stackoverflow.com/questions/7430248/creating-a-new-directory-in-c

			struct stat st = {0};

			if (stat(token, &st) == -1) {
				mkdir(token, 0700);
				printf("Database created\n");
			} else {
				printf("Error creating database!\n"); exit(1);
			}

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

			// use [id]

			// If the database's folder exists, then use it 
			// else display an Error
			// Reference : https://stackoverflow.com/questions/7430248/creating-a-new-directory-in-c

			struct stat st = {0};

			if (stat(token, &st) == -1) {
				printf("Database doesn't exist!\n"); exit(1);
			} else {
				database_session::is_set = true;
				database_session::database_name = token;

				printf("Using specified database\n"); 
			}

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

	if(check == 1 && database_session::is_set == true) {
		if(is_identifier(token) != 0) {
			printf("Matched %s\n", token);

			create_table::table_name = token;

			check = next_token(fp1, token);
			if(check == 1) {
				if(strcmp(token, "(") == 0) {
					printf("Matched %s\n", token);
					Create_table_field(fp1);

					check = next_token(fp1, token);

					if(check == 1) {
						if(strcmp(token, ")") == 0) {
							printf("Matched %s\n", token);

							// create table [id] with fields as <id, datatpye>

							// Check if table already exists

							struct stat st = {0};

							string tablename = database_session::database_name + "/" + create_table::table_name;

							const char *cstr = tablename.c_str();
							
							if (stat(cstr, &st) == -1) {

								// create the folder and the metadata
								mkdir(cstr, 0700);

								string metadata = tablename + "/metadata.txt";

								ofstream out(metadata);

								int count = 0;

								for (vector<string>::const_iterator i = create_table::order.begin(); 
										i != create_table::order.end(); ++i) {
									
										string key = *i;
										string write_string = to_string(count) + "," + key + "," + create_table::fields[key];

										if(count != 0) {
											write_string = "\n" + write_string;
										}

										out << write_string;

										count += 1;
								}
						
								out.close();
								printf("Table created\n");
							} else {
								create_table_reset();
								printf("Error creating table or table already exists!\n"); exit(1);
							}							
						}
						else {
							create_table_reset();
							printf("Expected )\n"); exit(0);
						}

					} else {
						create_table_reset();
						printf("Incomplete query\n");
						exit(EXIT_SUCCESS);
					}
				}
				else {
					create_table_reset();
					printf("Expected (\n"); exit(0);
				}

			} else {
				create_table_reset();
				printf("Incomplete query\n");
				exit(EXIT_SUCCESS);
			}
		}		
		else {
			printf("Expected table name(identifier)\n"); exit(0);
		}

	} else if(database_session::is_set != true) {
		printf("Choose a database first!\n");
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
			string field_id = token;
			create_table::order.push_back(field_id);
			check = next_token(fp1, token);

			if(check == 1) {
				if(is_datatype(token) != 0) {
					printf("Matched %s\n", token);
					string datatype = token;

					// Check for duplicate field_id
					if(create_table::fields.count(field_id) == 0) {
						create_table::fields[field_id] = datatype;
						Create_table_field_ext(fp1);
					}
					else {
						create_table_reset();
						printf("Duplicate field name\n"); exit(0);
					}
				}		
				else {
					create_table_reset();
					printf("Expected data type(int or string)\n"); exit(0);
				}

			} else {
				create_table_reset();
				printf("Incomplete query\n");
				exit(EXIT_SUCCESS);
			}
		}		
		else {
			create_table_reset();
			printf("Expected attribute name(identifier)\n"); exit(0);
		}

	} else {
		create_table_reset();
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
					string field_id = token;
					create_table::order.push_back(field_id);
					check = next_token(fp1, token);

					if(check == 1) {
						if(is_datatype(token) != 0) {
							printf("Matched %s\n", token);
							string datatype = token;

							// Check for duplicate field_id
							if(create_table::fields.count(field_id) == 0) {
								create_table::fields[field_id] = datatype;
								Create_table_field_ext(fp1);
							}
							else {
								create_table_reset();
								printf("Duplicate field name\n"); exit(0);
							}
							
						}		
						else {
							create_table_reset();
							printf("Expected data type(int or string)\n"); exit(0);
						}

					} else {
						create_table_reset();
						printf("Incomplete query\n");
						exit(EXIT_SUCCESS);
					}
				}		
				else {
					create_table_reset();
					printf("Expected attribute name(identifier)\n"); exit(0);
				}

			} else {
				create_table_reset();
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

	char input[256];
	char continue_input;
	bool go_on = true;
	FILE *fp1;

	while(go_on == true) {
		printf(">");
		memset(input, '\0', sizeof(char)*256);
		cin.getline(input,sizeof(input));

		fp1 = fopen("text", "w+");
		fputs(input, fp1);

		fseek(fp1, 0, SEEK_SET);

		/*check = next_token(fp1, token);

		while(check == 1) { 
			printf("%s\n", token);
			check = next_token(fp1, token);
		}*/

		Program(fp1);

		fclose(fp1);
		
		printf("Continue(y/n)");
		scanf("%c", &continue_input);

		if(!(continue_input == 'y' || continue_input == 'Y')) {
			go_on = false;
		} else { getchar(); }
	}
	

	return 1;

}