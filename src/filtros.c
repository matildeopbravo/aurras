#include "../include/filtros.h"

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "auxiliary.h"

struct filtro {
  char*  identificador;
  char*  ficheiro_executavel;
  size_t max_instancias;
  size_t em_processamento;
};

struct catalogo_simples {
  char*                    identificador;
  struct catalogo_simples* prox;
};

struct catalogo_filtros {
  Filtro                   filtro;
  struct catalogo_filtros* prox;
};

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
//  falta verificar se é numero
static Filtro parse_filtro(char* string) {
  if (!string) return NULL;
  char* identificador       = strsep(&string, " ");
  char* ficheiro_executavel = strsep(&string, " ");
  char* max_instancias      = strsep(&string, " ");

  if (!max_instancias) return NULL;
  int max_instancias_valor = atoi(max_instancias);

  // if (1 != scanf(max_instancias, "%zu", &max_instancias_valor)) return NULL;
  return init_filtro(identificador, ficheiro_executavel, max_instancias_valor);
}

void show_filtro(Filtro filtro) {
  if (!filtro) return;
  printf("\nFILTRO\n");
  printf("\tidentificador\t: %s\n", filtro->identificador);
  printf("\tficheiro executavel: %s\n", filtro->ficheiro_executavel);
  printf("\tmax instancias\t: %zu\n", filtro->max_instancias);
  printf("\tem processamento: %zu\n\n", filtro->em_processamento);
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

Catalogo_filtros add_filtro_catalogo(Catalogo_filtros catalogo, Filtro filtro) {
  if (!filtro) return catalogo;

  Catalogo_filtros novo =
      (Catalogo_filtros) malloc(sizeof(struct catalogo_filtros));

  novo->filtro = filtro;
  novo->prox   = catalogo;
  return novo;
}

Catalogo_filtros init_fitros(char* config_name) {
  int fd;
  if ((fd = open(config_name, O_RDONLY)) < 0) return NULL;

  char* line = malloc(BUF_SIZE * (sizeof(char)));

  Catalogo_filtros catalogo = NULL;

  size_t total_read;
  while ((total_read = readln(fd, line, BUF_SIZE)) > 0) {
    Filtro filtro = parse_filtro(line);
    catalogo      = add_filtro_catalogo(catalogo, filtro);
  }

  close(fd);
  free(line);
  return catalogo;
}

Filtro search_filtro(Catalogo_filtros catalogo, char* name) {
  Filtro filtro = NULL;
  bool   find   = false;
  if (catalogo || name) {
    Catalogo_filtros e = catalogo;
    for (; e && !find; e = e->prox) {
      if (e->filtro) {
        find   = !strcmp(e->filtro->identificador, name);
        filtro = e->filtro;
      }
    }
  }
  return filtro;
}

bool valid_filtro(
    Catalogo_filtros catalogo, char* name_filtro) {  // verifica se é valido
  Filtro filtro = search_filtro(catalogo, name_filtro);
  return filtro != NULL && !strcmp(filtro->identificador, name_filtro);
}

// So aumenta se os em processamento não forem igual ao valor do max_instancias
bool increase_number_filtro(Catalogo_filtros catalogo, char* name) {
  bool   result = false;
  Filtro filtro = search_filtro(catalogo, name);
  if (filtro) {
    filtro->em_processamento++;
    if ((result = filtro->em_processamento > filtro->max_instancias)) {
      filtro->em_processamento = filtro->max_instancias;
    }
  }
  return !result;
}

// So diminui se os em processamento não forem igual ao valor do max_instancias
bool decrease_number_filtro(Catalogo_filtros catalogo, char* name) {
  bool   result = false;
  Filtro filtro = search_filtro(catalogo, name);
  if (filtro) {
    if ((result =
             filtro->em_processamento == 1 || filtro->em_processamento == 0)) {
      filtro->em_processamento = 0;
    }
    else {
      filtro->em_processamento--;
    }
  }
  return !result;
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

void show_one_filtro(Catalogo_filtros catalogo, char* name) {
  show_filtro(search_filtro(catalogo, name));
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

// TODO apagar
void teste_catalogo_filtros() {
  Catalogo_filtros catalogo = init_fitros("../etc/aurrasd.conf");
  printf("CATALOGO:\n");
  show_catalogo(catalogo);
  printf("--------------\n");

  printf("Is valid \"baixo\": %d\n", valid_filtro(catalogo, "baixo"));
  printf("Is valid \"hahah\": %d\n", valid_filtro(catalogo, "hahah"));
  printf("Is valid \"algo\"\t: %d\n", valid_filtro(catalogo, "algo"));
  printf("Is valid \"rapido\": %d\n", valid_filtro(catalogo, "rapido"));

  printf("--------------\n\n");

  printf("Increase value to rapido\n");
  printf("Executou :%d\n", increase_number_filtro(catalogo, "rapido"));
  show_one_filtro(catalogo, "rapido");
  printf("Increase value to rapido\n");
  printf("Executou :%d\n", increase_number_filtro(catalogo, "rapido"));
  printf("Increase value to rapido\n");
  printf("Executou :%d\n", increase_number_filtro(catalogo, "rapido"));
  show_one_filtro(catalogo, "rapido");

  printf("--------------\n\n");
  printf(
      " Decrease value -> Executou :%d\n",
      decrease_number_filtro(catalogo, "rapido"));
  printf(
      " Decrease value -> Executou :%d\n",
      decrease_number_filtro(catalogo, "rapido"));
  printf(
      " Decrease value -> Executou :%d\n",
      decrease_number_filtro(catalogo, "rapido"));
  printf(
      " Decrease value -> Executou :%d\n",
      decrease_number_filtro(catalogo, "rapido"));
  show_one_filtro(catalogo, "rapido");

  printf("--------------\n\n");
  printf("Dar free ao catalogo\n");
  free_catalogo_filtros(catalogo);
}

// int main(void) {
//  teste_catalogo_filtros();
//  return 1;
//}
