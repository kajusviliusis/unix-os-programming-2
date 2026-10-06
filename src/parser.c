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

// returns 0 on success, 1 for invalid syntax, or -1 on allocation failure.
int parse_pipeline(const char *input, Command **commands, size_t *count)
{
    *commands = NULL;
    *count = 0;

    size_t parts = 1;
    for (const char *cursor = input; *cursor != '\0'; cursor++) {
        if (*cursor == '|') {
            parts++;
        }
    }

    Command *parsed = calloc(parts, sizeof(*parsed));
    if (parsed == NULL) {
        return -1;
    }

    char *copy = malloc(strlen(input) + 1);
    if (copy == NULL) {
        free(parsed);
        return -1;
    }
    strcpy(copy, input);

    // split at each pipe, then parse the text between pipes as a command.
    char *segment = copy;
    for (size_t i = 0; i < parts; i++) {
        char *pipe_pos = strchr(segment, '|');
        if (pipe_pos != NULL) {
            *pipe_pos = '\0';
        }

        if (parse_command(segment, &parsed[i]) == -1) {
            free(copy);
            free_commands(parsed, parts);
            return -1;
        }
        if (parts > 1 && parsed[i].argv == NULL) {
            free(copy);
            free_commands(parsed, parts);
            return 1;
        }

        if (pipe_pos != NULL) {
            segment = pipe_pos + 1;
        }
    }

    free(copy);
    *commands = parsed;
    *count = parts;
    return 0;
}
