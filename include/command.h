#ifndef COMMAND_H
#define COMMAND_H

#include <stddef.h>

typedef struct {
    char **argv;
} Command;

void free_command(Command *command);
void free_commands(Command *commands, size_t count);

#endif
