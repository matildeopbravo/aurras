#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../include/auxiliary.h"
#include "../include/filtros.h"
#include "../include/reply.h"
#include "../include/request.h"
#define BUFSIZE 1024

void print_server(Request* new_request) {

  printf("Client PID is %d\n", new_request->client_pid);
  for (int i = 0; i < new_request->number_filters; i++) {
    printf("Filter %d was requested\n", new_request->requested_filters[i]);
  }
  printf(
      "Input file : %s, output_file : %s\n ",
      new_request->input_file,
      new_request->output_file);
}
bool stop = false;

void sigterm_handler(int signum) {
  char buf[] = "Finishing up current requests. No more requests allowed.\n";
  write(STDOUT_FILENO, buf, sizeof(buf));
  stop = true;
}

bool is_available(CatalogoFiltros* catalogo, int indice, int number_required) {
  // Supondo um indice valido
  if (indice > catalogo->used) return false;
  return (catalogo->filtros[indice]->max_instancias -
          catalogo->filtros[indice]->em_processamento) >= number_required;
}

void inform_client(State state, pid_t pid) {
  Reply reply;
  reply.state = state;
  char server_to_client_pipe[1024];
  sprintf(server_to_client_pipe, "tubo_%d", pid);
  int server_to_client = open(server_to_client_pipe, O_WRONLY);
  write(server_to_client, &reply, sizeof(struct reply));
}
// chamado para o elemento acabado de tirar da queue
// retorna se iniciou o processamento ou false caso algum dos filtros nao
// esteja disponivel
bool processa_pedido(
    CatalogoFiltros* catalogo, Request* req, char* all_filters_string) {
  switch (req->request_type) {
    case TRANSFORM: {
      // é mandado diretamente pra o filho q trata da queue

      // int number_required[MAX_FILTER_NUMBER] = {0};
      // for (int i = 0, j = 0; i < catalogo->used && j <
      // req->number_filters;
      //      i++) {
      //   int filtro_pedido            = req->requested_filters[i];
      //   int number_requested_filters =
      //   ++number_required[filtro_pedido]; if (!is_available(catalogo,
      //   filtro_pedido, number_requested_filters))
      //   {
      //     return false;
      //   }
      // }
      return false;
      break;
    }
    case STATUS:
      break;
    case HANDSHAKE: {
      char fifo_name[1024];
      sprintf(fifo_name, "tubo_%d", req->client_pid);
      int tubo_escrita = open(fifo_name, O_WRONLY);
      write(
          tubo_escrita,
          all_filters_string,
          strlen(all_filters_string) * sizeof(char));
      break;
    }
  }
  return true;
}

int executa_pedido(CatalogoFiltros* catalogo, Request* request) {
  // tem que devolver 0 se os execs deram todos certo
  // tem que criar a pipeline e aplicar todos os filtros

  fprintf(stderr, "executa pedido foi executado\n");
  int n          = request->number_filters;  // numero de execs a fazer
  int beforePipe = open(request->input_file, O_RDONLY);
  int afterPipe[2];
  for (int i = 0; i < n; i++) {
    if (i < n - 1) pipe(afterPipe);
    if (!fork()) {
      dup2(beforePipe, 0);
      close(beforePipe);
      if (i < n - 1) {
        dup2(afterPipe[1], STDOUT_FILENO);
        close(afterPipe[0]);
        close(afterPipe[1]);
      }
      else {
        int fd_output =
            open(request->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        dup2(fd_output, STDOUT_FILENO);
        close(fd_output);
      }
      // fazer exec
      char    path[256];
      Filtro* filtro = catalogo->filtros[request->requested_filters[i]];
      sprintf(path, "bin/aurrasd-filters/%s", filtro->ficheiro_executavel);
      execl(path, path, NULL);  // le do stdin
      perror("exec failed");
      _exit(1);
    }
    if (i < n - 1) close(afterPipe[1]);
    close(beforePipe);
    beforePipe = afterPipe[0];
  }

  int exit_code = 0;
  for (int i = 0; i < n; i++) {
    int status;
    wait(&status);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
      // avisa o cliente que terminou
    }
    else {
      exit_code = 1;
    }
  }
  return exit_code;
}

