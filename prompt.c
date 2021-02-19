//
// Created by Group 26 on 29/01/2021.
//
#include <unistd.h>
#include <stdio.h>
#include "string.h"
#include "prompt.h"


char DELIMITERS[] = " \t|><&;";
char PROMPT[] = "==> ";

void print_prompt() {
    char arr[256];
    getcwd(arr, 255);
    printf("[%s]%s", arr, PROMPT);
}

char* get_input(char *input, int size){
    // stores input from stdin to input
    return fgets(input, size, stdin);
}

int ctrl_d_typed() {
    // returns 1 if ctrl+d was pressed
    return feof(stdin) != 0;
}

int input_is_empty(char *input) {
    // checks for empty input
    return strlen(input)-1 == 0;
}

int input_too_large(char *input) {
    // if the string has not exceed the maximum length (SIZE-2 characters to
    // accommodate for \0 and \n) the last character should be the \n
    //
    // returns 1 if input is tool large (more than size-2) and 0 otherwise

    return input[strlen(input)-1] != '\n';
}

void clear_stdin() {
    // useful for when input is too large
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void remove_trailing_new_line(char *string) {
    // removes last newline character in a string
    char *new = strchr( string, '\n' );
    if (new)
        *new = 0;
}

void remove_leading_whitespace(char *input) {
    // ensures string such as "  " can be typed without segfault
    int index, s;
    index = 0;
    while (input[index] == ' ')
        index++;

    if (index != 0) {
        s = 0;
        while(input[s + index] != '\0') {
            input[s] = input[s + index];
            s++;
        }
        input[s] = '\0';
    }
}

int check_for_exit(char **tokens) {
    // returns 1 if exit was "requested"
   return (strcmp(tokens[0], "exit") == 0 ||
           strcmp(tokens[0], "EXIT") == 0);
}

int get_tokens(char **tokens,int size, char *input){
    // returns 0 if tokens can't be extracted from input and 1 if successful

    // leading white space is ugly
    remove_leading_whitespace(input);
    if (input_is_empty(input)) {
        return 0;
    }

    // strings need \n to check for correct length
    if (input_too_large(input)) {
        clear_stdin();
        printf("Input too large - default max character limit is 512 "
                  "characters (can be changed in config file)");
        return 0;
    }

    // \n at the end of string could potentially interfere with the commands - has to be stripped
    remove_trailing_new_line(input);
    if(!store_tokens(tokens, size, input)){
        printf("Too many tokens - 50 is the maximum number of tokens user can input");
        return 0;
    }

    //string is correct
    return 1;
}

int store_tokens(char **tokens, int size, char *input) {
    // stores individual tokens from input to provided
    // tokens array - max is 50
    // returns 0 if unsuccessful
    char* token = strtok(input, DELIMITERS);
    int i = 0;
    while(token) {
        tokens[i] = token;
        i++;
        token = strtok(NULL, DELIMITERS);
        // too many tokens - segfault
        if (i >= size-1)
            return 0;
    }
    // last token should be null to work with exec()
    tokens[i] = NULL;
    return 1;
}

void print_tokens(char **tokens){
    // prints tokens on terminal - DEBUG
  printf("Tokens: "); 
    int i = 0;
    while(tokens[i]){
        printf("{%s}\n", tokens[i]);
        i++;
    }
}
