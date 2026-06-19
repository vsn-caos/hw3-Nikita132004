#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// Программе передается аргумент — целое число N > 0.
// Необходимо создать N-1 дополнительных процессов таким образом,
// чтобы у каждого процесса было не более одного дочернего процесса.
// Каждый из процессов должен вывести ровно одно число так,
// чтобы на выходе получилась строка: 1 2 3 4 ... N
// Между числами — ровно один пробел, строка завершается символом '\n'.

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <N>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        fprintf(stderr, "N must be > 0\n");
        return 1;
    }
    int i = 1;
    setvbuf(stdout, NULL, _IONBF, 0);
    while (1) {
        if (i == 1) {
            printf("%d", i);
        } else {
            printf(" %d", i);
        }
        fflush(stdout);

        if (i == n) {
            printf("\n");
            fflush(stdout);
            break;
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            return 1;
        }

        if (pid > 0) {
            wait(NULL);
            break;
        } else {
            i++;
            continue;
        }
    }

    return 0;
}
