#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "parser.h"
#include "pipeline.h"

static int is_exit_command(const char *line)
{
    while (isspace((unsigned char)*line)) {
        line++;
    }

    if (strncmp(line, "exit", 4) != 0) {
        return 0;
    }

    line += 4;
    while (isspace((unsigned char)*line)) {
        line++;
    }

    return *line == '\0';
}

int main(void)
{
    char *line = NULL;
    size_t capacity = 0;

    while (1) {
        fputs("kvshell> ", stdout);
        fflush(stdout);

        if (getline(&line, &capacity, stdin) == -1) {
            if (ferror(stdin)) {
                perror("getline");
                free(line);
                return EXIT_FAILURE;
            }

            putchar('\n');
            break;
        }

        if (is_exit_command(line)) {
            break;
        }

        Command command;
        if (parse_command(line, &command) == -1) {
            perror("parse_command");
            free(line);
            return EXIT_FAILURE;
        }

        if (command.argv != NULL) {
            execute_command(&command);
        }

        free_command(&command);
    }

    free(line);
    return 0;
}
