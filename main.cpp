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
#include <sstream>
#include "generate_token.cpp"

using namespace std;

void Program(FILE *);
void Create(FILE *);
void Create_database(FILE *);
void Use_database(FILE *);
void Create_table(FILE *);
void Create_table_field(FILE *);
void Create_table_field_ext(FILE *);
void Insert(FILE *);
void Insert_field(FILE *);
void Insert_field_ext(FILE *);

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

class insert {
	public:
		static string table_name;
		static vector<string> field_values;
};

string insert::table_name = "";
vector<string> insert::field_values;

void insert_reset() {
    insert::table_name = "";
    insert::field_values.clear();
}

/* Data structure [End] */

/* Helper functions [Start] */
int is_identifier(char * str) {
	int len = strlen(str);

	if(len < 1) {
		return 0;
	}

	string keywords[] = {"int", "string", "use", "database", "create", "table",
						"insert", "into", "values", "select", "from", "where",
						"delete"};

	for(int i = 0; i < 13; i++) {
		string temp = str;
		if(temp.compare(keywords[i]) == 0) {
			return 0;
		}
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

int is_string(string str) {
	int len = str.length();

	if(len < 1) {
		return 0;
	}

	if(str[0] == '"' && str[len-1] == '"') {
		return 1;
	}

	return 0;
}

int is_integer(string str) {
	
	int len = str.length();

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

int number_lines_file(string filename) {
	int number_of_lines = 0;
    string line;
    ifstream myfile(filename);

    while (getline(myfile, line))
        ++number_of_lines;
    
	myfile.close();
	return number_of_lines;
}

vector<string> string_tokenize_on_comma(string str) {
	vector<string> strings;

    istringstream f(str);
    string s;   

    while (getline(f, s, ',')) {
        strings.push_back(s);
    }
	return strings;
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
		else if(strcmp(token, "insert") == 0) {
			printf("Matched %s\n", token);
			Insert(fp1);
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

void Insert(FILE *fp1) {
	int check;
	char token[50];
	check = next_token(fp1, token);

	if(check == 1) {
		if(strcmp(token, "into") == 0) {
			printf("Matched %s\n", token);
			
			check = next_token(fp1, token);

			if(is_identifier(token) != 0) {
				printf("Matched %s\n", token);
				insert::table_name = token;

				check = next_token(fp1, token);				

				if(strcmp(token, "values") == 0) {
					printf("Matched %s\n", token);
					
					check = next_token(fp1, token);

					if(strcmp(token, "(") == 0) {
						printf("Matched %s\n", token);
						
						Insert_field(fp1);

						check = next_token(fp1, token);

						if(strcmp(token, ")") == 0) {
							printf("Matched %s\n", token);

							// insert into [id] values (...)

							// If the table's folder exists, then use it 
							// else display an Error
							// Reference : https://stackoverflow.com/questions/7430248/creating-a-new-directory-in-c

							struct stat st = {0};

							string tablename = database_session::database_name + "/" + insert::table_name;

							const char *cstr = tablename.c_str();

							if (stat(cstr, &st) == -1) {
								printf("Table doesn't exist!\n"); exit(1);
							} else {
								//So, table exists

								/* first check if number of fields in insert commands
								   matches the number of fields in the table */

								if(number_lines_file(tablename+"/metadata.txt") == insert::field_values.size()) {

									/* let's check whether the datatypes of values 
									   are as per the metadata of the table */

									   ifstream infile(tablename+"/metadata.txt");
									   string line;

									   int i = 0;

									   while (std::getline(infile, line)) {
											vector<string> tokens = string_tokenize_on_comma(line);
											
											string value_str = insert::field_values[i];

											if(is_integer(insert::field_values[i]) != 0 
											   && tokens[2] != "int") {
												insert_reset();
												infile.close();
												cout<<"Datatype of value " + insert::field_values[i] + 
												" mismatch; Expected integer\n"; exit(0);												
											}
											else if(is_string(insert::field_values[i]) != 0 
											   && tokens[2] != "string") {
												insert_reset();
												infile.close();
												cout<<"Datatype of value " + insert::field_values[i] + 
												" mismatch; Expected string\n"; exit(0);												
											}

											i += 1;
									   }

									   infile.close();

									   // At this point, datatypes of all values are correct.
									   // Now, let's write the values as CSV into data.txt.

									   
									   string datafile = tablename + "/data.txt";

										ofstream out(datafile, ios_base::app);

										string write_line = "";

										if(number_lines_file(datafile) > 0) {
											write_line = "\n";
										}

										i = 0;

										for (vector<string>::const_iterator iter = insert::field_values.begin(); 
												iter != insert::field_values.end(); ++iter) {
											
												string key = *iter;
												
												if(i != 0) {
													write_line = write_line + ",";
												}

												write_line += key;

												i += 1;
										}

										out << write_line;
								
										insert_reset();
										out.close();

										printf("Insertion complete\n");
								}
								else {
									insert_reset();
									printf("Incorrect number of values\n"); exit(0);
								}
							}
						}		
						else {
							insert_reset();
							printf("Expected )\n"); exit(0);
						}						
					}		
					else {
						insert_reset();
						printf("Expected (\n"); exit(0);
					}
				}		
				else {
					insert_reset();
					printf("Expected keyword values\n"); exit(0);
				}
			}		
			else {
				printf("Expected valid table name\n"); exit(0);
			}
		}		
		else {
			printf("Expected into\n"); exit(0);
		}

	} else {
		printf("Incomplete query\n");
		exit(EXIT_SUCCESS);
	}
}

void Insert_field(FILE *fp1) {
	int check;
	char token[50];
	check = next_token(fp1, token);

	if(check == 1) {
		if(is_integer(token) != 0) {
			printf("Matched %s\n", token);
			insert::field_values.push_back(token);
			
			Insert_field_ext(fp1);			
		}
		else if(is_string(token) != 0) {
			printf("Matched %s\n", token);
			insert::field_values.push_back(token);

			Insert_field_ext(fp1);			
		}	
		else {
			insert_reset();
			printf("Expected integer value or string value\n"); exit(0);
		}
	} else {
		insert_reset();
		printf("Incomplete query\n");
		exit(EXIT_SUCCESS);
	}
}

void Insert_field_ext(FILE *fp1) {
	int check;
	char token[50];
	check = next_token(fp1, token);

	if(check == 1) {
		if(strcmp(token, ",") == 0) {
			printf("Matched %s\n", token);
			
			check = next_token(fp1, token);

			if(check == 1) {
				if(is_integer(token) != 0) {
					printf("Matched %s\n", token);
					insert::field_values.push_back(token);

					Insert_field_ext(fp1);
				}
				else if(is_string(token) != 0) {
					printf("Matched %s\n", token);
					insert::field_values.push_back(token);

					Insert_field_ext(fp1);
				}	
				else {
					printf("Expected integer value or string value\n"); exit(0);
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