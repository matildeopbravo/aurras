#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// TODO Mudar para .h
#include "auxiliary.c"

#define MAX_BUFF 1024

struct filtro {
  char*  identificador;
  char*  ficheiro_executavel;
  size_t max_instancias;
  size_t em_processamento;
};
typedef struct filtro* Filtro;

struct catalogo_simples {
  char*                    identificador;
  struct catalogo_simples* prox;
};
typedef struct catalogo_simples* Catalogo_simples;

struct catalogo_filtros {
  // Tree* lista_filtros;  // ou uma lista ligada
  // ordenada por identificador

  Filtro                   filtro;
  struct catalogo_filtros* prox;
  // size_t total;
};
typedef struct catalogo_filtros* Catalogo_filtros;

Filtro init_filtro(
    char* identificador, char* ficheiro_executavel, size_t max_instancias) {

  Filtro filtro = (Filtro) malloc(sizeof(struct filtro));
  *filtro       = (struct filtro){
      .identificador       = strdup(identificador),
      .ficheiro_executavel = strdup(ficheiro_executavel),
      .max_instancias      = max_instancias,
      .em_processamento    = 0};
  return filtro;
}

void free_filtro(Filtro filtro) {
  if (!filtro) return;
  free(filtro->identificador);
  free(filtro->ficheiro_executavel);
  free(filtro);
}

// TODO mudar
// pode dar overflow
static Filtro parse_filtro(char* string) {
  if (!string) return NULL;
  char* identificador       = strsep(&string, " ");
  char* ficheiro_executavel = strsep(&string, " ");
  char* max_instancias      = strsep(&string, " ");

  if (!max_instancias) return NULL;
  size_t max_instancias_valor = -1;

  if (1 != scanf(max_instancias, "%zu", &max_instancias_valor)) return NULL;

  return init_filtro(identificador, ficheiro_executavel, max_instancias_valor);
}

void show_filtro(Filtro filtro) {
  if (!filtro) return;
  printf("\nFILTRO\n");
  printf("\tidentificador: %s\n", filtro->identificador);
  printf("\tficheiro executavel: %s\n", filtro->ficheiro_executavel);
  printf("\tmax instancias: %zu\n", filtro->em_processamento);
  printf("\tem processamento: %zu\n\n", filtro->max_instancias);
}

/* Catalogo de filtros simples */

Catalogo_simples add_catalogo_simples(char* string, Catalogo_simples catalogo) {
  Catalogo_simples novo =
      (Catalogo_simples) malloc(sizeof(struct catalogo_simples));

  novo->identificador = strdup(string);
  novo->prox          = catalogo;
  return novo;
}

Catalogo_simples init_catalogo_simples(char* matrix[], size_t N) {
  if (!matrix) return NULL;
  Catalogo_simples catalogo = NULL;

  for (int i = N - 1; i >= 0; --i) {
    if (matrix[i]) catalogo = add_catalogo_simples(matrix[i], catalogo);
  }
  return catalogo;
}

void free_catalogo_simples(Catalogo_simples catalogo) {
  Catalogo_simples endereco;

  while (catalogo) {
    endereco       = catalogo;
    catalogo       = catalogo->prox;
    endereco->prox = NULL;
    if (endereco->identificador) free(endereco->identificador);
    free(endereco);
  }
}

void show_catalogo_simples(Catalogo_simples catalogo) {
  while (catalogo) {
    printf("%s\n", catalogo->identificador);
    catalogo = catalogo->prox;
  }
}

bool valid_filtro_catalogo_simples(Catalogo_simples catalogo, char* filtro) {
  bool result = false;
  if (catalogo || filtro) {
    Catalogo_simples e = catalogo;
    for (; e && !result; e = e->prox) {
      result = !strcmp(e->identificador, filtro);
    }
  }
  return result;
}

/* Catalogo de filtros completo */

// TODO verificar
Catalogo_filtros add_filtro_catalogo(Catalogo_filtros catalogo, Filtro filtro) {
  if (!filtro) return catalogo;

  Catalogo_filtros novo =
      (Catalogo_filtros) malloc(sizeof(struct catalogo_filtros));

  novo->filtro = filtro;
  novo->prox   = catalogo;
  return novo;
}

// ssize_t readln2(int fd, char* line, size_t size) {
//  ssize_t i = 0;
//  ssize_t bytes_read;
//  while (i < size && (bytes_read = read(fd, &line[i], 1)) > 0 &&
//         line[i] != '\n')
//    i++;
//  line[i++] = '\n';
//  return i;
//}

