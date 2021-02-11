//
// Created by Group 26 on 29/01/2021.
//

void warn_user(char* warning);
void print_prompt();
char* get_input(char *input, int size);
int ctrl_d_typed();
int input_is_empty(char *input);
int input_too_large(char *input);
void clear_stdin();
void remove_trailing_new_line(char *string);
void remove_leading_whitespace(char *input);
int check_for_exit(char **tokens);
int store_tokens(char **tokens, int size, char *input);
int get_tokens(char **tokens, int size, char *input);
void print_tokens(char **tokens);
