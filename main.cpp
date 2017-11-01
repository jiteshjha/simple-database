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

void Select(FILE *);
void Select_field(FILE *);

void Delete(FILE *);

void Update(FILE *);

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

//----------------------------------------------------

class select {
	public:
		static bool require_all_fields;
		static string table_name;
		static vector<string> field_names;
};


bool select::require_all_fields = false;
string select::table_name = "";
vector<string> select::field_names;

void select_reset() {
	select::require_all_fields = false;
    select::table_name = "";
    select::field_names.clear();
}

//--------------------------------------------------------

class delete_command {
	public:
		static string table_name;
		static string fieldname;
		static string fieldvalue;
};

string delete_command::table_name = "";
string delete_command::fieldname = "";
string delete_command::fieldvalue = "";

void delete_reset() {
	delete_command::table_name = "";
	delete_command::fieldname = "";
	delete_command::fieldvalue = "";
}

//----------------------------------------------------------

class update {
	public:
		static string table_name;
		static string setname;
		static string setvalue;
		static string wherename;
		static string wherevalue;
};

string update::table_name = "";
string update::setname = "";
string update::setvalue = "";
string update::wherename = "";
string update::wherevalue = "";

void update_reset() {
	update::table_name = "";
	update::setname = "";
	update::setvalue = "";
	update::wherename = "";
	update::wherevalue = "";
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
		else if(strcmp(token, "select") == 0) {
			printf("Matched %s\n", token);
			Select(fp1);
		}
		else if(strcmp(token, "delete") == 0) {
			printf("Matched %s\n", token);
			Delete(fp1);
		}
		else if(strcmp(token, "update") == 0) {
			printf("Matched %s\n", token);
			Update(fp1);
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

								// Create a blank data.txt

								ofstream outagain(tablename+"/data.txt");
								outagain.close();

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

											if(tokens[2] == "int" && is_integer(insert::field_values[i]) == 0
											   ) {
												insert_reset();
												infile.close();
												cout<<"Datatype of value " + insert::field_values[i] + 
												" mismatch; Expected integer\n"; exit(0);												
											}
											else if(tokens[2] == "string" &&is_string(insert::field_values[i]) == 0) {
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

void Select(FILE *fp1) {
	int check;
	char token[50];
	check = next_token(fp1, token);

	if(check == 1) {
		if(strcmp(token, "*") == 0) {
			printf("Matched %s\n", token);
			select::require_all_fields = true;
		}
		else if(is_identifier(token) != 0) {
			printf("Matched %s\n", token);
			select::require_all_fields = false;
			select::field_names.push_back(token);
			Select_field(fp1);
		}
		else {
			select_reset();
			printf("Expected * or attribute name\n"); exit(0);
		}

		check = next_token(fp1, token);

		if(check == 1) {
			if(strcmp(token, "from") == 0) {
				printf("Matched %s\n", token);

				check = next_token(fp1, token);

				if(check == 1) {
					if(is_identifier(token) != 0) {
						printf("Matched %s\n", token);

						select::table_name = token;
						// select ... from [id];

						// First check whether the table exists or not.

						struct stat st = {0};

						string tablename = database_session::database_name + "/" + select::table_name;

						const char *cstr = tablename.c_str();

						if (stat(cstr, &st) == -1) {
							printf("Table doesn't exist!\n"); exit(1);
						} else {
							// So, Table exists

						
							// If the table is empty, show the message
							if(number_lines_file(tablename+"/data.txt") == 0) {
									printf("Empty table\n");
							}
							else {
								
								// If * operator is used, print all the data
								if(select::require_all_fields == true) {
									ifstream infile(tablename+"/data.txt");
									string line;								

									while (std::getline(infile, line)) {
										cout<<line<<"\n";
									}

									infile.close();

									select_reset();
									printf(".\n.\n.\nSelect operation complete\n");
								}
								else { // print data only from specified fields

									// Load metadata in the form of <fieldname, position>
									ifstream infile(tablename+"/metadata.txt");

									map<string, int> metadata_pair;

									string line;
									while (std::getline(infile, line)) {
										vector<string> tokens = string_tokenize_on_comma(line);
										
										string key = tokens[1];
										string str_value = tokens[0];
										int value;
										stringstream(str_value) >> value;
										metadata_pair[key] = value;

									}

									infile.close();

									

									// At this point, we have the metadata in the required pair

									ifstream datafile(tablename+"/data.txt");

									while (std::getline(datafile, line)) {
										
										int i = 0;
										vector<string> tokens = string_tokenize_on_comma(line);

										for (vector<string>::const_iterator iter = select::field_names.begin(); 
												iter != select::field_names.end(); ++iter) {											
											
												string field = *iter;
												
												int position = metadata_pair[field];

												if(i != 0) {
													printf(",");
												}

												cout<<tokens[position];

												i += 1;
										}
										cout<<"\n";
									}

									datafile.close();
									metadata_pair.clear();
									select_reset();
									printf(".\n.\n.\nSelect operation complete\n"); 
								}						
							}
						}
					}
					else {
						select_reset();
						printf("Expected a valid table name\n"); exit(0);
					}
				} else {
					select_reset();
					printf("Incomplete query\n");
					exit(EXIT_SUCCESS);
				}
			}
			else {
				select_reset();
				printf("Expected keyword from\n"); exit(0);
			}
		} else {
			select_reset();
			printf("Incomplete query\n");
			exit(EXIT_SUCCESS);
		}

	} else {
		
		printf("Incomplete query\n");
		exit(EXIT_SUCCESS);
	}
}

void Select_field(FILE *fp1) {
	int check;
	char token[50];
	check = next_token(fp1, token);

	if(check == 1) {
		if(strcmp(token, ",") == 0) {
			printf("Matched %s\n", token);

			check = next_token(fp1, token);
			if(is_identifier(token) != 0) {
				printf("Matched %s\n", token);
				select::field_names.push_back(token);
				Select_field(fp1);				
			}
			else {
				select_reset();
				printf("Expected attribute name\n"); exit(0);
			}
		}
		else {
			fseek(fp1, (-1 * strlen(token)), SEEK_CUR);
		}

	} else {
		select_reset();
		printf("Incomplete query\n");
		exit(EXIT_SUCCESS);
	}
}

void Delete(FILE *fp1) {
	int check;
	char token[50];
	check = next_token(fp1, token);

	if(check == 1) {
		if(strcmp(token, "from") == 0) {
			printf("Matched %s\n", token);
			
			check = next_token(fp1, token);

			if(check == 1) {
				if(is_identifier(token) != 0) {
					printf("Matched %s\n", token);
					
					delete_command::table_name = token;

					check = next_token(fp1, token);

					if(check == 1) {
						if(strcmp(token, "where") == 0) {
							printf("Matched %s\n", token);
							
							check = next_token(fp1, token);	

							if(check == 1) {
								if(is_identifier(token) != 0) {
									printf("Matched %s\n", token);
									delete_command::fieldname = token;
									
									check = next_token(fp1, token);	

									if(check == 1) {
										if(strcmp(token, "=") == 0) {
											printf("Matched %s\n", token);											
											
											check = next_token(fp1, token);
											
											if(check == 1) {
												if(is_string(token) != 0 || is_integer(token) != 0) {
													printf("Matched %s\n", token);
													delete_command::fieldvalue = token;
													
													// delete ... where [id] = [id]

													// First, let's check whether the table exists or not

													struct stat st = {0};
													string tablename = database_session::database_name + "/" + delete_command::table_name;

													const char *cstr = tablename.c_str();
													if (stat(cstr, &st) == -1) {
														delete_reset();
														printf("Table doesn't exist!\n"); exit(1);
													} else {
														// So, the table exists

														// Now, check if the attribute in the query, exists or not
														// Also, if the attribute exists, load the datatype and check the value against it
														
														string line;

														ifstream datafile(tablename+"/metadata.txt");

														bool attribute_exists = false;
														string attribute_datatype = "";
														int position;

														while (std::getline(datafile, line)) {
															vector<string> tokens = string_tokenize_on_comma(line);

															if(tokens[1].compare(delete_command::fieldname) == 0) {
																attribute_exists = true;
																attribute_datatype = tokens[2];
																stringstream(tokens[0]) >> position;
																break;
															}
														}

														datafile.close();

														if(attribute_exists == true) {
															// So, attribute exists
															// Now let's check the datatype

															if(attribute_datatype.compare("int") == 0
															 && is_integer(delete_command::fieldvalue) == 0) {
																delete_reset();
																printf("Expected int datatype for attribute!\n"); exit(1);
															 }
															 else if(attribute_datatype.compare("string") == 0
															 && is_string(delete_command::fieldvalue) == 0) {
																delete_reset();
																printf("Expected string datatype for attribute!\n"); exit(1);
															 }
															 else {
																// datatype of value checks out, now let's
																// read the data.txt, and write only those tuples which do not satisfy
																// the where clause

																ifstream datafile(tablename+"/data.txt");
																ofstream out(tablename+"/temp.txt");

																int i = 0;

																bool tuples_deleted = false;
																																
																while (std::getline(datafile, line)) {
																	vector<string> tokens = string_tokenize_on_comma(line);
																	
																	if(tokens[position].compare(delete_command::fieldvalue) != 0) {
																		if(i != 0) {
																			out << "\n";
																		}
																		out << line;
																		i += 1;		
																	} else {
																		tuples_deleted = true;
																	}																															
																}

																datafile.close();
																out.close();

																string tempfilename = tablename + "/temp.txt";
																if(tuples_deleted == false) {
																	printf("No tuples deleted\n");
																	
																	remove(tempfilename.c_str());
																}else {
																	printf("Delete operation completed!\n");
																	string datafilename = tablename + "/data.txt";
																	remove(datafilename.c_str());
																	rename(tempfilename.c_str(), datafilename.c_str());
																}

																delete_reset();															
															 }

															
														} else {
															delete_reset();
															printf("Attribute doesn't exist!\n"); exit(1);
														}


													}

													delete_reset();
												}
												else {
													delete_reset();
													printf("Expected attribute value\n"); exit(0);
												}

											} else {
												delete_reset();
												printf("Incomplete query\n");
												exit(EXIT_SUCCESS);
											}
										}
										else {
											delete_reset();
											printf("Expected operator =\n"); exit(0);
										}

									} else {
										delete_reset();
										printf("Incomplete query\n");
										exit(EXIT_SUCCESS);
									}									
								}
								else {
									delete_reset();
									printf("Expected attribute name\n"); exit(0);
								}

							} else {
								delete_reset();
								printf("Incomplete query\n");
								exit(EXIT_SUCCESS);
							}
							
						}
						else {
							delete_reset();
							printf("Expected keyword where\n"); exit(0);
						}

					} else {
						// delete from [id]

						// First check the existence of table

						struct stat st = {0};

						if (stat(token, &st) == -1) {
							printf("Table doesn't exist!\n"); exit(1);
						} else {

							// Clear the data.txt

							string tablename = database_session::database_name + "/" + delete_command::table_name;
							ofstream ofs;
							ofs.open(tablename+"/data.txt", ofstream::out | ofstream::trunc);
							ofs.close();

							delete_reset();
							printf("Table content deleted\n");

						}

					}
					
				}
				else {
					delete_reset();
					printf("Expected table name\n"); exit(0);
				}

			} else {
				printf("Incomplete query\n");
				exit(EXIT_SUCCESS);
			}
		}
		else {
			printf("Expected keyword from\n"); exit(0);
		}

	} else {
		printf("Incomplete query\n");
		exit(EXIT_SUCCESS);
	}
}

void Update(FILE *fp1) {
	int check;
	char token[50];
	check = next_token(fp1, token);

	if(check == 1) {
		if(is_identifier(token) != 0) {
			printf("Matched %s\n", token);
			update::table_name = token;

			check = next_token(fp1, token);
			
			if(check == 1) {
				if(strcmp(token, "set") == 0) {
					printf("Matched %s\n", token);
					
					check = next_token(fp1, token);

					if(check == 1) {
						if(is_identifier(token) != 0) {
							printf("Matched %s\n", token);
							update::setname = token;

							check = next_token(fp1, token);

							if(check == 1) {
								if(strcmp(token, "=") == 0) {
									printf("Matched %s\n", token);									
									
									check = next_token(fp1, token);									
									
									if(check == 1) {
										if(is_string(token) != 0 || is_integer(token) != 0) {
											printf("Matched %s\n", token);
											update::setvalue = token;
											
											check = next_token(fp1, token);									
											
											if(check == 1) {
												if(strcmp(token, "where") == 0) {
													printf("Matched %s\n", token);
													
													check = next_token(fp1, token);									
													
													if(check == 1) {
														if(is_identifier(token) != 0) {
															printf("Matched %s\n", token);
															update::wherename = token;

															check = next_token(fp1, token);

															if(check == 1) {
																if(strcmp(token, "=") == 0) {
																	printf("Matched %s\n", token);

																	check = next_token(fp1, token);

																	if(check == 1) {
																		if(is_string(token) != 0 || is_integer(token) != 0) {
																			printf("Matched %s\n", token);
																			update::wherevalue = token;

																			// update ... where [id] = [id]
																			

																		}		
																		else {
																			update_reset();
																			printf("Expected where condition value\n"); exit(0);
																		}

																	} else {
																		update_reset();
																		printf("Incomplete query\n");
																		exit(EXIT_SUCCESS);
																	}

																	
																}		
																else {
																	update_reset();
																	printf("Expected operator =\n"); exit(0);
																}

															} else {
																update_reset();
																printf("Incomplete query\n");
																exit(EXIT_SUCCESS);
															}
														}		
														else {
															update_reset();
															printf("Expected where condition attribute\n"); exit(0);
														}

													} else {
														update_reset();
														printf("Incomplete query\n");
														exit(EXIT_SUCCESS);
													}													
												}	
												else {
													update_reset();
													printf("Expected keyword where\n"); exit(0);
												}

											} else {
												// update [id] set [id] = [id]

												// first check the existence of table

												struct stat st = {0};

												string tablename = database_session::database_name + "/" + update::table_name;

												if (stat(tablename.c_str(), &st) == -1) {
													printf("Table doesn't exist!\n"); exit(1);
												} else {
													// So, table exists

													// Check whether table is empty or not

													if(number_lines_file(tablename+"/data.txt") != 0) {
														// Table is not empty,

														// Now, let's validate whether attribute exists

														string line;

														ifstream datafile(tablename+"/metadata.txt");

														bool attribute_exists = false;
														string attribute_datatype = "";
														int position;

														while (std::getline(datafile, line)) {
															vector<string> tokens = string_tokenize_on_comma(line);

															if(tokens[1].compare(update::setname) == 0) {
																attribute_exists = true;
																attribute_datatype = tokens[2];
																stringstream(tokens[0]) >> position;
																break;
															}
														}

														datafile.close();

														if(attribute_exists == true) {
															// So, attribute exists
															// Now let's check the datatype

															if(attribute_datatype.compare("int") == 0
															 && is_integer(update::setvalue) == 0) {
																delete_reset();
																printf("Expected int datatype for set value!\n"); exit(1);
															 }
															 else if(attribute_datatype.compare("string") == 0
															 && is_string(update::setvalue) == 0) {
																delete_reset();
																printf("Expected string datatype for set value!\n"); exit(1);
															 }
															 else {
																// datatype of value checks out, now let's
																// change the value of the attribute as specified 
																// in set

																ifstream datafile(tablename+"/data.txt");
																ofstream out(tablename+"/temp.txt");

																int i = 0;
																																
																while (std::getline(datafile, line)) {
																	vector<string> tokens = string_tokenize_on_comma(line);

																	tokens[position] = update::setvalue;

																	if(i != 0) {
																		out << "\n";
																		
																	}																	
																
																	out << string_join_on_comma(tokens);
																	i = 1;
																}

																datafile.close();
																out.close();

																string datafilename = tablename + "/data.txt";
																string tempfilename = tablename + "/temp.txt";
																remove(datafilename.c_str());
																rename(tempfilename.c_str(), datafilename.c_str());

																update_reset();
																printf("Update completed\n");
															 }

														} else {
															update_reset();
															printf("Set attribute doesn't exists!\n"); exit(1);
														}
													}
													else {
														update_reset();
														printf("Table is empty!\n"); exit(1);
													}
												}
											}
										}	
										else {
											update_reset();
											printf("Expected set value\n"); exit(0);
										}

									} else {
										update_reset();
										printf("Incomplete query\n");
										exit(EXIT_SUCCESS);
									}
								}	
								else {
									update_reset();
									printf("Expected operator =\n"); exit(0);
								}

							} else {
								update_reset();
								printf("Incomplete query\n");
								exit(EXIT_SUCCESS);
							}
							
						}	
						else {
							update_reset();
							printf("Expected set attribute\n"); exit(0);
						}

					} else {
						update_reset();
						printf("Incomplete query\n");
						exit(EXIT_SUCCESS);
					}
				}	
				else {
					update_reset();
					printf("Expected keyword set\n"); exit(0);
				}

			} else {
				update_reset();
				printf("Incomplete query\n");
				exit(EXIT_SUCCESS);
			}
		}	
		else {
			update_reset();
			printf("Expected table name\n"); exit(0);
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