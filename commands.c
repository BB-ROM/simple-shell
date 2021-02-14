#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "commands.h"

// keywords that map commands to functions
char *commands_calls[] = {
        "getpath",
        "setpath"
};

// array of function pointers :)
int (*commands[])(char **) = {
        &getpath,
        &setpath
};

int is_command(char* command) {
    // returns the index of function or -1 if command is not a function
    int num_of_commands = sizeof(commands_calls) / sizeof(char *);
    for(int i = 0; i < num_of_commands; i++)
    {
        if(strcmp(command, commands_calls[i]) == 0)
            return i;
    }
    return -1;
}

int exec_command(int command, char** tokens) {
        commands[command](tokens);
    return 0;
}

int number_of_args(char** args) {
    int length = 0;
    while ( args[length] != NULL ){
        length++;
    }
    return length-1;
}

int path_is_valid(char* path) {
    return 1;
}

int getpath(char** args) {
    if (number_of_args(args) > 0) {
        printf("This command doesn't take any arguments\n");
    }
    else {
        printf("%s\n", getenv("PATH"));
    }
    return 0;
}

int setpath(char** args) {
    if(number_of_args(args) != 1){
        printf("This command takes exactly one argument\n");
    }
    else if(path_is_valid(args[1])) {
        setenv("PATH", args[1], 1);
    }
    else{
        printf("Provided path has invalid format. Path should be "
               "a string of system paths separated by a comma\n");
    }
    return 0;
}
