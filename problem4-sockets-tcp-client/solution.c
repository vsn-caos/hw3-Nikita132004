#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Программе передаются два аргумента:
//   argv[1] — IPv4-адрес сервера в десятичной записи (например, "127.0.0.1")
//   argv[2] — номер порта
//
// Программа должна:
//   1. Установить TCP-соединение с указанным сервером.
//   2. В цикле читать со stdin целые знаковые числа в текстовом формате.
//   3. Отправлять каждое число на сервер в бинарном виде (int32, Little Endian).
//   4. Получать от сервера int32 LE в ответ и выводить его в stdout в текстовом виде.
//   5. Если сервер закрыл соединение — завершиться с кодом возврата 0.

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ipv4_addr> <port>\n", argv[0]);
        return 1;
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);
    if (port <= 0 || port > 65535) {
        fprintf(stderr, "Invalid port: %s\n", argv[2]);
        return 1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons((unsigned short)port);
    if (inet_pton(AF_INET, ip, &servaddr.sin_addr) != 1) {
        fprintf(stderr, "Invalid IPv4 address: %s\n", ip);
        close(sockfd);
        return 1;
    }

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect");
        close(sockfd);
        return 1;
    }

    int x;
    unsigned char out[4];
    unsigned char inbuf[4];

    while (1) {
        if (scanf("%d", &x) != 1) {
            break;
        }

        out[0] = (unsigned char)(x & 0xFF);
        out[1] = (unsigned char)((x >> 8) & 0xFF);
        out[2] = (unsigned char)((x >> 16) & 0xFF);
        out[3] = (unsigned char)((x >> 24) & 0xFF);

        ssize_t sent = 0;
        while (sent < 4) {
            ssize_t s = send(sockfd, out + sent, 4 - sent, 0);
            if (s <= 0) {
                if (s == 0) {
                    close(sockfd);
                    return 0;
                } else {
                    perror("send");
                    close(sockfd);
                    return 1;
                }
            }
            sent += s;
        }

        ssize_t got = 0;
        while (got < 4) {
            ssize_t r = recv(sockfd, inbuf + got, 4 - got, 0);
            if (r <= 0) {
                if (r == 0) {
                    close(sockfd);
                    return 0;
                } else {
                    perror("recv");
                    close(sockfd);
                    return 1;
                }
            }
            got += r;
        }

        int resp = (inbuf[0] | (inbuf[1] << 8) | (inbuf[2] << 16) | (inbuf[3] << 24));

        printf("%d\n", resp);
        fflush(stdout);
    }

    close(sockfd);
    return 0;
}
