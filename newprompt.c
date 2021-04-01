#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <wait.h>
#include "newprompt.h"

// removes leading spaces
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

// extracts an integer from a string
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

void clear_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void remove_trailing_new_line(char *string) {
    char *new = strchr(string, '\n');
    if (new)
        *new = 0;
}

// return how many arguments are in the tokens array
int get_number_of_args(char **tokens) {
    int length = 0;
    while (tokens[length] != NULL) {
        length++;
    }
    return length - 1;
}

// shell command - prints PATH environmental variable on the screen
int getpath(char **args) {
    if (get_number_of_args(args) > 0) {
        printf("This command doesn't take any arguments, and neither do I\n");
    } else {
        printf("%s\n", getenv("PATH"));
    }
    return 0;
}

// checks if entered path is a valid path on the system
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

// shell command - sets PATH environmental variable
int setpath(char **args) {
    if (get_number_of_args(args) != 1) {
        printf("This command takes exactly one argument, please only put one ;( )\n");
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
            printf("Error, path: %s doesn't exist.\n", args[1]);
        }
    } else {
        printf("This command take either no or one argument: a path on the system\n");
    }
    return 1;
}

// prints history list
void print_history(History history) {
    if (history.is_full == 0) {
        int j = 1;
        for (int i = history.counter; i < HISTORY_SIZE; i++) {
            printf("%d.%s\n", j, history.commands[i]);
            ++j;
        }
        for (int i = 0; i < history.counter; i++) {
            printf("%d.%s\n", j, history.commands[i]);
            ++j;
        }

    } else {
        for (int i = 0; i < history.counter; i++) {
            printf("%d.%s\n", i + 1, history.commands[i]);
        }
    }

}

// returns the index of alias or -1 if command is not an alias
int is_alias(char *command, char *aliases[ALIAS_MAX][2]) {
    for (int i = 0; i < ALIAS_MAX; i++) {
        if (aliases[i][0] != NULL) {
            if (strcmp(command, aliases[i][0]) == 0) {
                return i;
            }
        }
    }
    return -1;
}

