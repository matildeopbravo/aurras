#ifndef FILTROS_H
#define FILTROS_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct filtro* Filtro;

typedef struct catalogo_simples* Catalogo_simples;

typedef struct catalogo_filtros* Catalogo_filtros;

Filtro init_filtro(
    char* identificador, char* ficheiro_executavel, size_t max_instancias);
void free_filtro(Filtro filtro);

static Filtro parse_filtro(char* string);

void show_filtro(Filtro filtro);

Catalogo_simples add_catalogo_simples(char* string, Catalogo_simples catalogo);

Catalogo_simples init_catalogo_simples(char* matrix[], size_t N);

void free_catalogo_simples(Catalogo_simples catalogo);

void show_catalogo_simples(Catalogo_simples catalogo);

bool valid_filtro_catalogo_simples(Catalogo_simples catalogo, char* filtro);

Catalogo_filtros add_filtro_catalogo(Catalogo_filtros catalogo, Filtro filtro);

Catalogo_filtros init_fitros(char* config_name);

Filtro search_filtro(Catalogo_filtros catalogo, char* name);

bool valid_filtro(Catalogo_filtros catalogo, char* name_filtro);

bool increase_number_filtro(Catalogo_filtros catalogo, char* name);

bool decrease_number_filtro(Catalogo_filtros catalogo, char* name);

void free_catalogo_filtros(Catalogo_filtros catalogo);

void show_catalogo(Catalogo_filtros catalogo);

void show_one_filtro(Catalogo_filtros catalogo, char* name);
#endif
