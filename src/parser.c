#include "parser.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int parse_command(const char *input, Command *command)
{
    command->argv = NULL;

    // count arguments so we know how much memory to allocate.
    size_t argc = 0;
    const char *cursor = input;
    while (*cursor != '\0') {
        while (isspace((unsigned char)*cursor)) {
            cursor++;
        }
        if (*cursor == '\0') {
            break;
        }

        argc++;
        while (*cursor != '\0' && !isspace((unsigned char)*cursor)) {
            cursor++;
        }
    }

    if (argc == 0) {
        return 0;
    }

    // allocate one extra slot for the final NULL required by exec.
    command->argv = calloc(argc + 1, sizeof(*command->argv));
    if (command->argv == NULL) {
        return -1;
    }

    // copy each argument into argv.
    cursor = input;
    for (size_t i = 0; i < argc; i++) {
        while (isspace((unsigned char)*cursor)) {
            cursor++;
        }

        const char *start = cursor;
        while (*cursor != '\0' && !isspace((unsigned char)*cursor)) {
            cursor++;
        }

        size_t length = (size_t)(cursor - start);
        command->argv[i] = malloc(length + 1);
        if (command->argv[i] == NULL) {
            free_command(command);
            return -1;
        }

        memcpy(command->argv[i], start, length);
        command->argv[i][length] = '\0';
    }

    return 0;
}
