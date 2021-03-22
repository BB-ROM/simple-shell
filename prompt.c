#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <wait.h>
#include <ctype.h>
#include "prompt.h"

// struct history
int historyCounter = 0;
int historyFull = -1;
int lastCommand = 0;
struct historyCommand historyCommands[HISTORY_SIZE];

char *aliases[ALIAS_MAX][2];


void print_prompt() {
    char arr[256];
    getcwd(arr, 255);
    printf("[%s]%s", arr, "==> ");
}

// useful for when input is too large
void clear_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// removes last newline character in a string
void remove_trailing_new_line(char *string) {
    char *new = strchr(string, '\n');
    if (new)
        *new = 0;
}

// ensures string such as "  " can be typed without segfault
void remove_leading_whitespace(char *input) {
    int index, s;
    index = 0;
    while (input[index] == ' ')
        index++;

    if (index != 0) {
        s = 0;
        while (input[s + index] != '\0') {
            input[s] = input[s + index];
            s++;
        }
        input[s] = '\0';
    }
}

// returns the index of alias or -1 if command is not an alias
int is_alias(char *command) {
    for (int i = 0; i < ALIAS_MAX; i++) {
        if (aliases[i][0] != NULL) {
            if (strcmp(command, aliases[i][0]) == 0) {
                return i;
            }
        }
    }
    return -1;
}


int extract_int_from_string(char *string) {
    char *p = string;
    // until no more characters
    while (*p) {
        if (isdigit(*p)) {
            // put digits into val
            long val = strtol(p, &p, 10);
            return val;
        } else {
            p++;
        }
    }
    return -1;
}

int is_history_invocation(char *token) {
    // string begins with ! - history invocation
    if (strncmp(token, "!", 1) == 0) {
        // !! - last command
        if (strncmp(token, "!!", 2) == 0) {
            int index = (historyCounter - 1) % HISTORY_SIZE;
            if (index < 0) {
                return index + HISTORY_SIZE;
            } else if (index <= HISTORY_SIZE) {
                return index;
            } else {
                return -1;
            }
            // two behaviours for full and non full history
        } else if (historyFull == 0) {
            // !-n invoke n to last command ex. !-1 last command, !-2 second to last command, !-20 twentieth command
            if (strncmp(token, "!-", 2) == 0) {
                int n = extract_int_from_string(token);
                if (n > HISTORY_SIZE || n == 0) {
                    return -1;
                }
                int index = historyCounter - n;
                if (index < 0) {
                    return index + HISTORY_SIZE;
                } else if (index <= HISTORY_SIZE) {
                    return index;
                } else {
                    return -1;
                }
            } else {
                // !n invoke nth command from history where 20 is most recent and 1 is the oldest
                int n = extract_int_from_string(token);
                if (n > HISTORY_SIZE || n == 0) {
                    return -1;
                }
                int index = (historyCounter - (HISTORY_SIZE - n));
                if (index <= 0) {
                    return index + HISTORY_SIZE - 1;
                } else if ((HISTORY_SIZE - n) == 0) {
                    return index - 1;
                } else if (index < HISTORY_SIZE) {
                    return index;
                } else {
                    return -1;
                }
            }
            // non empty history
        } else {
            // !-
            if (strncmp(token, "!-", 2) == 0) {
                int n = extract_int_from_string(token);
                if (n <= historyCounter && n > 0) {
                    return historyCounter - n;
                } else {
                    return -1;
                }
            } else {
                // !n
                int n = extract_int_from_string(token);
                if (n <= historyCounter && n > 0) {
                    return n - 1;
                } else {
                    return -1;
                }
            }
        }
    }
    return -1;
}

