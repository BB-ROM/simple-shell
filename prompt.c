#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <wait.h>

#include <string.h>
#include "prompt.h"


char DELIMITERS[] = " \t|><&;";

void print_prompt() {
    char arr[256];
    getcwd(arr, 255);
    printf("[%s]%s", arr, "==> ");
}

char *get_input(char *input, int size) {
    // stores input from stdin to input
    return fgets(input, size, stdin);
}

void clear_stdin() {
    // useful for when input is too large
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void remove_trailing_new_line(char *string) {
    // removes last newline character in a string
    char *new = strchr(string, '\n');
    if (new)
        *new = 0;
}

void remove_leading_whitespace(char *input) {
    // ensures string such as "  " can be typed without segfault
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

int get_tokens(char **tokens, int size, char *input) {
    // returns 0 if tokens can't be extracted from input and 1 if successful

    // leading white space is ugly
    remove_leading_whitespace(input);
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

int store_tokens(char **tokens, int size, char *input) {
    // stores individual tokens from input to provided
    // tokens array - max is 50
    // returns 0 if unsuccessful
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

/**
 * Methods for commands
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

int is_command(char *command) {
    // returns the index of function or -1 if command is not a function
    int num_of_commands = sizeof(commands_calls) / sizeof(char *);
    for (int i = 0; i < num_of_commands; i++) {
        if (strcmp(command, commands_calls[i]) == 0)
            return i;
    }
    return -1;
}

int exec_command(int command, char **tokens) {
    commands[command](tokens);
    return 0;
}

int number_of_args(char **args) {
    int length = 0;
    while (args[length] != NULL) {
        length++;
    }
    return length - 1;
}

int getpath(char **args) {
    if (number_of_args(args) > 0) {
        printf("This command doesn't take any arguments\n");
    } else {
        printf("%s\n", getenv("PATH"));
    }
    return 0;
}

int setpath(char **args) {
    if (number_of_args(args) != 1) {
        printf("This command takes exactly one argument\n");
    } else if (chdir(args[1]) == 0)
      setenv("PATH", args[1], 1);
     else {
        printf("Provided path has invalid format. Path should be "
               "a string of system paths separated by a comma\n");
    }
    return 0;
}

int cd(char **args) {
    // get_home_dir and set_cwd are imports
    // should we get rid of them, merge commands.c and processing.c or keep it?
    if (number_of_args(args) == 0) {
        chdir(get_home_dir());
    } else if (number_of_args(args) == 1) {
        set_cwd(args[1]);
    } else {
        printf("This command take either no or one argument: a path on the system\n");
    }

    return 1;
}

/**
 * Methods for processing
 */
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

void process_child_process(char **tokens) {
    // if command is an executable it runs
    if (execvp(tokens[0], tokens) == -1) {
        perror(tokens[0]);
        exit(0);
    }
}

char *get_environment() {
    return getenv("PATH");
}

void set_environment(char *environment) {
    setenv("PATH", environment, 1);
}

char *get_home_dir() {
    return getenv("HOME");
}

void set_cwd(char *dir) {
    // current working directory
    if (chdir(dir) == -1) {
        perror("Error");
    }
}