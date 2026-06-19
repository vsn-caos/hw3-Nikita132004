#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Программе на стандартный поток ввода задается арифметическое выражение
// в синтаксисе языка python3. Необходимо вычислить это выражение и вывести результат.
// Использовать дополнительные процессы запрещено — нужно использовать exec.

int main(void) {
    size_t cap = 1024;
    char *expr = malloc(cap);
    if (!expr) {
        perror("malloc");
        return 1;
    }

    size_t len = 0;
    int ch;
    while ((ch = getchar()) != EOF) {
        if (ch == '\n') break;
        if (len + 1 >= cap) {
            cap *= 2;
            char *tmp = realloc(expr, cap);
            if (!tmp) {
                free(expr);
                perror("realloc");
                return 1;
            }
            expr = tmp;
        }
        expr[len++] = (char)ch;
    }
    expr[len] = '\0';

    if (len == 0) {
        // Ничего не прочитано
        free(expr);
        return 0;
    }

    size_t code_len = strlen("print(") + len + 1 /* ')' */ + 1;
   char *code = malloc(code_len);
    if (!code) {
        free(expr);
        perror("malloc");
        return 1;
    }
    int written = snprintf(code, code_len, "print(%s)", expr);
    free(expr);
    if (written < 0 || (size_t)written >= code_len) {
        free(code);
        fprintf(stderr, "Expression too long\n");
        return 1;
    }

    execlp("python3", "python3", "-c", code, (char*)NULL);

    perror("execlp");
    free(code);
    return 1;
}
