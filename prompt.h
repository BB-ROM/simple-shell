#define INPUT_SIZE 512
#define TOKENS_SIZE 50 // allows for n - 2 tokens
#define ALIAS_MAX 10
#define HISTORY_SIZE 20

typedef struct History {
    int counter;
    int is_full;
    char *commands[HISTORY_SIZE];
} History;

/**
 * prints the prompt
 */
void print_prompt();

/**
 * strips input of leading whitespace, ensures its proper length and removes
 * trailing new line and takes care of ctrl-d being pressed if input is larger
 * than INPUT_SIZE it returns null, if ctrl-d was pressed, it returns exit, otherwise
 * returns a sanitized string
 */
char *sanitize_input(char *input_copy);

/**
 * substitutes from history and from aliases and tokenizes input
 * @param input sanitized input to be tokenized
 * @return the a pointer to an array of tokens
 */
char **tokenize_input(char *input);


/**
 * checks if input is a shell commnad
 * @param command the first token of an input string - the command
 * @return index of the command in command array
 */
int is_command(char *command);

/**
 * executes a command of give index
 * @param index index of the command in the command array
 * @param tokens tokenized input
 * @param history command history
 * @param aliases aliases
 */
void exec_command(int index, char **tokens, History history, char *aliases[ALIAS_MAX][2]);

/**
 * forks process into parent and child
 * @param tokens
 * @return
 */
int fork_process(char **tokens);

/**
 * store sanitized input into the command history
 * @param history command history
 * @param input_copy sanitized input
 */
void store_in_history(History *history, char *input_copy);

/**
 * performs substitution if the input is a history invocation
 * @param history command history
 * @param input sanitized input
 * @return substituted command
 */
char *substitute_from_history(History history, char *input);

/**
 * performs substitution if the input is an aliased command
 * @param history command history
 * @param input sanitized input
 * @return substituted command
 */
char *substitute_from_aliases(char *aliases[ALIAS_MAX][2], char *input);

/**
 * loads history from .hist_list file in users home directory
 * @param history command history
 */
void load_history(History *history);

/**
 * save history into a .hist_list file in users home directory
 * @param history
 */
void save_history(History history);

/**
 * saves aliases into a .aliases file in users home directory
 * @param aliases
 */
void save_aliases(char *aliases[ALIAS_MAX][2]);

/**
 * load aliases from .aliases file in users home directory
 * @param aliases
 */
void load_aliases(char *aliases[ALIAS_MAX][2]);

void print_tokens(char **tokens);
