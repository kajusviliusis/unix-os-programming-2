#include "pipeline.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int execute_command(const Command *command)
{
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        execvp(command->argv[0], command->argv);
        // exec returns only if it failed, so the child must exit here.
        perror(command->argv[0]);
        _exit(127);
    }

    // wait for the child process to finish.
    int status;
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        return -1;
    }

    return 0;
}
