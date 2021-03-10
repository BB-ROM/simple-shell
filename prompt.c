#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <wait.h>
#include "prompt.h"

char* aliases[ALIAS_MAX][2];

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

// stores individual tokens from input to provided
// tokens array - max is 50
// returns 0 if unsuccessful
int store_tokens(char **tokens, int size, char *input) {
    char DELIMITERS[] = " \t|><&;";
    char *token = strtok(input, DELIMITERS);
    int i = 0;
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

// returns 0 if tokens can't be extracted from input and 1 if successful
int get_tokens(char **tokens, int size, char *input) {
    // checking for input too long or EOF
    if (input[strlen(input) - 1] != '\n') {
        if (strlen(input) > INPUT_SIZE - 1) {
            clear_stdin();
            printf("Input too large - default max character limit is 512 "
               "characters (can be changed in config file)\n");
            return 0;
        }
        else {
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
        perror(tokens[0]);
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

// returns the index of function or -1 if command is not a function
int is_command(char *command) {
    char *commands_calls[] = {
            "getpath",
            "setpath",
            "cd",
            "alias"
    };
    int num_of_commands = sizeof(commands_calls) / sizeof(char *);
    for (int i = 0; i < num_of_commands; i++) {
        if (strcmp(command, commands_calls[i]) == 0)
            return i;
    }
    return -1;
}

// executes a command with provided arguments
int exec_command(int command, char **tokens) {
    int (*commands[])(char **) = {
            &getpath,
            &setpath,
            &cd,
            &alias
    };
    commands[command](tokens);
    return 0;
}

// return how many arguments are in the tokens array
int get_number_of_args(char **tokens) {
    int length = 0;
    while (tokens[length] != NULL) {
        length++;
    }
    return length - 1;
}

// helper function
int is_path_valid(char* path) {
    char cwd[256];
    getcwd(cwd, 255);
    if(chdir(path) == 0) {
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
    } else if ( get_number_of_args(args) == 1 ){
        if (chdir(args[1]) == -1 ) {
            printf("Error, this path doesn't exist.\n");
        }
    } else {
        printf("This command take either no or one argument: a path on the system\n");
    }
    return 1;
}

int alias(char **args) {
    int count_null = 0;
    char *alias_name = args[1];
    char command[INPUT_SIZE] = "";
    int count_tokens = 0;
    bool exists = false;

    //counts number of null elements of alias array
    for (int i = 0; i < ALIAS_MAX; i++){
        if(aliases[i][0] == NULL){
            count_null = count_null + 1;
        }
    }
    
    //checks to see if alias already entered and shows what aliases exists or says there are none
    if (args[1] == NULL){
        if(count_null == ALIAS_MAX) {
            printf("There are no aliases to list\n");
        } else {
            for (int i = 0; (i < (ALIAS_MAX-count_null)); i++){
                printf("%s\t%s \n", aliases[i][0], aliases[i][1]);
            }
        }
        return 0;
    } else {
        //will prompt for parameters if there has been an incorrect input
        if(args[2] == NULL){
            printf("Incorrect parameters entered for alias. Specify <""second command"">\n");
            return 0;
        }
    }

    alias_name = args[1];
    count_tokens = 0;

    strcat(command, args[2]);

    // checks how many tokens are in the array
    while (args[count_tokens+3] != NULL) {
        count_tokens++;
    }

    //concatenates all the tokens excluding the alias and command to be created 
    for (int i = 0; i < count_tokens; i++) {
        strcat(command, " ");
        strcat(command, args[i+3]);
    }

    //appends a null character to the end of the string.allows aliased commands to function like any other command in use
    strcat(command, "\0");
    
    //ensures no duplication of existing aliases by overwriting, warns user this has been done.
    for (int i = 0; i < (ALIAS_MAX-count_null); i++) {
        if(strcmp(alias_name, aliases[i][0]) == 0) {
            printf("Alias already exists. Overwriting.\n");
            aliases[i][1] = strdup(command);
            exists = true;
        }
    }

    //if there are too many aliases in existence, warns user no more can be added
    if((count_null == 0) && (exists == false)) {
        printf("No more aliases can be added \n");
    }
    else {
        //adds the alias to the Array if it is not already there
        if ((exists == false) && (count_null != 0)) {
            aliases[ALIAS_MAX-count_null][0] = strdup(alias_name);
            aliases[ALIAS_MAX-count_null][1] = strdup(command);
        }
    }

    return 0;
}

/*
int unalias(char* tokens[TOKENS_SIZE], char* alias[ALIAS_MAX][2]){
    int i; int index;
    char* command;
    bool found;

    // checks inputs
    if (tokens[1] == NULL){
        printf("Error: No alias provided. \n");
        return;
    }

    if(tokens[2]!=NULL){
        printf("Error: Too many values provided. \n");
        return;
    }

    command = tokens[1];
    found = false;

    //searches through the alias array for the value to be removed
    for (i = 0; i < ALIAS_MAX; i++){
        if (alias[i][0] != NULL){
            if(strcmp(command, alias[i][0]) == true){
                alias[i][0] = NULL;
                alias[i][1] = NULL;
                index = i;
                found = true;
            }
        }

    }

    //if the above search returns false, prints error to terminal, otherwise removes alias
    if (found == false){
        printf("Error: The alias does not exist. \n");
        return;
    }
    else{
        while(index<(ALIAS_MAX-1)){
            alias[index][0] = alias[index+1][0];
            alias[index][1] = alias[index+1][1];
            index++;
        }
        alias[ALIAS_MAX-1][0] = NULL;
        alias[ALIAS_MAX-1][1] = NULL;
    }
    return 0;
}

*/