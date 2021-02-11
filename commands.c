#include <string.h>
#include <stdio.h>

int getpath(char** args) {
    return 1;
}

int setpath(char** args) {
    return 1;
}

char *commands_calls[] = {
        "getpath",
        "setpath"
};

int (*commands[])(char **) = {
        &getpath,
        &setpath
};

int num_of_commands() {
    return sizeof(commands_calls) / sizeof(char *);
}

int is_command(char* command) {
    for(int i = 0; i < num_of_commands(); i++)
    {
        if(strcmp(command, commands_calls[i]) == 0)
            return i;
    }
    return 0;
}

int exec_command(int command, char** tokens) {
//        commands[command](tokens);
    for(int i = 0 ; i<49; i++){
        printf("%s\n", tokens[i]);
    }

    return 0;
}



