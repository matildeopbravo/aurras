#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  size_t total;
};
typedef struct catalogo_filtros* Catalogo_filtros;

Filtro init_filtro(
    char*  identificador,
    char*  ficheiro_executavel,
    size_t max_instancias,
    size_t em_processamento) {

  Filtro filtro = (Filtro) malloc(sizeof(struct filtro));
  *filtro       = (struct filtro){
      .identificador       = strdup(identificador),
      .ficheiro_executavel = strdup(ficheiro_executavel),
      .max_instancias      = max_instancias,
      .em_processamento    = em_processamento};
  return filtro;
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

// TODO
void add_filtro_catalogo(Catalogo_filtros catalogo, Filtro filtro) {
  if (!filtro) return;

  if (!catalogo)
    // TODO iniciar com o filtro dado
    return;
}

// TODO
Catalogo_filtros* init_fitros(char* config_name) {
  // ler o ficheiro de configuracao
  // verifica linhas invalidas ou nao
  // adiciona no Catalogo
  return NULL;
}

// TODO
bool valid_filtro(char* filtro) {  // verifica se é valido
  return false;
}

// int main(void) {
//  int              N        = 3;
//  char*            matrix[] = {"ola", "adeus", "Taraaa"};
//  Catalogo_simples catalogo = init_catalogo_simples(matrix, N);
//
//  printf("CATALOGO:\n");
//  show_catalogo_simples(catalogo);
//  printf("--------------\n\n");
//
//  printf(
//      "Is valid \"adeus\": %d\n",
//      valid_filtro_catalogo_simples(catalogo, "adeus"));
//
//  printf(
//      "Is valid \"hahah\": %d\n",
//      valid_filtro_catalogo_simples(catalogo, "hahah"));
//
//  printf(
//      "Is valid \"algo\": %d\n",
//      valid_filtro_catalogo_simples(catalogo, "algo"));
//
//  printf("\n\nCATALOGO:\n");
//  show_catalogo_simples(catalogo);
//  printf("--------------\n");
//  printf("Dar free ao catalogo\n");
//  free_catalogo_simples(catalogo);
//}
