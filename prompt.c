//
// Created by Group 26 on 29/01/2021.
//
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <wait.h>
#include <stdlib.h>
#include "prompt.h"


char DELIMITERS[] = " \t|><&;";
char PROMPT[] = "==>";

void warn_user(char* warning){
    // print an Error with warning as a message
    printf("ERROR! - %s\n", warning);
}

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
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void remove_trailing_new_line(char *string) {
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

int check_for_exit(char **tokens) {
    // returns 1 if exit was "requested"
   return (strcmp(tokens[0], "exit") == 0 ||
           strcmp(tokens[0], "EXIT") == 0);
}

int get_tokens(char **tokens,int size, char *input){
    // returns 0 if tokens can't be extracted from input and 1 if successful

    remove_leading_whitespace(input);
    if (input_is_empty(input)) {
        return 0;
    }

    if (input_too_large(input)) {
        clear_stdin();
        warn_user("Input too large - default max character limit is 512 "
                  "characters (can be changed in config file)");
        return 0;
    }

    remove_trailing_new_line(input);
    if(!store_tokens(tokens, size, input)){
        warn_user("Too many tokens - 50 is the maximum number of tokens user can input");
        return 0;
    }

    return 1;
}

void print_tokens(char **tokens){
    // prints tokens on terminal
  printf("Tokens: "); 
    int i = 0;
    while(tokens[i]){
        printf("{%s}\n", tokens[i]);
        i++;
    }
}

int store_tokens(char **tokens, int size, char *input) {
    // stores individual tokens from input to tokens - returns 0 if unsuccessful
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

int fork_process(char **tokens) {
    pid_t pid;
    pid = fork();

    // unsuccessful
    if(pid < 0){
        printf("Fork Failed");
        return 1;
    }
    // child process
    else if (pid == 0){
        process_child_process(tokens);
    }
    // parent process
    else {
       wait(NULL);
    }
    return 0;
}

void process_child_process(char **tokens) {
    if(execvp(tokens[0], tokens) == -1) {
        perror("Error");
        printf("unable to execute %s command\n", tokens[0]);
        exit(0);
    }
}

char* get_environment() {
    return getenv("PATH");
}

void set_environment(char* environment) {
    setenv("PATH", environment, 1);
}

char* get_home_dir() {
    return getenv("HOME");
}

void set_cwd(char* dir) {
    if(chdir(dir) == -1){
        perror("Error");
    }
}