#ifndef FILTROS_H
#define FILTROS_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../include/request.h"

extern char* config_path;
extern char* filter_path;

typedef struct filtro {
  char*  identificador;
  char*  ficheiro_executavel;
  size_t max_instancias;
  size_t em_processamento;
} Filtro;

typedef struct catalogo_filtros {
  Filtro* filtros[MAX_FILTER_NUMBER];
  size_t  used;
} CatalogoFiltros;

void free_filtro(Filtro* filtro);

void show_filtro(Filtro* filtro);

CatalogoFiltros* init_catalogo_fitros(
    char* all_filters_string, size_t size, size_t size_used);

Filtro* search_filtro(CatalogoFiltros* catalogo, char* name);

bool valid_filtro(CatalogoFiltros* catalogo, char* name_filtro);

void increase_number_filtro(CatalogoFiltros* catalogo, size_t indice);

void decrease_number_filtro(CatalogoFiltros* catalogo, size_t indice);

void show_filtro(Filtro* filtro);

void show_catalogo(CatalogoFiltros* catalogo);

void free_catalogo_filtros(CatalogoFiltros* catalogo);

void update_catalogo_done_request(CatalogoFiltros* catalogo, Request request);

void update_catalogo_execute_request(
    CatalogoFiltros* catalogo, Request request);
void update_fake_request(CatalogoFiltros* catalogo, Request* fake_request);
#endif
