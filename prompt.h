#define INPUT_SIZE 512
#define TOKENS_SIZE 50
#define ALIAS_MAX 10
#define HISTORY_SIZE 20

void print_prompt();
int fork_process(char **tokens);
int get_tokens(char **tokens, int size, char *input);
int is_command(char* tokens);
int exec_command(int command, char** args);
void save_history();
void load_history();
void save_aliases();
void load_aliases();

//history
typedef struct historyCommand {
    int commandNumber;
    char command[INPUT_SIZE];
} historyCommand;