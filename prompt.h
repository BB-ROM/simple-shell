//
// Created by Peter on 29/01/2021.
//

#ifndef CS210_SIMPLE_SHELL_PROMPT_H
#define CS210_SIMPLE_SHELL_PROMPT_H

#endif //CS210_SIMPLE_SHELL_PROMPT_H

char* new_input();
void warn_user(char* warning);
void print_prompt();
char* get_input(char* input);
int ctrl_d_typed();
int input_is_empty(char* input);
int input_too_large(char* input);
void clear_stdin();
void remove_trailing_new_line(char* string);
void remove_leading_whitespace(char** input);
int check_for_exit(char* tokens);
void process_tokens(char* tokens);
void print_tokens(char* tokens);
char* get_tokens(char* input);
void remove_trailing_whitespace(char* input);
char* prepare_input(char* input);
int input_is_valid(char* input);
