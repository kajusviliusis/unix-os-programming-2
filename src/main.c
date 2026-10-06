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

        Command *commands;
        size_t count;
        int result = parse_pipeline(line, &commands, &count);
        if (result == -1) {
            perror("parse_pipeline");
            free(line);
            return EXIT_FAILURE;
        }
        if (result == 1) {
            fputs("Invalid pipeline\n", stderr);
            continue;
        }

        if (count == 1 && commands[0].argv != NULL) {
            execute_command(&commands[0]);
        } else if (count == 2) {
            execute_two_command_pipeline(commands);
        } else if (count > 2) {
            for (size_t i = 0; i < count; i++) {
                printf("Command %zu:\n", i);
                for (size_t j = 0; commands[i].argv[j] != NULL; j++) {
                    printf("  %s\n", commands[i].argv[j]);
                }
            }
        }

        free_commands(commands, count);
    }

    free(line);
    return 0;
}
