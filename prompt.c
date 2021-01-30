//
// Created by Peter on 29/01/2021.
//

#include <stdio.h>
#include "prompt.h"
#include  <string.h>

void warn_user(char* warning){
    printf("ERROR! - %s\n", warning);
}

void print_prompt(char* prompt) {
    printf("%s", prompt);
}

int get_input(char* input, int size){
    // returns 1 if succesfull and 0 if crtl+d was pressed

    return !(fgets(input, size, stdin) == NULL);
}

int input_is_empty(char* input) {
    return strlen(input)-1 == 0;
}

int input_too_large(char* input) {
    // if the string has not exceed the maximum length (SIZE-2 characters to
    // accomodate for \0 and \n) the last character should be the \n
    //
    // returns 1 if input is tool large (more than size-2) and 0 otherwise

    return input[strlen(input)-1] != '\n';
}

void clear_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// maybe redundant
int input_is_valid(char* input){
    // returns 1 if input is valid

    return (!input_too_large(input) &&
            1
            );
}

void remove_trailing_new_line(char* string) {
    // removes last newline character in passed string

    string[strlen(string)-1] = '\0';
}

// maybe redundant
void process_input(char* input) {
    remove_trailing_new_line(input);

}

char* tokenize(char* input) {
    return input;
}

