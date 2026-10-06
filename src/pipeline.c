#include "pipeline.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static void close_pipes(int pipes[][2], size_t pipe_count)
{
    for (size_t i = 0; i < pipe_count; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
}

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

int execute_pipeline(const Command *commands, size_t count)
{
    if (count < 2) {
        return -1;
    }

    size_t pipe_count = count - 1;
    int pipes[pipe_count][2];
    pid_t pids[count];

    for (size_t i = 0; i < pipe_count; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            close_pipes(pipes, i);
            return -1;
        }
    }

    for (size_t i = 0; i < count; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork");
            close_pipes(pipes, pipe_count);
            for (size_t j = 0; j < i; j++) {
                waitpid(pids[j], NULL, 0);
            }
            return -1;
        }

        if (pids[i] == 0) {
            // middle commands read from the previous pipe and write to the next.
            if (i > 0 && dup2(pipes[i - 1][0], STDIN_FILENO) == -1) {
                perror("dup2");
                _exit(1);
            }
            if (i < pipe_count && dup2(pipes[i][1], STDOUT_FILENO) == -1) {
                perror("dup2");
                _exit(1);
            }

            // close unused pipe ends so readers can get EOF.
            close_pipes(pipes, pipe_count);
            execvp(commands[i].argv[0], commands[i].argv);
            perror(commands[i].argv[0]);
            _exit(127);
        }
    }

    close_pipes(pipes, pipe_count);

    int result = 0;
    for (size_t i = 0; i < count; i++) {
        if (waitpid(pids[i], NULL, 0) == -1) {
            perror("waitpid");
            result = -1;
        }
    }
    return result;
}
