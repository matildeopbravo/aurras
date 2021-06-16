#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
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

bool is_available(
    ListaFiltros* catalogo, int indice, size_t number_requested_filters) {
    // {indice valido}
    for (int i = 0; i < indice; i++, catalogo = catalogo->prox)
        ;
    return (catalogo->filtro->max_instancias -
            catalogo->filtro->em_processamento) >= number_requested_filters;
    // {pos condicao}
}

// chamado para o elemento acabado de tirar da queue
// retorna se iniciou o processamento ou false caso algum dos filtros nao
// esteja disponivel
bool processa_pedido(CatalogoFiltros* catalogo, Request* req) {
    ListaFiltros* lista_filtros = catalogo->lista_filtros;
    if (req->request_type == TRANSFORM) {
        // TO FIX
        for (int i = 0, j = 0; i < catalogo->size && j < req->number_filters;
             i++) {
            size_t number_requested_filters = req->requested_filters[i];
            if (number_requested_filters > 0) {
                if (!is_available(lista_filtros, i, number_requested_filters)) {
                    return false;
                }
            }
        }
        // status
    }
    else {
    }
    return true;
}

int executa_pedido(Request* request) {
    // tem que devolver 0 se os execs deram todos certo
    // tem que criar a pipeline e aplicar todos os filtros

    // dado o indice, ir buscar o nome do executavel para fazer exec
    return 0;
}

int main(int argc, char* argv[]) {
    if (signal(SIGTERM, sigterm_handler) == SIG_ERR) {
        perror("SIGQUIT");
        return 1;
    }

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

    mkfifo("client_to_server", 0644);
    int fd_leitura = open("client_to_server", O_RDONLY);
    open("client_to_server", O_WRONLY);

    printf("\n%s\n\n", all_filters_string);
    // vai tratar da queue
    int pipe_onhold[2], pipe_ready[2], pipe_updates[2], pipe_execucao[2];
    if (pipe(pipe_onhold) == -1 || pipe(pipe_ready) == -1 ||
            pipe(pipe_updates) == -1,
        pipe(pipe_execucao) == -1) {
        perror("Pipe creation failed");
    }
    if (fork() == 0) {
        close(pipe_onhold[1]);
        close(pipe_ready[0]);
        // vai mandar o estado atual dos filtros cada vez que algum e liberatdo
        close(pipe_updates[1]);
        close(pipe_execucao[0]);
        // criacao da queue
        while (1) {
            Request request;
            read(pipe_onhold[0], &request, sizeof(Request));
            Request fake_request;
            // vai devolver as stats, no array  dos filtros em que em cada
            // posicao indica o numero de filtros livres
            read(pipe_updates[0], &fake_request, sizeof(Request));
        }
        // le do pipe on hold e coloca no fim da queue
        // le alternadamente do pipe updates e do pipe on hold
        // processo de decisao de qual executar
        // escreve o Request selecionado para o pipe_execucao que vai fazer a
        // execucao a serio
    }

    /*  # proximo filho
    vai meter os filtros em execucao
    vai ler um request do pipe que e efetivamente o que vai ser executado  e
    faz fork exec quem escreve para este pipe sera o pai que le do pipe com
    nome e o outro filho que le do pipe anonimo da queue
   */
    if (fork() == 0) {
        // so chegam aqui requests que tem todos os filtros disponiveis
        Request request;
        while (read(pipe_execucao[0], &request, sizeof(Request)) > 0) {
            if (fork() == 0) {
                // vai fazer a pipeline de execs
                int r = executa_pedido(&request);
                _exit(r);
            }
            // avisar o cliente que esta processing

            int status;
            wait(&status);
            // ver que filtros foram libertados e enviar para o pipe
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                // avisa o cliente que terminou
            }
            else {
                // tratar o  erro, pode ter sido porque o file nao existe ou
                // assim
            }
        }
        // le do pipe_execucao que ja e uma fila de espera, e recebe tanto
        // pedidos que foram retirados da queue como mandados diretamente faz
        // fork exec e avisa o cliente
    }

    size_t bytes_read = 0;
    Request* new_request = malloc(sizeof(struct request));
    while (!stop &&
           (bytes_read =
                read(fd_leitura, new_request, sizeof(struct request))) > 0) {
        printf("Client PID is %d\n", new_request->client_pid);
        // le do pipe anonimo que vem as cenas da queue

        if (new_request->request_type == HANDSHAKE) {
            char fifo_name[1024];
            sprintf(fifo_name, "tubo_%d", new_request->client_pid);
            int tubo_escrita = open(fifo_name, O_WRONLY);
            // temos que ter alguma cena com os clientes atualmnte conectados
        }

        else {
            // TODO
            // ver se os filtros do request lido estao disponiveis,
            // se sim envia para o pipe de execucao. Caso contrario manda para o
            // pipe on hold e avisa o client que esta pending (?)

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
            // Reply reply;
            // reply.state = PENDING;

            // char server_to_client_pipe[1024];
            // sprintf(server_to_client_pipe, "tubo_%d",
            // new_request->client_pid); int server_to_client =
            // open(server_to_client_pipe, O_WRONLY); write(server_to_client,
            // &reply, sizeof(struct reply)); sleep(5); reply.state =
            // PROCESSING; write(server_to_client, &reply, sizeof(struct
            // reply)); sleep(5); reply.state = FINISHED;
            // write(server_to_client, &reply, sizeof(struct reply));
            // sleep(5);
            /* fim do debug de um reply */
            processa_pedido(catalogo, new_request);
        }
    }
    if (catalogo) free_catalogo_filtros(catalogo);
    if (all_filters_string) free(all_filters_string);
    free(new_request);
}