// substitutes from history and from aliases and tokenizes input
// tokens array - max is 50
// returns 0 if unsuccessful
int store_tokens(char **tokens, int size, char *input) {
    char DELIMITERS[] = " \t|><&;";

    // handling invoking aliases
    char *input_copy = malloc(INPUT_SIZE);
    strcpy(input_copy, input);
    strtok(input_copy, DELIMITERS);
    char *token = malloc(strlen(input_copy));
    strcpy(token, input_copy);
    int i = 0;
    int history_index = is_history_invocation(token);

    // checking if the first token is a history invocation and substituting
    if (history_index != -1) {
        free(in);
        in = malloc(INPUT_SIZE);
        strcat(in, historyCommands[history_index].command);
        strcpy(input, in);
    }
    // make space for command to be split into tokens
    free(in);
    in = malloc(INPUT_SIZE);
    strcpy(in, input);
    strtok(in, DELIMITERS);
    free(token);
    token = malloc(strlen(in));
    strcpy(token, in);

    int index = is_alias(token);
    // checking if the first token contains an alias name
    if(index != -1) {
        // replaces alias with a command
        free(input_copy);
        input_copy = malloc(INPUT_SIZE);
        strcat(input_copy, aliases[index][1]);
        strcat(input_copy, input + strlen(token));
        strcpy(input, input_copy);
    }
    free(input_copy);
    free(token);

    token = strtok(input, DELIMITERS);
    while (token) {
        tokens[i] = token;
        i++;
        token = strtok(NULL, DELIMITERS);
        // too many tokens - segfault
        if (i >= size - 1)
            return 0;
    }

    // last token should be null to work with exec()
    tokens[i] = NULL;
    return 1;
}

void put_input_in_history(char *input) {
    if (strcmp(input, "history") != 0 &&
        strncmp(input, "!", 1) != 0 &&
        strcmp(input, "exit") != 0) {
        historyCommands[historyCounter].commandNumber = lastCommand + 1;
        strcpy(historyCommands[historyCounter].command, input);
        if (historyCounter + 1 > 19)
            historyFull = 0;
        historyCounter = (historyCounter + 1) % HISTORY_SIZE;
        lastCommand++;
    }
}

// returns 0 if tokens can't be extracted from input and 1 if successful
int get_tokens(char **tokens, int size, char *input) {
    // checking for input too long or EOF
    if (input[strlen(input) - 1] != '\n') {
        if (strlen(input) > INPUT_SIZE - 1) {
            clear_stdin();
            printf("Input too large - default max character limit is 512 "
                   "characters (can be changed in config file)\n");
            return 0;
        } else {
            printf("\n");
            return -1;
        }
    }

    // leading white space is useless
    remove_leading_whitespace(input);
    // is input empty
    if (strlen(input) - 1 == 0) {
        return 0;
    }

    // \n at the end of string could potentially interfere with the commands - has to be stripped
    remove_trailing_new_line(input);
    char *input_copy = malloc(INPUT_SIZE);
    strcpy(input_copy, input);
    put_input_in_history(input_copy);
    if (!store_tokens(tokens, size, input)) {
        printf("Too many tokens - 50 is the maximum number of tokens user can input\n");
        return 0;
    }

    //string is correct
    return 1;
}

// helper function to take care of child processes after fork
void process_child_process(char **tokens) {
    // if command is an executable it runs
    if (execvp(tokens[0], tokens) == -1) {
        printf("%s: No such file or directory.\n", tokens[0]);
        exit(0);
    }
}

int fork_process(char **tokens) {
    pid_t pid;
    pid = fork();

    // unsuccessful
    if (pid < 0) {
        printf("Fork Failed\n");
        return 1;
    }
        // child process
    else if (pid == 0) {
        process_child_process(tokens);
    }
        // parent process
    else {
        wait(NULL);
    }
    return 0;
}

/**
 * Functions for commands
 */

// keywords that map commands to functions


// return how many arguments are in the tokens array
int get_number_of_args(char **tokens) {
    int length = 0;
    while (tokens[length] != NULL) {
        length++;
    }
    return length - 1;
}

// helper function
int is_path_valid(char *path) {
    char cwd[256];
    getcwd(cwd, 255);
    if (chdir(path) == 0) {
        chdir(cwd);
        return 1;
    } else {
        return 0;
    }
}

