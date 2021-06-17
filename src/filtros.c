#include "../include/filtros.h"

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/auxiliary.h"
#include "../include/request.h"

Filtro* init_filtro(
    char* identificador, char* ficheiro_executavel, size_t max_instancias) {

  Filtro* filtro = (Filtro*) malloc(sizeof(struct filtro));
  *filtro        = (struct filtro){
      .identificador       = strdup(identificador),
      .ficheiro_executavel = strdup(ficheiro_executavel),
      .max_instancias      = max_instancias,
      .em_processamento    = 0};
  return filtro;
}

void free_filtro(Filtro* filtro) {
  if (!filtro) return;
  free(filtro->identificador);
  free(filtro->ficheiro_executavel);
  free(filtro);
}

// TODO mudar
//  falta verificar se é numero
static Filtro* parse_filtro(char* string) {
  if (!string) return NULL;
  char* identificador       = strsep(&string, " ");
  char* ficheiro_executavel = strsep(&string, " ");
  char* max_instancias      = strsep(&string, " ");

  if (!max_instancias) return NULL;
  int max_instancias_valor = atoi(max_instancias);

  // if (1 != scanf(max_instancias, "%zu", &max_instancias_valor)) return
  // NULL;
  return init_filtro(identificador, ficheiro_executavel, max_instancias_valor);
}

void show_filtro(Filtro* filtro) {
  if (!filtro) return;
  printf("\nFILTRO\n");
  printf("identificador: %s\n", filtro->identificador);
  printf("ficheiro executavel: %s\n", filtro->ficheiro_executavel);
  printf("max instancias: %zu\n", filtro->max_instancias);
  printf("em processamento: %zu\n", filtro->em_processamento);
}

/* Catalogo de filtros completo */

static void add_filtro_catalogo(CatalogoFiltros* catalogo, Filtro* filtro) {
  if (!filtro) return;

  catalogo->filtros[catalogo->used] = filtro;
  catalogo->used++;
}

CatalogoFiltros* init_catalogo_fitros(
    char*  config_name,
    char*  all_filters_string,
    size_t size,
    size_t size_used) {
  int fd;
  if ((fd = open(config_name, O_RDONLY)) < 0 && !all_filters_string)
    return NULL;

  char*            line          = malloc(BUF_SIZE * (sizeof(char)));
  size_t           size_catalogo = 0;
  CatalogoFiltros* catalogo      = NULL;

  size_t total_read;
  while (size_catalogo < MAX_FILTER_NUMBER &&
         (total_read = readln(fd, line, BUF_SIZE)) > 0) {
    Filtro* filtro = parse_filtro(line);

    if (catalogo) {
      add_filtro_catalogo(catalogo, filtro);
    }
    else {
      catalogo = (CatalogoFiltros*) malloc(sizeof(struct catalogo_filtros));
      catalogo->filtros[0] = filtro;
      catalogo->used       = 1;
    }

    // add string
    if (filtro) {
      char*  name_filtro     = filtro->identificador;
      size_t size_need       = strlen(name_filtro) + strlen(";");
      char*  add_string_name = malloc(size_need * sizeof(char));
      strcat(add_string_name, name_filtro);
      strcat(add_string_name, ";");

      if (size < (size_used + size_need)) {
        size               = 2 * size + size_need;
        all_filters_string = realloc(all_filters_string, size);
      }
      size_used += size_need;
      strcat(all_filters_string, strdup(add_string_name));
      free(add_string_name);

      size_catalogo++;
    }
  }
  if (size_used > 0) all_filters_string[size_used - 1] = '\n';
  close(fd);
  free(line);
  return catalogo;
}

Filtro* search_filtro(CatalogoFiltros* catalogo, char* name) {
  Filtro* filtro = NULL;
  bool    find   = false;
  size_t  size   = catalogo->used;
  if (catalogo && name) {
    for (int i = 0; i < size && !find; i++) {
      if (catalogo->filtros[i]) {
        find   = !strcmp(catalogo->filtros[i]->identificador, name);
        filtro = catalogo->filtros[i];
      }
    }
  }
  return filtro;
}

bool valid_filtro(
    CatalogoFiltros* catalogo, char* name_filtro) {  // verifica se é valido
  Filtro* filtro = search_filtro(catalogo, name_filtro);
  return filtro != NULL && !strcmp(filtro->identificador, name_filtro);
}

// So aumenta se os em processamento não forem igual ao valor do max_instancias
bool increase_number_filtro(CatalogoFiltros* catalogo, char* name) {
  bool    result = false;
  Filtro* filtro = search_filtro(catalogo, name);
  if (filtro) {
    filtro->em_processamento++;
    if ((result = filtro->em_processamento > filtro->max_instancias)) {
      filtro->em_processamento = filtro->max_instancias;
    }
  }
  return !result;
}

// So diminui se os em processamento não forem igual ao valor do max_instancias
bool decrease_number_filtro(CatalogoFiltros* catalogo, char* name) {
  bool    result = false;
  Filtro* filtro = search_filtro(catalogo, name);
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

static void free_array_filtros(Filtro* filtros[], size_t size) {
  for (int i = 0; i < size; i++) {
    free_filtro(filtros[i]);
  }
}

void free_catalogo_filtros(CatalogoFiltros* catalogo) {
  if (!catalogo) return;
  free_array_filtros(catalogo->filtros, catalogo->used);
  free(catalogo);
}

void show_catalogo(CatalogoFiltros* catalogo) {
  if (!catalogo) return;
  size_t size = catalogo->used;
  for (int i = 0; i < size; i++) show_filtro(catalogo->filtros[i]);
}

void show_one_filtro(CatalogoFiltros* catalogo, char* name) {
  show_filtro(search_filtro(catalogo, name));
}
