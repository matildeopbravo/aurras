#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../include/filtros.h"
#include "../include/reply.h"
#include "../include/request.h"
#define BUFSIZE 1024
#define PATH_FILTROS "etc/aurrasd.conf"

bool stop = false;

void sigterm_handler(int signum) {
    char buf[] = "Finishing up current requests. No more requests allowed.\n";
    write(STDOUT_FILENO, buf, sizeof(buf));
    stop = true;
}

bool is_available(CatalogoFiltros* catalogo, int indice) {
    for (int i = 0; i < indice; i++, catalogo = catalogo->prox)
        ;
    return catalogo->filtro->max_instancias -
               catalogo->filtro->em_processamento >
           0;
}

// chamado para o elemento acabado de tirar da queue
// retorna se iniciou o processamento ou false caso algum dos filtros nao
// esteja disponivel
bool processa_pedido(CatalogoFiltros* catalogo, Request* req) {
    if (req->request_type == TRANSFORM) {
        // TO FIX
        for (int i = 0, j = 0; i < MAX_FILTER_NUMBER && j < req->number_filters;
             i++) {
            if (req->requested_filters[i] > 0) {
                if (!is_available(catalogo, i)) {
                    return false;
                }
                j++;
            }
        }
    }
    // status
    else {
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (signal(SIGTERM, sigterm_handler) == SIG_ERR) {
        perror("SIGQUIT");
        return 1;
    }

    mkfifo("client_to_server", 0644);
    int fd_leitura = open("client_to_server", O_RDONLY);
    open("client_to_server", O_WRONLY);

    char* all_filters_string = (char*) malloc(sizeof(char) * BUFSIZE);

    printf("CATALOGO DE FILTROS\n");
    CatalogoFiltros* catalogo =
        init_catalogo_fitros(PATH_FILTROS, all_filters_string, BUFSIZE, 0);
    if (!catalogo)
        printf("Erro-> Não foi possivel a criacao do Catalogo de filtros\n\n");
    // msg de erro
    show_catalogo(catalogo);
    printf("-----------------------\n");

    printf("\n%s\n\n", all_filters_string);

    size_t bytes_read = 0;
    Request* new_request = malloc(sizeof(struct request));
    while (!stop &&
           (bytes_read =
                read(fd_leitura, new_request, sizeof(struct request))) > 0) {
        printf("Client PID is %d\n", new_request->client_pid);
        if (new_request->request_type == HANDSHAKE) {
            char fifo_name[1024];
            sprintf(fifo_name, "tubo_%d\n", getpid());
            int tubo_leitura = open(fifo_name, O_WRONLY);
            // temos que ter alguma cena com os clientes atualmnte conectados
        }
        else {
            // isto e so para debug
            printf(
                "Request type is %s\n",
                new_request->request_type == TRANSFORM ? "Transform"
                                                       : "Status");
            for (size_t i = 0; i < MAX_FILTER_NUMBER; i++) {
                size_t n = 0;
                if ((n = new_request->requested_filters[i]) > 0) {
                    printf("Filter number %zu was requested %zu times\n", i, n);
                }
            }
            printf(
                "Input file : %s, output_file : %s\n ",
                new_request->input_file,
                new_request->output_file);
            /* criação de um reply para debug */
            Reply reply;
            reply.state = PENDING;

            char server_to_client_pipe[1024];
            sprintf(server_to_client_pipe, "tubo_%d", new_request->client_pid);
            int server_to_client = open(server_to_client_pipe, O_WRONLY);
            write(server_to_client, &reply, sizeof(struct reply));
            sleep(5);
            reply.state = PROCESSING;
            write(server_to_client, &reply, sizeof(struct reply));
            sleep(5);
            reply.state = FINISHED;
            write(server_to_client, &reply, sizeof(struct reply));
            sleep(5);
            /* fim do debug de um reply */
            processa_pedido(catalogo, new_request);
            if (catalogo) free_catalogo_filtros(catalogo);
            free(new_request);
            if (all_filters_string) free(all_filters_string);
        }
    }
}
