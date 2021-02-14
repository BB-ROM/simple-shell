#include "processing.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>

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
    // if command is a executable it runs
    if(execvp(tokens[0], tokens) == -1) {
        perror(tokens[0]);
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
    // current working directory
    if(chdir(dir) == -1){
        perror("Error");
    }
}