int main(int argc, char* argv[]) {
  if (argc == 3) {
    config_path = argv[1];
    filter_path = argv[2];
  }
  else {
    char buf[] = "wrong number of arguments\n";
    write(STDERR_FILENO, buf, sizeof(buf));
    _exit(1);
  }

  if (signal(SIGTERM, sigterm_handler) == SIG_ERR) {
    perror("SIGQUIT");
    return 1;
  }

  char* all_filters_string = (char*) malloc(sizeof(char) * BUFSIZE);

  CatalogoFiltros* catalogo =
      init_catalogo_fitros(all_filters_string, BUFSIZE, 0);
  if (!catalogo || catalogo->used == 0) {
    printf("Erro-> Não foi possivel a criacao do Catalogo de filtros\n\n");
    _exit(1);
  }

  // TODO apagar printf
  printf("CATALOGO DE FILTROS\n");
  show_catalogo(catalogo);
  printf("-----------------------\n");
  printf("\n%s\n\n", all_filters_string);

  mkfifo("client_to_server", 0644);
  int fd_leitura = open("client_to_server", O_RDONLY);
  open("client_to_server", O_WRONLY);

  // vai tratar da queue
  int pipe_onhold[2], pipe_updates[2], pipe_execucao[2];
  if (pipe(pipe_onhold) == -1 || pipe(pipe_updates) == -1 ||
      pipe(pipe_execucao) == -1) {
    perror("Pipe creation failed");
    _exit(1);
  }
  if (fork() == 0) {
    close(pipe_onhold[1]);
    // vai mandar o estado atual dos filtros cada vez que algum e liberatdo
    close(pipe_updates[1]);
    close(pipe_execucao[0]);

    // criacao da queue
    Queue*  queue = NULL;
    Queue** last_request;
    size_t  max_queue = 10;

    while (1) {
      /* se a queue estiver vazia, só le pedidos ??. nao porque ele pode
       * receber pedidos acabados estrategia seria um bool q diria o q
       * faria mudar nome
       */
      Request request;
      read(pipe_onhold[0], &request, sizeof(Request));
      // verifica se pode ser executado
      if (valid_request_to_execute(&request, catalogo)) {
        // se sim mandado
        write(pipe_execucao[1], &request, sizeof(Request));
        update_catalogo_execute_request(catalogo, request);
      }
      else {
        // caso contrario adiciona o a queue
        if (!queue) {
          queue = init_queue(&request);
          // TODO verificar
          last_request = &queue;
        }
        else {
          add_request_to_queue(&request, last_request);
        }
      }
      // recebe um request q ja foi concluido
      Request fake_request;
      read(pipe_updates[0], &fake_request, sizeof(Request));
      update_catalogo_done_request(catalogo, fake_request);

      // verifica se algum filtro(s) pode ser executado
      // TODO ou alterar e remover o endereco aser usado, ou entao
      // verificar se varias funcoes podem ser executadas
      Request* request_to_execute = can_execute_request(queue, catalogo);
      // se sim manda
      if (request_to_execute) {
        if (request_to_execute->request_type == STATUS)
          update_fake_request(catalogo, request_to_execute);
        write(pipe_execucao[1], request_to_execute, sizeof(Request));
        update_catalogo_execute_request(catalogo, *request_to_execute);
      }
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
  close(pipe_onhold[0]);
  close(pipe_onhold[1]);
  close(pipe_execucao[1]);
  close(pipe_updates[0]);
  if (fork() == 0) {
    // so chegam aqui requests que tem todos os filtros disponiveis
    Request request;
    while (read(pipe_execucao[0], &request, sizeof(Request)) > 0) {

      printf(" lixo ? %d\n", request.request_type);
      _exit(0);
      // if (request.request_type == HANDSHAKE) continue;

      fprintf(stderr, "teste\n");
      if (fork() == 0) {
        // vai fazer a pipeline de execs
        int r = executa_pedido(catalogo, &request);
        _exit(r);
      }
      inform_client(PROCESSING, request.client_pid);

      int status;
      wait(&status);
      // ver que filtros foram libertados e enviar para o pipe
      // envia o request q terminou
      write(pipe_updates[1], &request, sizeof(Request));
      State state =
          (WIFEXITED(status) && WEXITSTATUS(status) == 0) ? FINISHED : NOTHING;
      inform_client(state, request.client_pid);
    }
    // le do pipe_execucao que ja e uma fila de espera, e recebe tanto
    // pedidos que foram retirados da queue como mandados diretamente faz
    // fork exec e avisa o cliente
  }

  /* Leitura principal de novos pedidos atraves do pipe com nome */
  Request* new_request = malloc(sizeof(struct request));
  while (!stop && read(fd_leitura, new_request, sizeof(struct request)) > 0) {
    print_server(new_request);
    bool ready = processa_pedido(catalogo, new_request, all_filters_string);
    if (new_request->request_type == HANDSHAKE) continue;
    int chosen_pipe = ready ? pipe_execucao[1] : pipe_onhold[1];
    if (!ready) {
      inform_client(PENDING, new_request->client_pid);
    }
    write(chosen_pipe, new_request, sizeof(Request));
  }
  if (catalogo) free_catalogo_filtros(catalogo);
  if (all_filters_string) free(all_filters_string);
  free(new_request);
}
