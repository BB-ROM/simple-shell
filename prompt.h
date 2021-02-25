#define INPUT_SIZE 512
#define TOKENS_SIZE 50

void print_prompt();
void clear_stdin();
void remove_trailing_new_line(char *string);
void remove_leading_whitespace(char *input);
int store_tokens(char **tokens, int size, char *input);
int fork_process(char **tokens);
void process_child_process(char **tokens);
int get_tokens(char **tokens, int size, char *input);
void print_tokens(char **tokens);

int is_command(char* tokens);
int exec_command(int command, char** args);
int get_number_of_args(char** tokens);
int is_path_valid(char* path);

//commands
int getpath(char** args);
int setpath(char** args);
int cd(char** args);

