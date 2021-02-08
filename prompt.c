//
// Created by Peter on 29/01/2021.
//
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prompt.h"
#include <ctype.h>

int SIZE = 512;
char DELIMITERS[] = " \t|><&;";
char PROMPT[] = "==>";

void warn_user(char* warning){
    // print an Error with warning as a message
    printf("ERROR! - %s\n", warning);
}

void print_prompt() {
    printf("%s", PROMPT);
}

char* get_input(char* input){
    // stores typed commands from stdin to input
    return fgets(input, SIZE, stdin);
}

int ctrl_d_typed() {
    //  returns 1 if ctrl+d was pressed
    return feof(stdin) != 0;
}

int input_is_empty(char* input) {
    // checks for empty input
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


void remove_trailing_new_line(char* string) {
    // removes last newline character in passed string
    char *new = strchr( string, '\n' );
    if (new)
        *new = 0;
}

void remove_leading_whitespace(char *input) {
    // here so that string such as "  " can be typed without segfault
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

int check_for_exit(char** tokens) {
    // returns 1 if exit was "requested"
   return (strcmp(tokens[0], "exit") == 0 ||
           strcmp(tokens[0], "EXIT") == 0);
}

void print_tokens(char* tokens[]){
    // prints tokens on terminal
    int i = 0;
    while(tokens[i]){
        printf("%s\n", tokens[i]);
        i++;
    }
}

int get_tokens(char *tokens[], char* input) {
    // fills supplied array (tokens) with tokens - returns 0 if unsuccessful
    char* token = strtok(input, DELIMITERS);
    int i = 0;
    while(token) {
        tokens[i] = token;
        i++;
        token = strtok(NULL, DELIMITERS);
        // too many tokens - segfault
        if (i >= 50)
            return 0;
    }
    return 1;
}

//int fork_process(char* input){
//    pid_t pid;
//    pid = fork();
//
//    if(pid < 0){
//        printf(stderr, "Fork Failed");
//        return 1;
//    }
//    else if (pid == 0){
//      //char* programname = input;
//      char* programname = get_token(input);
//      printf("%s\n", programname);
//      char* argv[] = {programname, "-lh", "/home", NULL};
//      execvp(programname, argv);
//    }
//    else{
//        wait(NULL);
//        printf("Child Complete");
//    }
//    return 0;
//}


