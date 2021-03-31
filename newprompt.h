#define INPUT_SIZE 512
#define TOKENS_SIZE 50 // allows for n - 2 tokens
#define ALIAS_MAX 10
#define HISTORY_SIZE 20

typedef struct History {
    int counter;
    int is_full;
    char* commands[HISTORY_SIZE];
} History;

typedef struct Aliases {
    int i;
} Aliases;

void print_prompt();
char* sanitize_input(char *input_copy);
char** tokenize_input(char* input);
void print_tokens(char **tokens);
int is_command(char *command);
void exec_command(int command, char **tokens, History history, char* aliases[ALIAS_MAX][2]);
int fork_process(char **tokens);
void store_in_history(History *history, char *input_copy);
char* substitute_from_history(History history, char* input);
void load_history(History *history);
void save_history(History history);
void print_history(History history);
void save_aliases(char* aliases[ALIAS_MAX][2]);
void load_aliases(char* aliases[ALIAS_MAX][2]);
//int fork_process(char **tokens);
//int is_command(char* tokens);
//int exec_command(int command, char** args);
//void save_history();
//void load_history();
//void save_aliases();
//void load_aliases();
//void clear_stdin();

