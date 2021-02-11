#include "prompt.h"

#define INPUT_SIZE 512
#define TOKENS_SIZE 50

int main() {

    // initialise state variables
    char input[INPUT_SIZE];
    char* tokens[TOKENS_SIZE] = {"0"};

    while(1) {
        print_prompt();
        get_input(input, INPUT_SIZE);

        // exits for ctrl+d
        if(ctrl_d_typed())
            break;

        // get tokens from the input
        if(!get_tokens(tokens, TOKENS_SIZE, input))
            continue;

        // handling of exit
        if (check_for_exit(tokens))
            break;

        // process command
        fork_process(tokens);
    }

    return 0;
}
