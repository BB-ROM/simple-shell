//
// Created by Peter on 29/01/2021.
//

#ifndef CS210_SIMPLE_SHELL_PROMPT_H
#define CS210_SIMPLE_SHELL_PROMPT_H

#endif //CS210_SIMPLE_SHELL_PROMPT_H

char* new_input();
void warn_user(char* warning);
void print_prompt(char* prompt);
int get_input(char* input, int size);
int input_is_empty(char* input);
int input_too_large(char* input);
void clear_stdin();
int input_is_valid(char* input);
void remove_trailing_new_line(char* string);
void process_input(char* input);
char* tokenize(char* input);
