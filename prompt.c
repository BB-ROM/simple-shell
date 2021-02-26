#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <wait.h>

#include "prompt.h"
//
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

// returns 0 if tokens can't be extracted from input and 1 if successful
int get_tokens(char **tokens, int size, char *input) {

    // leading white space is useless
    remove_leading_whitespace(input);
    // is input empty
    if (strlen(input) - 1 == 0) {
        return 0;
    }

    // strings need \n to check for correct length
    if (input[strlen(input) - 1] != '\n') {
        clear_stdin();
        printf("Input too large - default max character limit is 512 "
               "characters (can be changed in config file)");
        return 0;
    }

    // \n at the end of string could potentially interfere with the commands - has to be stripped
    remove_trailing_new_line(input);
    if (!store_tokens(tokens, size, input)) {
        printf("Too many tokens - 50 is the maximum number of tokens user can input");
        return 0;
    }

    //string is correct
    return 1;
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

int fork_process(char **tokens) {
    pid_t pid;
    pid = fork();

    // unsuccessful
    if (pid < 0) {
        printf("Fork Failed");
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

// helper function to take care of child processes after fork
void process_child_process(char **tokens) {
    // if command is an executable it runs
    if (execvp(tokens[0], tokens) == -1) {
        perror(tokens[0]);
        exit(0);
    }
}

/**
 * Functions for commands
 */

// keywords that map commands to functions
char *commands_calls[] = {
        "getpath",
        "setpath",
        "cd"
};

// array of function pointers :)
int (*commands[])(char **) = {
        &getpath,
        &setpath,
        &cd
};

// returns the index of function or -1 if command is not a function
int is_command(char *command) {
    int num_of_commands = sizeof(commands_calls) / sizeof(char *);
    for (int i = 0; i < num_of_commands; i++) {
        if (strcmp(command, commands_calls[i]) == 0)
            return i;
    }
    return -1;
}

// executes a command with provided arguments
int exec_command(int command, char **tokens) {
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
    } else if (is_path_valid(args[1]))
      setenv("PATH", args[1], 1);
    else {
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
    } else if ( get_number_of_args(args) == 1 &&
                chdir(args[1]) == -1 ) {
        perror("Error");
    } else {
        printf("This command take either no or one argument: a path on the system\n");
    }
    return 1;
}
