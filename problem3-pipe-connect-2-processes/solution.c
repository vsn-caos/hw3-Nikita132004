#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// Программе передаются два аргумента: CMD1 и CMD2.
// Необходимо запустить два процесса, выполняющих эти команды,
// и перенаправить stdout CMD1 в stdin CMD2 (CMD1 | CMD2).
// Родительский процесс должен завершаться последним.

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <CMD1> <CMD2>\n", argv[0]);
        return 1;
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid1 = fork();
    if (pid1 == -1) {
        perror("fork1");
        return 1;
    }
    if (pid1 == 0) {
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("dup2 stdout");
            _exit(1);
        }
        close(pipefd[0]);
        close(pipefd[1]);
        execl("/bin/sh", "sh", "-c", argv[1], (char*)NULL);
        perror("execl CMD1");
        _exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 == -1) {
        perror("fork2");
        return 1;
    }
    if (pid2 == 0) {
        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            perror("dup2 stdin");
            _exit(1);
        }
        close(pipefd[0]);
        close(pipefd[1]);
        // выполняем CMD2 через оболочку
        execl("/bin/sh", "sh", "-c", argv[2], (char*)NULL);
        perror("execl CMD2");
        _exit(1);
    }

    close(pipefd[0]);
    close(pipefd[1]);

    int status;
    waitpid(pid1, &status, 0);
    waitpid(pid2, &status, 0);

    return 0;
}
