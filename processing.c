#include "processing.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include "commands.h"

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
    int command = is_command(tokens[0]);
    printf("%d\n", command);
    if (command != -1){
        exec_command(command, tokens);
        exit(0);
    }
    else if(execvp(tokens[0], tokens) == -1) {
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