int fork_process(char **tokens);
void process_child_process(char **tokens);
char* get_environment();
void set_environment(char* environment);
char* get_home_dir();
void set_cwd(char* dir);