#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prompt.h"

int SIZE = 512;
char DELIMITERS[] = " \t|><&;";
char PROMPT[] = "==>";

int main() {

    // initialise state variables
    char *input = malloc(sizeof(char)*SIZE);
    char *tokens;

    while(1) {
        print_prompt(PROMPT);

        // handles ctrl+d
        if(!get_input(input, SIZE))
            break;

        // check for empty string
        if (input_is_empty(input)) {
            continue;
        }

        // basically a length check - if the string has not exceed the maximum
        // length (SIZE-2 characters to accomodate for \0 and \n) the last
        // character should be the \n
        if (input_too_large(input)) {
            clear_stdin();
            warn_user("Input too large - default max character limit is 512 "
                      "characters (can be changed in config file");
        } else {

            process_input(input);

            tokens = strtok(input, DELIMITERS);

            // exits if exit is 1st word
            if (strcmp(&tokens[0], "exit") == 0 ||
                strcmp(&tokens[0], "EXIT") == 0)

                break;

            // iterate through tokens
            while (tokens) {
                printf("%s\n", tokens);
                tokens = strtok(0, DELIMITERS);
            }
        }
    }

    free(input);
    return 0;
}