#ifndef COMMAND_H
#define COMMAND_H

typedef struct {
    char **argv;
} Command;

void free_command(Command *command);

#endif
