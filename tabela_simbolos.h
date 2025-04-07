#ifndef TABELA_SIMBOLOS_H
#define TABELA_SIMBOLOS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum Tipo_e { INT,
                      FLOAT,
                      VAZIO } Tipo;
typedef enum TipoSimbolo_e { VARIAVEL,
                             FUNCAO,
                             PROC } TipoSimbolo;

struct simbolo {
    char *lexema;
    char *id_funcao;
    Tipo tipo;
    TipoSimbolo tipo_simb;
    int escopo;
    struct lista_args *args;
};

struct lista_args {
    Tipo tipo;
    char *lexema;
    struct lista_args *proximo;
};

struct lista_simbolo {
    struct simbolo *simb;
    struct lista_simbolo *proximo;
};

struct tabela_simbolos {
    struct simbolo *simb;
    struct tabela_simbolos *proximo;
};

// sem overload em ANSI C!
struct simbolo *novo_simbolo1(char *lexema);
struct simbolo *novo_simbolo2(char *lexema, TipoSimbolo tipo_simb);
struct simbolo *novo_simbolo3(char *lexema, TipoSimbolo tipo_simb, int escopo);
struct simbolo *novo_simbolo4(char *lexema, TipoSimbolo tipo_simb, int escopo,
                              Tipo tipo);

struct lista_simbolo *insere_lista_simbolo(struct lista_simbolo *lista,
                                           struct simbolo *simb);
void atualiza_tipo_simbolos(struct lista_simbolo *lista, Tipo t);
struct simbolo *novo_simbolo5(char *lexema, TipoSimbolo tipo_simb, int escopo,
                              char *id_funcao);
void insere_func_args(struct simbolo *funcao, struct lista_simbolo *args);
void free_lista_simbolo(struct lista_simbolo *lista);
struct tabela_simbolos *insere_simbolo_ts(struct tabela_simbolos *ts,
                                          struct simbolo *simb);
struct tabela_simbolos *insere_simbolos_ts(struct tabela_simbolos *ts,
                                           struct lista_simbolo *lista);
struct simbolo *busca_simbolo(struct tabela_simbolos *ts, char *lexema);
struct tabela_simbolos *remove_simbolos(struct tabela_simbolos *ts, int escopo);

void imprime_tabela_simbolos(FILE *fp, struct tabela_simbolos *ts);
void imprime_lista_simbolos(FILE *fp, struct lista_simbolo *lista);
#endif