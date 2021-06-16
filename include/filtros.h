#ifndef FILTROS_H
#define FILTROS_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct filtro {
    char* identificador;
    char* ficheiro_executavel;
    size_t max_instancias;
    size_t em_processamento;
} Filtro;

typedef struct catalogo_filtros {
    Filtro* filtro;
    struct catalogo_filtros* prox;
} CatalogoFiltros;

Filtro* init_filtro(
    char* identificador, char* ficheiro_executavel, size_t max_instancias);
void free_filtro(Filtro* filtro);

void show_filtro(Filtro* filtro);

void show_catalogo(CatalogoFiltros* catalogo);

CatalogoFiltros* init_catalogo_fitros(char* config_name);
void free_catalogo_filtros(CatalogoFiltros* catalogo);

#endif
