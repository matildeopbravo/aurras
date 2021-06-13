#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#define BUFSIZE 1024

void sigter_handler(int signum) {
    printf("Finishing up current requests. No more requests allowed.\n");
    // codigo de término
}

int main(int argc, char* argv[]) {
    if (signal(SIGTERM, sigter_handler) == SIG_ERR) {
        perror("SIGQUIT");
        return 1;
    }
    mkfifo("servidor_cliente", 0644);
    int fd_leitura = open("servidor_cliente", O_RDONLY);
    int fd_escrita = open("servidor_cliente", O_WRONLY);
    size_t bytes_read = 0;
    char buf[BUFSIZE];
    while ((bytes_read = read(fd_leitura, buf, BUFSIZE)) > 0) {
    }
}

