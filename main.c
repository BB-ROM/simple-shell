#include <stdio.h>
#include "prompt.h"
#include "processing.h"

#define INPUT_SIZE 512
#define TOKENS_SIZE 50

int main() {

    // initialise state variables, get current PATH and set cwd to
    // users home directory
    char input[INPUT_SIZE];
    char* tokens[TOKENS_SIZE] = {"0"};
    char* env = get_environment();
    set_cwd(get_home_dir());

    while(1) {
        print_prompt();
        get_input(input, INPUT_SIZE);

        // exits for ctrl+d
        if(ctrl_d_typed()) {
            printf("\n");
            break;
        }

        // get tokens from the input
        if(!get_tokens(tokens, TOKENS_SIZE, input))
            continue;

        // handling of exit
        if (check_for_exit(tokens))
            break;

        // process command
        fork_process(tokens);
    }

    // terminate shell
    set_environment(env);
    printf("%s\n", get_environment());
    return 0;
}
