#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "prompt.h"

int main() {

    // initialise state variables, get current PATH
    char input[INPUT_SIZE];
    char* tokens[TOKENS_SIZE] = {"0"};
    int command;
    int tokens_flag;
    // save path and set cwd to users home directory
    char* env = getenv("PATH");
    chdir(getenv("HOME"));

    //load history and aliases
    load_history();
    load_aliases();

    while(1) {
        // empty stdin, clear input and print the prompt
        memset(input, 0, sizeof input);
        print_prompt();
        fflush(stdout);
        read(STDIN_FILENO, input, INPUT_SIZE);

        // tokenize input
        // history and aliases are handled in this section
        tokens_flag = get_tokens(tokens, TOKENS_SIZE, input);
        // if get_tokens returned -1 quit shell
        if(tokens_flag == -1) {
            save_history();
            save_aliases();
            break;
        } else if(tokens_flag == 0) {
            continue;
        }

        // handling of exit
        if(strcmp(tokens[0], "exit") == 0) {
            // save history and aliases on exit
            save_history();
            save_aliases();
            break;
        }

        // execute the command or fork and execute the program
        if ((command = is_command(tokens[0])) != -1) {
            exec_command(command, tokens);
        }
        else
            fork_process(tokens);

    }

    // restore path
    setenv("PATH", env, 1);
    printf("%s\n", getenv("PATH")); // remove after testing - DEBUG
    return 0;
}
