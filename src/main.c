#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>

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

        /* echo input until parsing and execution are implemented. */
        fputs(line, stdout);
    }

    free(line);
    return 0;
}
