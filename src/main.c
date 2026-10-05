#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

        /* echo input until parsing and execution are implemented. */
        fputs(line, stdout);
    }

    free(line);
    return 0;
}
