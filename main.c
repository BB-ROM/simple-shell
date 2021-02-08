#include "prompt.h"

int main() {

    // initialise state variables
    char input[512];
    char* tokens[50] = {"0"};

    while(1) {
        print_prompt();
        get_input(input);

        // exits for ctrl+d
        if(ctrl_d_typed())
            break;

        // handling of empty strings
        remove_leading_whitespace(input);
        if (input_is_empty(input)) {
            continue;
        } 

        // prompts the user if input is too big char-wise - max input length configurable
        // in the config file
        if (input_too_large(input)) {
            clear_stdin();
            warn_user("Input too large - default max character limit is 512 "
                      "characters (can be changed in config file)");
            continue;
        }

        // preprocessing done - actual starting point

        remove_trailing_new_line(input);
        if(!get_tokens(tokens, input)){
            warn_user("Too many tokens - 50 is the maximum number of tokens user can input");
            break;
        }

        print_tokens(tokens); // for testing

        // handling of exit
        if (check_for_exit((char **)tokens))
            break;

        //fork_process(tokens);

        // iterate through tokens
        //process_tokens(tokens);

    }

    return 0;
}