// shell command - prints PATH environmental variable on the screen
int getpath(char **args) {
    if (get_number_of_args(args) > 0) {
        printf("This command doesn't take any arguments\n");
    } else {
        printf("%s\n", getenv("PATH"));
    }
    return 0;
}

// shell command - sets PATH environmental variable
int setpath(char **args) {
    if (get_number_of_args(args) != 1) {
        printf("This command takes exactly one argument\n");
    } else if (is_path_valid(args[1])) {
        setenv("PATH", args[1], 1);
    } else {
        printf("Provided path has invalid format. Path should be "
               "a string of system paths separated by a comma\n");
    }
    return 0;
}

// shell command - changes current working directory to the provided path
// without arguments it takes user to home directory
int cd(char **args) {
    if (get_number_of_args(args) == 0) {
        chdir(getenv("HOME"));
    } else if (get_number_of_args(args) == 1) {
        if (chdir(args[1]) == -1) {
            printf("Error, this path doesn't exist.\n");
        }
    } else {
        printf("This command take either no or one argument: a path on the system\n");
    }
    return 1;
}

int alias(char **args) {
    int count_null = 0;
    char *alias_name;
    char command[INPUT_SIZE] = "";
    int count_tokens;
    int index;

    // counts number of null elements of alias array
    for (int i = 0; i < ALIAS_MAX; i++) {
        if(aliases[i][0] == NULL) {
            count_null = ALIAS_MAX - i;
            break;
        }
    }

    // checks to see if alias already entered and shows what aliases exists or says there are none
    if (args[1] == NULL) {
        if(count_null == ALIAS_MAX) {
            printf("There are no aliases to list\n");
        } else {
            for (int i = 0; (i < (ALIAS_MAX-count_null)); i++) {
                printf("%s\t%s \n", aliases[i][0], aliases[i][1]);
            }
        }
        return 0;
    } else {
        // will prompt for parameters if there has been an incorrect input
        if(args[2] == NULL) {
            printf("Incorrect parameters entered for alias. Specify <""second command"">\n");
            return 0;
        }
    }

    alias_name = args[1];
    count_tokens = 0;

    strcat(command, args[2]);

    // checks how many tokens are in the array
    while (args[count_tokens + 3] != NULL) {
        count_tokens++;
    }

    // concatenates all the tokens excluding the alias and command to be created
    for (int i = 0; i < count_tokens; i++) {
        strcat(command, " ");
        strcat(command, args[i + 3]);
    }

    index = is_alias(alias_name);

    // ensures no duplication of existing aliases by overwriting, warns user this has been done.
    if (index != -1) {
        aliases[index][1] = strdup(command);
        printf("Alias \"%s\" overwritten successfully\n", alias_name);
        return 0;
    }

    // if there are too many aliases in existence, warns user no more can be added
    if (count_null == 0) {
        printf("No more aliases can be added \n");
    } else {
        // adds the alias to the Array if it is not already there
        aliases[ALIAS_MAX - count_null][0] = strdup(alias_name);
        aliases[ALIAS_MAX - count_null][1] = strdup(command);
        printf("Alias \"%s\" added successfully\n", alias_name);
    }

    return 0;
}

int unalias(char **args) {
    char *alias_name;
    int index;

    // checks if an alias name is provided
    if (args[1] == NULL) {
        printf("No alias provided\n");
        return 0;
    }

    // checks if any unnecessary parameters are provided
    if (args[2] != NULL) {
        printf("Too many values provided\n");
        return 0;
    }

    alias_name = args[1];
    index = is_alias(alias_name);

    // removes alias if exists and shifts the next elements of aliases to fill the empty space
    if (index != -1) {
        while (index < (ALIAS_MAX - 1)) {
            aliases[index][0] = aliases[index + 1][0];
            aliases[index][1] = aliases[index + 1][1];
            index++;
        }
        aliases[ALIAS_MAX - 1][0] = NULL;
        aliases[ALIAS_MAX - 1][1] = NULL;
        printf("Alias \"%s\" removed successfully\n", alias_name);
        return 0;
    }

    printf("Alias \"%s\" does not exist\n", alias_name);

    return 0;
}

