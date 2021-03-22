#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "prompt.h"

int main() {

    // initialise state variables, get current PATH
    // and set cwd to users home directory
    char input[INPUT_SIZE];
    char* tokens[TOKENS_SIZE] = {"0"};
    char* env = getenv("PATH");
    chdir(getenv("HOME"));
    int command;
    int tokens_flag;
    while(1) {
        memset(input, 0, sizeof input);
        print_prompt();
        fflush(stdout);
        read(STDIN_FILENO, input, INPUT_SIZE);

        tokens_flag = get_tokens(tokens, TOKENS_SIZE, input);
        if(tokens_flag == -1) {
            break;
        } else if(tokens_flag == 0) {
            continue;
        }

        // handling of exit
        if(strcmp(tokens[0], "exit") == 0) {
            break;
        }
        
        if ((command = is_command(tokens[0])) != -1) {
            exec_command(command, tokens);
        }
        else
            fork_process(tokens);

    }
    //
    // terminate shell
    setenv("PATH", env, 1);
    printf("%s\n", getenv("PATH")); // remove after testing - DEBUG
    return 0;
}