// TODO
Catalogo_filtros init_fitros(char* config_name) {
  int fd;
  if ((fd = open(config_name, O_RDONLY)) < 0) return NULL;

  char* line = malloc(BUF_SIZE * (sizeof(char)));

  Catalogo_filtros catalogo = NULL;

  size_t total_read;
  while ((total_read = readln(fd, line, BUF_SIZE)) > 0) {
    // printf("%s\n", line);
    Filtro filtro = parse_filtro(line);
    catalogo      = add_filtro_catalogo(catalogo, filtro);
  }

  close(fd);
  free(line);
  return catalogo;
}

// TODO verificar
bool valid_filtro(
    Catalogo_filtros catalogo, char* filtro) {  // verifica se é valido
  bool result = false;
  if (catalogo || filtro) {
    Catalogo_filtros e = catalogo;
    for (; e && !result; e = e->prox) {
      if (e->filtro) result = !strcmp(e->filtro->identificador, filtro);
    }
  }
  return result;
}

bool increase_number_filtro(Catalogo_filtros catalogo, char* name) {
  bool result = false;
  if (catalogo || name) {
    Catalogo_filtros e = catalogo;
    for (; e && !result; e = e->prox) {
      if (e->filtro) result = !strcmp(e->filtro->identificador, name);
      if (result) e->filtro->max_instancias++;
    }
  }
  return result;
}

bool decrease_number_filtro(Catalogo_filtros catalogo, char* name) {
  bool result = false;
  if (catalogo || name) {
    Catalogo_filtros e = catalogo;
    for (; e && !result; e = e->prox) {
      if (e->filtro) result = !strcmp(e->filtro->identificador, name);
      if (result) e->filtro->max_instancias--;
    }
  }
  return result;
}

void free_catalogo_filtros(Catalogo_filtros catalogo) {
  Catalogo_filtros endereco;

  while (catalogo) {
    endereco       = catalogo;
    catalogo       = catalogo->prox;
    endereco->prox = NULL;
    free(endereco->filtro);
    free(endereco);
  }
}

void show_catalogo(Catalogo_filtros catalogo) {
  Catalogo_filtros e = catalogo;
  while (e) {
    show_filtro(e->filtro);
    e = e->prox;
  }
}

// TODO apagar
void teste_catalogo_simples() {
  int              N        = 3;
  char*            matrix[] = {"ola", "adeus", "Taraaa"};
  Catalogo_simples catalogo = init_catalogo_simples(matrix, N);

  printf("CATALOGO:\n");
  show_catalogo_simples(catalogo);
  printf("--------------\n\n");

  printf(
      "Is valid \"adeus\": %d\n",
      valid_filtro_catalogo_simples(catalogo, "adeus"));

  printf(
      "Is valid \"hahah\": %d\n",
      valid_filtro_catalogo_simples(catalogo, "hahah"));

  printf(
      "Is valid \"algo\": %d\n",
      valid_filtro_catalogo_simples(catalogo, "algo"));

  printf("\n\nCATALOGO:\n");
  show_catalogo_simples(catalogo);
  printf("--------------\n");
  printf("Dar free ao catalogo\n");
  free_catalogo_simples(catalogo);
}

void teste_catalogo_filtros() {
  // Catalogo_filtros catalogo = init_fitros("etc/aurrasd.conf");

  char* s  = "alto aurrasd-gain-double 2";
  char* s1 = "baixo aurrasd-gain-half 2";
  char* s2 = "eco aurrasd-echo 1";
  char* s3 = "rapido aurrasd-tempo-double 2";
  char* s4 = "lento aurrasd-tempo-half 1";

  Filtro f  = parse_filtro(s);
  Filtro f1 = parse_filtro(s1);
  Filtro f2 = parse_filtro(s2);
  Filtro f3 = parse_filtro(s3);
  Filtro f4 = parse_filtro(s4);

  Catalogo_filtros catalogo = NULL;
  catalogo                  = add_filtro_catalogo(catalogo, f);
  catalogo                  = add_filtro_catalogo(catalogo, f1);
  catalogo                  = add_filtro_catalogo(catalogo, f2);
  catalogo                  = add_filtro_catalogo(catalogo, f3);
  catalogo                  = add_filtro_catalogo(catalogo, f4);

  show_catalogo(catalogo);
  free_catalogo_filtros(catalogo);
}

// int main(void) {
//  teste_catalogo_filtros();
//  return 1;
//}
