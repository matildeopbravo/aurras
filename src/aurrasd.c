#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../include/filtros.h"
#include "../include/request.h"
#define BUFSIZE 1024
#define PATH_FILTROS "etc/aurrasd.conf"

Catalogo_filtros catalogo = NULL;

void sigter_handler(int signum) {
    printf("Finishing up current requests. No more requests allowed.\n");

    // codigo de término
    if (catalogo) free_catalogo_filtros(catalogo);
}

int main(int argc, char* argv[]) {
    if (signal(SIGTERM, sigter_handler) == SIG_ERR) {
        perror("SIGQUIT");
        return 1;
    }

    mkfifo("client_to_server", 0644);
    int fd_leitura = open("client_to_server", O_RDONLY);
    open("client_to_server", O_WRONLY);

    printf("CATALOGO DE FILTROS\n");
    catalogo = init_catalogo_fitros(PATH_FILTROS);
    if (!catalogo)
        printf("Erro-> Não foi possivel a criacao do Catalogo de filtros\n\n");
    // msg de erro
    show_catalogo(catalogo);
    printf("-----------------------\n");

    size_t bytes_read = 0;
    Request new_request;
    while ((bytes_read =
                read(fd_leitura, &new_request, sizeof(struct request))) > 0) {
        printf("Client PID is %d\n", new_request.client_pid);
        if (new_request.request_type == HANDSHAKE) {
            char fifo_name[1024];
            sprintf(fifo_name, "tubo_%d\n", getpid());
            int tubo_leitura = open(fifo_name, O_WRONLY);
            // envia os filtros disponiveis
            // temos que ter alguma cena com os clientes atualmnte conectados
        }
        else {
            printf(
                "Request type is %s\n",
                new_request.request_type == TRANSFORM ? "Transform" : "Status");
            for (size_t i = 0; i < MAX_FILTER_NUMBER; i++) {
                size_t n = 0;
                if ((n = new_request.requested_filters[i]) > 0) {
                    printf("Filter number %zu was requested %zu times\n", i, n);
                }
            }
            printf(
                "Input file : %s, output_file : %s\n ",
                new_request.input_file,
                new_request.output_file);
        }
    }
}

