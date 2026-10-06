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

int execute_two_command_pipeline(const Command *commands)
{
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        return -1;
    }

    pid_t first_pid = fork();
    if (first_pid == -1) {
        perror("fork");
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return -1;
    }

    if (first_pid == 0) {
        // send the first command output into the pipe.
        if (dup2(pipe_fd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            _exit(1);
        }
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        execvp(commands[0].argv[0], commands[0].argv);
        perror(commands[0].argv[0]);
        _exit(127);
    }

    pid_t second_pid = fork();
    if (second_pid == -1) {
        perror("fork");
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        waitpid(first_pid, NULL, 0);
        return -1;
    }

    if (second_pid == 0) {
        // read the second command input from the pipe.
        if (dup2(pipe_fd[0], STDIN_FILENO) == -1) {
            perror("dup2");
            _exit(1);
        }
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        execvp(commands[1].argv[0], commands[1].argv);
        perror(commands[1].argv[0]);
        _exit(127);
    }

    // the parent must close both ends so the reader can receive EOF.
    close(pipe_fd[0]);
    close(pipe_fd[1]);

    int result = 0;
    if (waitpid(first_pid, NULL, 0) == -1) {
        perror("waitpid");
        result = -1;
    }
    if (waitpid(second_pid, NULL, 0) == -1) {
        perror("waitpid");
        result = -1;
    }
    return result;
}