// shell command - with no argument it lists currently saved aliases
// alias <name> <command with parameters> - saves an alias
int alias(char **args, char *aliases[ALIAS_MAX][2]) {
    int count_null = 0;
    char *alias_name;
    char command[INPUT_SIZE] = "";
    int count_tokens;
    int index;

    // counts number of null elements of alias array
    for (int i = 0; i < ALIAS_MAX; i++) {
        if (aliases[i][0] == NULL) {
            count_null = ALIAS_MAX - i;
            break;
        }
    }

    // checks to see if alias already entered and shows what aliases exists or says there are none
    if (args[1] == NULL) {
        if (count_null == ALIAS_MAX) {
            printf("There are no aliases to list\n");
        } else {
            for (int i = 0; (i < (ALIAS_MAX - count_null)); i++) {
                printf("%s\t%s \n", aliases[i][0], aliases[i][1]);
            }
        }
        return 0;
    } else {
        // will prompt for parameters if there has been an incorrect input
        if (args[2] == NULL) {
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

    index = is_alias(alias_name, aliases);

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

// shell command - removes an alias
int unalias(char **args, char *aliases[ALIAS_MAX][2]) {
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
    index = is_alias(alias_name, aliases);

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

// returns index of history invocation to be called or -1 if the token in not an invocation
int is_history_invocation(History history, char *token) {
    // string begins with ! - history invocation
    if (strncmp(token, "!", 1) == 0) {
        // !! - last command
        int counter = history.counter;
        if (strncmp(token, "!!", 2) == 0) {
            int index = (counter - 1) % HISTORY_SIZE;
            if (history.is_full != 0 && counter == 0) {
                return -1;
            } else if (index < 0) {
                return index + HISTORY_SIZE;
            } else if (index <= HISTORY_SIZE) {
                return index;
            } else {
                return -1;
            }

            // two behaviours for full and non full history
        } else if (history.is_full == 0) {
            // !-n invoke n to last command ex. !-1 last command, !-2 second to last command, !-20 twentieth command
            if (strncmp(token, "!-", 2) == 0) {
                int n = extract_int_from_string(token);
                if (n > HISTORY_SIZE || n <= 0) {
                    printf("History invocation should be !-<number>, where number is between 1 and 20.\n");
                    return -1;
                }
                int index = counter - n;
                if (index < 0) {
                    return index + HISTORY_SIZE;
                } else if (index <= HISTORY_SIZE) {
                    return index;
                } else {
                    printf("History invocation should be !<number>, where number is between 1 and 20.\n");
                    return -1;
                }
            } else {
                // !n invoke nth command from history where 20 is most recent and 1 is the oldest
                int n = extract_int_from_string(token);
                if (n > HISTORY_SIZE || n <= 0) {
                    printf("History invocation should be !-<number>, where number is between 1 and 20.\n");
                    return -1;
                }
                int index = (counter - (HISTORY_SIZE - n));
                if (index <= 0) {
                    return index + HISTORY_SIZE - 1;
                } else if ((HISTORY_SIZE - n) == 0) {
                    return index - 1;
                } else if (index < HISTORY_SIZE) {
                    return index;
                } else {
                    printf("History invocation should be !<number>, where number is between 1 and 20.\n");
                    return -1;
                }
            }
            // non empty history
        } else {
            // !-
            if (strncmp(token, "!-", 2) == 0) {
                int n = extract_int_from_string(token);
                if (n <= counter && n > 0) {
                    return counter - n;
                } else {
                    if (counter == 0) {
                        printf("History is empty, enter a command first.\n");
                    } else {
                        printf("History invocation should be !<number>, where number is between 0 and %d.\n",
                               counter);
                    }
                    return -1;
                }
            } else {
                // !n
                int n = extract_int_from_string(token);
                if (n <= counter && n > 0) {
                    return n - 1;
                } else {
                    if (counter == 0) {
                        printf("History is empty, enter a command first.\n");
                    } else {
                        printf("History invocation should be !<number>, where number is between 1 and %d.\n",
                               counter);
                    }
                    return -1;
                }
            }
        }
    }
    return -1;
}

void print_prompt() {
    char arr[256];
    getcwd(arr, 255);
    printf("[%s]%s", arr, "==> ");
}

char *sanitize_input(char *input) {
    // checking for input_copy too long or EOF
    char *input_copy = malloc(INPUT_SIZE);
    strcpy(input_copy, input);

    if (input_copy[strlen(input_copy) - 1] != '\n') {
        if (strlen(input_copy) > INPUT_SIZE - 1) {
            clear_stdin();
            printf("Input too large - default max character limit is 512.\n");
            return "";
        } else {
            printf("ctrld-pressed, exiting.\n");
            return "exit";
        }
    }

    // leading white space is useless
    remove_leading_whitespace(input_copy);
    // is input_copy empty
    if (strlen(input_copy) - 1 == 0) {
        printf("input empty\n");
        return "";
    }

    // \n at the end of string could potentially interfere with the commands - has to be stripped ;(
    remove_trailing_new_line(input_copy);
    return input_copy;
}

char **tokenize_input(char *input) {
    char DELIMITERS[] = " \t|><&;";
    char **tokens = malloc(sizeof(char *) * TOKENS_SIZE);

    // handling invoking history
    char *input_copy = malloc(INPUT_SIZE); // create copy of input
    strcpy(input_copy, input);
    char *token = strtok(input_copy, DELIMITERS); // get first token out of copy

    int i = 0;
    while (token) {
        tokens[i] = token;
        i++;
        token = strtok(NULL, DELIMITERS);
        // too many tokens
        if (i >= TOKENS_SIZE - 1) {
            printf("too many tokens\n");
            return NULL;
        }
    }

    // last token should be null to work with exec()
    tokens[i] = NULL;
    return tokens;
}

int is_command(char *command) {
    char *commands_calls[] = {
            "getpath",
            "setpath",
            "cd",
            "history",
            "alias",
            "unalias"
    };
    int num_of_commands = sizeof(commands_calls) / sizeof(char *);
    for (int i = 0; i < num_of_commands; i++) {
        if (strcmp(command, commands_calls[i]) == 0)
            return i;
    }
    return -1;
}

void exec_command(int command, char **tokens, History history, char* aliases[ALIAS_MAX][2]) {
    if (command <= 2) {
        int (*tokens_commands[])(char **) = {
                &getpath,
                &setpath,
                &cd,
        };
        tokens_commands[command](tokens);
    } else if (command == 3) {
        print_history(history); // add tokens to parameters and check if there are none

    } else if (command <= 5) {
        int (*tokens_commands[])(char **, char* [ALIAS_MAX][2]) = {
                &alias,
                &unalias
        };
        tokens_commands[command-4](tokens, aliases);
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
        if (execvp(tokens[0], tokens) == -1) {
            printf("%s: No such file or directory.\n", tokens[0]);
            exit(0);
        }
    }
        // parent process
    else {
        wait(NULL);
    }
    return 0;
}

void store_in_history(History *history, char *input) {
    char *input_copy = malloc(sizeof(char) * INPUT_SIZE);
    strcpy(input_copy, input);
        if(history->is_full == 0){
            memset(history->commands[history->counter],0 , sizeof (char) * INPUT_SIZE);//(history->commands[history->counter]));
        }
        history->commands[history->counter] = malloc(sizeof (char) * INPUT_SIZE);
        strcpy(history->commands[history->counter], input_copy);

        if (history->counter + 1 > 19)
            history->is_full = 0;
        history->counter = (history->counter + 1) % HISTORY_SIZE;
}

char* substitute_from_history(History history, char* input) {
    char DELIMITERS[] = " \t|><&;";
    char *input_copy = malloc(sizeof(char) * INPUT_SIZE);
    strcpy(input_copy, input);
    char *first = strtok(input_copy, DELIMITERS);
    char *second = strtok(NULL, DELIMITERS);

    int history_index = is_history_invocation(history, first);

    if (history_index != -1) {
        if (second != NULL) {
            printf("History invocations take no arguments.\n");
            return NULL;
        } else {
            char *out = malloc(sizeof(char) * INPUT_SIZE);
            memset(out, 0, strlen(out));
            strcat(out, history.commands[history_index]);
            return out;
        }
    }
    memset(input_copy, 0, strlen(input_copy));
    free(input_copy);
    return input;
}

char *substitute_from_aliases(char *aliases[ALIAS_MAX][2], char *input) {
    char DELIMITERS[] = " \t|><&;";
    char *input_copy = malloc(sizeof(char) * INPUT_SIZE);
    strcpy(input_copy, input);
    char *token = strtok(input_copy, DELIMITERS);

    int index = is_alias(token, aliases);
    // checking if the first token contains an alias name
    if (index != -1) {
        char *out = malloc(sizeof(char) * (INPUT_SIZE + strlen(aliases[index][1])));
        memset(out, 0, strlen(out));
        strcat(out, aliases[index][1]);
        strcat(out, input + strlen(token));
        return out;
    }

    memset(input_copy, 0, strlen(input_copy));
    free(input_copy);
    return input;
}

void load_history(History *history) {
    char line[INPUT_SIZE];
    FILE *file;
    // opens the file in a read mode
    char *user_home_dir_path = malloc(sizeof(char) * 256);
    memset(user_home_dir_path, 0, strlen(user_home_dir_path));
    user_home_dir_path = strcat(user_home_dir_path, getenv("HOME"));
    user_home_dir_path = strcat(user_home_dir_path, "/.hist_list");
    // opens the file in a read mode
    file = fopen(user_home_dir_path, "r");
    // returns if the file does not exist or is inaccessible
    if (file == NULL) {
        printf("No previous history found, history will be saved on exit.\n");

    } else {
        // reads file line by line
        while (fgets(line, sizeof(line), file) != NULL && (history->counter < HISTORY_SIZE)) {
            remove_trailing_new_line(line);
            store_in_history(history, line);
        }
        fclose(file);
        fflush(file);
    }
    free(user_home_dir_path);
}

void save_history(History history) {
    FILE *file;
    //opens file to be written to
    char *user_home_dir_path = malloc(sizeof(char) * 256);
    memset(user_home_dir_path, 0, strlen(user_home_dir_path));
    user_home_dir_path = strcat(user_home_dir_path, getenv("HOME"));
    user_home_dir_path = strcat(user_home_dir_path, "/.hist_list");

    file = fopen(user_home_dir_path, "w");
    if (file == NULL) {
        printf("Unable to save history, a file access error occurred.\n");
    } else {
        if (history.is_full == 0) {
            for (int i = history.counter; i < HISTORY_SIZE; i++) {
                fprintf(file, "%s\n", history.commands[i]);
            }
            for (int i = 0; i < history.counter; i++) {
                fprintf(file, "%s\n", history.commands[i]);
            }

        } else {
            for (int i = 0; i < history.counter; i++) {
                fprintf(file, "%s\n", history.commands[i]);
            }
        }
        fclose(file);
        fflush(file);
    }
    free(user_home_dir_path);
}

void load_aliases(char* aliases[ALIAS_MAX][2]) {
    FILE *file;
    // opens the file in a read mode
    char *user_home_dir_path = malloc(sizeof(char) * 256);
    memset(user_home_dir_path, 0, strlen(user_home_dir_path));
    user_home_dir_path = strcat(user_home_dir_path, getenv("HOME"));
    user_home_dir_path = strcat(user_home_dir_path, "/.aliases");
    file = fopen(user_home_dir_path, "r");

    // returns if the file does not exist or is inaccessible
    if (file == NULL) {
        printf("file unaccessible or does not exist");
        return;
    }

    char line[INPUT_SIZE];
    char *alias_name;
    char command[INPUT_SIZE] = "";
    int count_tokens;
    int index;

    index = 0;

    // reads file line by line
    while (fgets(line, sizeof(line), file) != NULL && index < ALIAS_MAX) {
        remove_trailing_new_line(line);
        char **line_tokens = tokenize_input(line);

        alias_name = line_tokens[0];
        command[0] = '\0';
        count_tokens = 0;

        strcat(command, line_tokens[1]);

        // checks how many tokens are in the array
        while (line_tokens[count_tokens + 2] != NULL) {
            count_tokens++;
        }

        // concentrates the command with its arguments
        for (int i = 0; i < count_tokens; i++) {
            strcat(command, " ");
            strcat(command, line_tokens[i + 2]);
        }

        // updates the aliases array
        aliases[index][0] = strdup(alias_name);
        aliases[index][1] = strdup(command);

        index++;
    }

    free(user_home_dir_path);
    fclose(file);
    fflush(file);
}

void save_aliases(char* aliases[ALIAS_MAX][2]) {
    FILE *file;
    char *user_home_dir_path = malloc(sizeof(char) * 256);
    memset(user_home_dir_path, 0, strlen(user_home_dir_path));
    user_home_dir_path = strcat(user_home_dir_path, getenv("HOME"));
    user_home_dir_path = strcat(user_home_dir_path, "/.aliases");

    // opens the file in a write mode
    file = fopen(user_home_dir_path, "w");

    // prints an error if a file is inaccessible
    if (file == NULL) {
        printf("Error: Aliases file access denied.\n");
    } else {
        // writes aliases to the file line by line
        for (int i = 0; i < ALIAS_MAX; i++) {
            // breaks the loop at the first empty alias
            if (aliases[i][0] == NULL) {
                break;
            }
            fprintf(file, "%s %s\n", aliases[i][0], aliases[i][1]);
        }
        fclose(file);
        fflush(file);
    }
    free(user_home_dir_path);
}

void print_tokens(char **tokens) {
    int i = 0;
    while (tokens[i] != NULL) {
        printf("%s\n", tokens[i]);
        i++;
    }
}
