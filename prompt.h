#define INPUT_SIZE 512
#define TOKENS_SIZE 50
#define ALIAS_MAX 2

void print_prompt();
int fork_process(char **tokens);
int get_tokens(char **tokens, int size, char *input);
void print_tokens(char **tokens);
int is_command(char* tokens);
int exec_command(int command, char** args);
void save_aliases();
