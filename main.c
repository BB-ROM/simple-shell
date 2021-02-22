#include <stdio.h>
#include <string.h>
#include "prompt.h"

#define INPUT_SIZE 512
#define TOKENS_SIZE 50

int main() {

    // initialise state variables, get current PATH
    // and set cwd to users home directory
    char input[INPUT_SIZE];
    char* tokens[TOKENS_SIZE] = {"0"};
    char* env = get_environment();
    set_cwd(get_home_dir());
    int command;

    while(1) {
        print_prompt();
        get_input(input, INPUT_SIZE);

        // exits for ctrl+d
        if(feof(stdin) != 0){
            printf("\n");
            break;
        }

        // get tokens from the input
        if(!get_tokens(tokens, TOKENS_SIZE, input))
            continue;

        // handling of exit
        if(strcmp(tokens[0], "exit") == 0)
            break;

        // process command
        if ((command = is_command(tokens[0])) != -1) {
            exec_command(command, tokens);
        }
        else
            fork_process(tokens);

    }

    // terminate shell
    set_environment(env);
    printf("%s\n", get_environment()); // remove after testing - DEBUG
    return 0;
}
