#ifndef TABELA_SIMBOLOS_H
#define TABELA_SIMBOLOS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum Tipo_e { INT,
                      FLOAT,
                      VAZIO } Tipo;
typedef enum TipoSimbolo_e { VARIAVEL,
                              NUMERO,
                             FUNCAO,
                             PROC,
                             RETORNO } TipoSimbolo;

struct simbolo {
  char *lexema;
  char *id_funcao;
  int id_llvm;
  char *id_llvm_str;
  int valor_int;
  float valor_float;
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
struct expressao {
  int id_llvm;
  char *lexema;
  Tipo tipo;
  TipoSimbolo tipo_simb;
  int valor_int;
  float valor_float;
  struct tabela_simbolos *id_tabela;
};
struct lista_expressoes {
  struct expressao *exp;
  struct lista_expressoes *proximo;
};

// sem overload em ANSI C!
struct simbolo *novo_simbolo1(char *lexema);
struct simbolo *novo_simbolo2(char *lexema, TipoSimbolo tipo_simb);
struct simbolo *novo_simbolo3(char *lexema, TipoSimbolo tipo_simb, int escopo);
struct simbolo *novo_simbolo4(char *lexema, TipoSimbolo tipo_simb, int escopo,
                              Tipo tipo);
struct expressao *nova_expressao(char *lexema, TipoSimbolo tipo_simb);
struct expressao *nova_expressao_int(char *valor_int, TipoSimbolo tipo_simb);
struct expressao *nova_expressao_float(float valor_float, TipoSimbolo tipo_simb);
struct expressao *nova_expressao_operador_multiplicativo(struct expressao *esq,
                                                         struct expressao *dir, char *operador);
struct expressao *nova_expressao_operador_aditivo(struct expressao *esq,
                                                  struct expressao *dir, char *operador);
struct expressao *nova_expressao2(struct tabela_simbolos *ts, char *lexema, TipoSimbolo tipo_simb, int escopo);
struct expressao *executar_funcao(struct tabela_simbolos *ts, char *func_id, struct lista_expressoes *args);
struct lista_expressoes *insere_lista_expressoes(struct lista_expressoes *lista,
                                                 struct expressao *exp);

struct lista_simbolo *insere_lista_simbolo(struct lista_simbolo *lista,
                                           struct simbolo *simb);
struct lista_simbolo *concatena_lista_simbolos(struct lista_simbolo *lista1,
                                               struct lista_simbolo *lista2);
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
void materializa_simbolos(FILE *fp, struct lista_simbolo *lista, int *contador_simbolos);
void materializa_funcao(FILE *fp, struct lista_simbolo *args, struct simbolo *funcao, int *contador_simbolos);
void materializa_atribuicao(FILE *fp, struct expressao *esq, struct expressao *dir);
void imprime_tabela_simbolos(FILE *fp, struct tabela_simbolos *ts);
void imprime_lista_simbolos(FILE *fp, struct lista_simbolo *lista);
#endif