// returns the index of function or -1 if command is not a function
int is_command(char *command) {
    char *commands_calls[] = {
            "getpath",
            "setpath",
            "cd",
            "alias",
            "unalias",
            "history"
    };
    int num_of_commands = sizeof(commands_calls) / sizeof(char *);
    for (int i = 0; i < num_of_commands; i++) {
        if (strcmp(command, commands_calls[i]) == 0)
            return i;
    }
    return -1;
}

void save_history(){
    FILE *file;
    //opens file to be written to
    file = fopen(".hist_list", "w");

    if(file == NULL){
        printf("Unable to save history, a file access error occurred");
    }
    for(int i = 0; i < HISTORY_SIZE; i++){
        fprintf(file, "\"%s\n", historyCommands[i]);
    }
    fclose(file);
}

void load_history() {
    char line[INPUT_SIZE];
    FILE *file;
// opens the file in a read mode
    file = fopen(".hist_list", "r");

// returns if the file does not exist or is inaccessible
    if (file == NULL) {
        printf("No previous history found, history will be saved on exit");
        return;
    }

// reads file line by line
    while (fgets(line, sizeof(line), file) != NULL && historyCounter < 20){
        historyCommands[historyCounter] = strdup(strtok(line, "\n"));
        historyCounter++;
    }
    fclose(file);
}

int print_history(char **tokens) {
    if (historyFull == 0) {
        int j = 1;
        for (int i = historyCounter; i < HISTORY_SIZE; i++) {
            printf("%d.%s\n", j, historyCommands[i].command);
            ++j;
        }
        for (int i = 0; i < historyCounter; i++) {
            printf("%d.%s\n", j, historyCommands[i].command);
            ++j;
        }

    } else {
        for (int i = 0; i < historyCounter; i++) {
            printf("%d.%s\n", i + 1, historyCommands[i].command);
        }
    }

    return 0;
}

// executes a command with provided arguments
int exec_command(int command, char **tokens) {
    int (*commands[])(char **) = {
            &getpath,
            &setpath,
            &cd,
            &alias,
            &unalias,
            &print_history
    };
    commands[command](tokens);
    return 0;
}

void save_aliases() {
    FILE *file;
    // opens the file in a write mode
    file = fopen(".aliases", "w");

    // prints an error if a file is inaccessible
    if(file == NULL) {
        printf("Error: Aliases file access denied.\n");
    }

    // writes aliases to the file line by line
    for (int i = 0; i < ALIAS_MAX; i++) {
        // breaks the loop at the first empty alias
        if(aliases[i][0] == NULL) {
            break;
        }
        fprintf(file, "%s %s\n", aliases[i][0], aliases[i][1]);
    }
    
    fclose(file);
}

void load_aliases() {
    FILE *file;
    // opens the file in a read mode
    file = fopen(".aliases", "r");

    // returns if the file does not exist or is inaccessible
    if(file == NULL) {
        return;
    }

    char line[INPUT_SIZE];
    char* line_tokens[TOKENS_SIZE] = {"0"};
    char *alias_name;
    char command[INPUT_SIZE] = "";
    int count_tokens;
    int index;

    index = 0;

    // reads file line by line
    while(fgets(line, sizeof(line), file) != NULL && index < ALIAS_MAX) {
        get_tokens(line_tokens, TOKENS_SIZE, line);

        alias_name = line_tokens[0];
        command[0] = '\0';
        count_tokens = 0;

        strcat(command, line_tokens[1]);

        // checks how many tokens are in the array
        while (line_tokens[count_tokens+2] != NULL) {
            count_tokens++;
        }

        // concentrates the command with its arguments
        for (int i = 0; i < count_tokens; i++) {
            strcat(command, " ");
            strcat(command, line_tokens[i+2]);
        }

        // updates the aliases array
        aliases[index][0] = strdup(alias_name);
        aliases[index][1] = strdup(command);

        index++;
    }
  
    fclose(file);
}
