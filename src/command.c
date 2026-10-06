#include "command.h"

#include <stdlib.h>

void free_command(Command *command)
{
    if (command->argv == NULL) {
        return;
    }

    // free each argument before freeing the argv array itself.
    for (size_t i = 0; command->argv[i] != NULL; i++) {
        free(command->argv[i]);
    }

    free(command->argv);
    command->argv = NULL;
}

void free_commands(Command *commands, size_t count)
{
    for (size_t i = 0; i < count; i++) {
        free_command(&commands[i]);
    }
    free(commands);
}
