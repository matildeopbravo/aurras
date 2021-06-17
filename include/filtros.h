#ifndef FILTROS_H
#define FILTROS_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../include/request.h"

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

// typedef struct lista_filtros {
//    Filtro* filtro;
//    struct lista_filtros* prox;
//} ListaFiltros;
//
// typedef struct catalogo_filtros {
//    ListaFiltros* lista_filtros;
//    size_t size;
//} CatalogoFiltros;
//
void free_filtro(Filtro* filtro);

void show_filtro(Filtro* filtro);

// CatalogoFiltros* add_filtro_catalogo(CatalogoFiltros* catalogo, Filtro*
// filtro);
//
CatalogoFiltros* init_catalogo_fitros(
    char* config_name, char* all_filters_string, size_t size, size_t size_used);

Filtro* search_filtro(CatalogoFiltros* catalogo, char* name);

bool valid_filtro(CatalogoFiltros* catalogo, char* name_filtro);

bool increase_number_filtro(CatalogoFiltros* catalogo, char* name);

void show_filtro(Filtro* filtro);

void show_catalogo(CatalogoFiltros* catalogo);

void free_catalogo_filtros(CatalogoFiltros* catalogo);

#endif
