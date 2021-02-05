//TODO:
// - ctrl+d behaves fine with no input before it but when you start typing and
//   then decide to press ctrl+d you have to do it twice


#include <stdlib.h>
#include "prompt.h"

int main() {

    // initialise state variables
    char *input = new_input();
    char *tokens;

    while(1) {
        print_prompt();
        get_input(input);

        // exits for ctrl+d
        if(ctrl_d_typed())
            break;

        // handling of empty strings
        remove_leading_whitespace(&input);
        if (input_is_empty(input)) {
            continue;
        }

        // prompts the user if input is too big - max input length configurable
        // in the config file
        if (input_too_large(input)) {
            clear_stdin();
            warn_user("Input too large - default max character limit is 512 "
                      "characters (can be changed in config file)");
            continue;
        }

        // preprocessing done - actual starting point
        tokens = get_tokens(input);
        fork_process(tokens);

        // handling of exit
        if (check_for_exit(tokens))
            break;

        // iterate through tokens
        process_tokens(tokens);

    }



    free(input);
    return 0;
}
