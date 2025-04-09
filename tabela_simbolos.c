
#include "tabela_simbolos.h"

#include "compilador.h"

struct simbolo *novo_simbolo1(char *lexema) {
  struct simbolo *novo = malloc(sizeof(struct simbolo));
  novo->lexema = lexema;
  return novo;
}

struct simbolo *novo_simbolo2(char *lexema, TipoSimbolo tipo_simb) {
  struct simbolo *novo = malloc(sizeof(struct simbolo));
  novo->lexema = lexema;
  novo->tipo_simb = tipo_simb;
  return novo;
}

struct simbolo *novo_simbolo3(char *lexema, TipoSimbolo tipo_simb, int escopo) {
  struct simbolo *novo = malloc(sizeof(struct simbolo));
  novo->lexema = lexema;
  novo->tipo_simb = tipo_simb;
  novo->escopo = escopo;
  return novo;
}

struct simbolo *novo_simbolo4(char *lexema, TipoSimbolo tipo_simb, int escopo, Tipo tipo) {
  struct simbolo *novo = malloc(sizeof(struct simbolo));
  novo->lexema = lexema;
  novo->tipo_simb = tipo_simb;
  novo->escopo = escopo;
  novo->tipo = tipo;
  return novo;
}

struct simbolo *novo_simbolo5(char *lexema, TipoSimbolo tipo_simb, int escopo, char *id_funcao) {
  struct simbolo *novo = malloc(sizeof(struct simbolo));
  novo->lexema = lexema;
  novo->tipo_simb = tipo_simb;
  novo->escopo = escopo;
  novo->id_funcao = id_funcao;
  return novo;
}

struct expressao *nova_expressao(char *lexema, TipoSimbolo tipo_simb) {
  struct expressao *novo = malloc(sizeof(struct expressao));
  struct simbolo *novo_simb = novo_simbolo2(lexema, tipo_simb);
  novo->lexema = lexema;
  novo->tipo_simb = tipo_simb;
  return novo;
}

struct expressao *nova_expressao2(struct tabela_simbolos *ts, char *lexema, TipoSimbolo tipo_simb, int escopo) {
  struct simbolo *aux = busca_simbolo(ts, lexema);
  if (aux == NULL) {
    char erro[500];
    sprintf(erro, "simbolo '%s' nao declarado", lexema);
    yyerror(erro);
    exit(1);
  } else if (aux->escopo != escopo) {
    char erro[500];
    sprintf(erro, "simbolo '%s' nao declarado nesse escopo", lexema);
    yyerror(erro);
    exit(1);
  }
  struct expressao *novo = malloc(sizeof(struct expressao));
  novo->lexema = lexema;
  novo->tipo_simb = tipo_simb;
  return novo;
}

struct expressao *nova_expressao_int(char *valor_int, TipoSimbolo tipo_simb) {
  struct expressao *novo = malloc(sizeof(struct expressao));
  novo->valor_int = atoi(valor_int);
  novo->valor_float = atof(valor_int);
  novo->lexema = valor_int;
  novo->tipo_simb = tipo_simb;
  return novo;
}
struct expressao *nova_expressao_float(float valor_float, TipoSimbolo tipo_simb) {
  struct expressao *novo = malloc(sizeof(struct expressao));
  novo->valor_float = valor_float;
  novo->tipo_simb = tipo_simb;
  return novo;
}

struct expressao *nova_expressao_operador_multiplicativo(struct expressao *esq, struct expressao *dir, char *operador) {
  struct expressao *novo = malloc(sizeof(struct expressao));
  novo->lexema = strcat(
      strcat(strcat(strcat(esq->lexema, " "), operador), " "), dir->lexema);
  if (strcmp(operador, "*") == 0) {
    novo->valor_int =
        esq->valor_int * dir->valor_int;   // verificar se os tipos batem
  } else if (strcmp(operador, "/") == 0) { // Em C, se ambos os operandos forem int, o resultado é // inteiro
    novo->valor_float = esq->valor_float / dir->valor_float;
  } else if (strcmp(operador, "mod") == 0) {
    novo->valor_int = esq->valor_int % dir->valor_int;
  } else if (strcmp(operador, "div") == 0) {
    novo->valor_int =
        esq->valor_int /
        dir->valor_int;                      // Mesmo operador, mas só funciona como div se os
                                             // operandos forem inteiros
  } else if (strcmp(operador, "and") == 0) { // bitwise and
    novo->valor_int = esq->valor_int & dir->valor_int;
  } else {
    char erro[500];
    sprintf(erro, "operador '%s' invalido", operador);
    yyerror(erro);
    exit(1);
  }
  return novo;
}

struct simbolo *busca_simbolo_llvm(struct tabela_simbolos *ts, int id_llvm) {
  while (ts != NULL) {
    if (ts->simb->id_llvm == id_llvm)
      return ts->simb;
    ts = ts->proximo;
  }
  return NULL;
}

struct expressao *nova_expressao_operador_aditivo(struct expressao *esq, struct expressao *dir, char *operador) {
  struct expressao *novo = malloc(sizeof(struct expressao));
  int tamanho = snprintf(NULL, 0, "add float %.2f %.2f", esq->valor_float, dir->valor_float); // conta o tamanho da string a ser escrita
  char *saida_llvm = malloc(tamanho + 1);
  
  if (esq->tipo == INT && dir->tipo == INT) {
    
    sprintf(saida_llvm, "add i32 %s %s", esq->lexema, dir->lexema);
    novo->lexema = saida_llvm;
    novo->valor_int = esq->valor_int + dir->valor_int;
  } else if (esq->tipo == FLOAT && dir->tipo == FLOAT) {
    sprintf(saida_llvm, "add float %f %f", esq->valor_float, dir->valor_float);
    novo->lexema = saida_llvm;
  } else {
    char erro[500];
    sprintf(erro, "operacao '%s' entre tipos diferentes", operador);
    yyerror(erro);
    exit(1);
  }
  if (strcmp(operador, "+") == 0) {
    novo->valor_int = esq->valor_int + dir->valor_int; // verificar se os tipos batem
    novo->valor_float = esq->valor_float + dir->valor_float;
  } else if (strcmp(operador, "-") == 0) {
    novo->valor_int = esq->valor_int - dir->valor_int;
  } else if (strcmp(operador, "or") == 0) { // bitwise or
    novo->valor_int = esq->valor_int | dir->valor_int;
  } else {
    char erro[500];
    sprintf(erro, "operador '%s' invalido", operador);
    yyerror(erro);
    exit(1);
  }
  return novo;
}

struct lista_expressoes *insere_lista_expressoes(struct lista_expressoes *lista, struct expressao *exp) {
  // insere no final
  struct lista_expressoes *aux,
      *novo = malloc(sizeof(struct lista_expressoes));
  novo->exp = exp;
  novo->proximo = NULL;
  if (lista == NULL) {
    return novo;
  }
  aux = lista;
  while (aux->proximo != NULL)
    aux = aux->proximo;
  aux->proximo = novo;
  return lista;
}

struct lista_simbolo *insere_lista_simbolo(struct lista_simbolo *lista, struct simbolo *simb) {
  // insere no final
  struct lista_simbolo *aux, *novo = malloc(sizeof(struct lista_simbolo));
  novo->simb = simb;
  novo->proximo = NULL;
  if (lista == NULL) {
    return novo;
  }
  aux = lista;
  while (aux->proximo != NULL)
    aux = aux->proximo;
  aux->proximo = novo;
  return lista;
}

void tratar_erro_funcao(struct tabela_simbolos *ts, char *func_id, struct lista_expressoes *args) {
  struct simbolo *funcao = busca_simbolo(ts, func_id);
  if (funcao == NULL) {
    char erro[500];
    sprintf(erro, "funcao '%s' nao declarada", func_id);
    yyerror(erro);
    exit(1);
  }
  if (funcao->args == NULL && args != NULL) {
    char erro[500];
    sprintf(erro, "funcao '%s' nao aceita argumentos", func_id);
    yyerror(erro);
    exit(1);
  }
  struct lista_args *arg = funcao->args;
  struct lista_expressoes *arg2 = args;
  while (arg != NULL && arg2 != NULL) {
    if (arg->tipo != arg2->exp->tipo) {
      char erro[500];
      sprintf(erro, "argumento '%s' da funcao '%s' tem tipo diferente do esperado", arg->lexema, func_id);
      yyerror(erro);
      exit(1);
    }
    arg = arg->proximo;
    arg2 = arg2->proximo;
  }
  if (arg != NULL) {
    char erro[500];
    sprintf(erro, "funcao '%s' espera mais argumentos", func_id);
    yyerror(erro);
    exit(1);
  }

  if (arg2 != NULL) {
    char erro[500];
    sprintf(erro, "funcao '%s' recebeu mais argumentos do que o esperado", func_id);
    yyerror(erro);
    exit(1);
  }
}

struct expressao *executar_funcao(struct tabela_simbolos *ts, char *func_id, struct lista_expressoes *args) {
  tratar_erro_funcao(ts, func_id, args);
  struct simbolo *funcao = busca_simbolo(ts, func_id);
  struct expressao *novo = malloc(sizeof(struct expressao));
  while (args != NULL) {
    funcao->args->lexema = args->exp->lexema;
    funcao->args->tipo = args->exp->tipo;
    args = args->proximo;
    funcao->args = funcao->args->proximo;
  }
  return novo;
}

void atualiza_tipo_simbolos(struct lista_simbolo *lista, Tipo t) {
  while (lista != NULL) {
    lista->simb->tipo = t;
    lista = lista->proximo;
  }
}

void free_lista_simbolo(struct lista_simbolo *lista) {
  if (lista == NULL)
    return;
  free_lista_simbolo(lista->proximo);
  free(lista);
}

struct lista_simbolo *concatena_lista_simbolos(struct lista_simbolo *lista1, struct lista_simbolo *lista2) {
  if (lista1 == NULL)
    return lista2;
  struct lista_simbolo *aux = lista1;
  while (aux->proximo != NULL)
    aux = aux->proximo;
  aux->proximo = lista2;
  return lista1;
}

struct tabela_simbolos *insere_simbolo_ts(struct tabela_simbolos *ts, struct simbolo *simb) {
  struct simbolo *simb_busca = busca_simbolo(ts, simb->lexema);
  if (simb_busca != NULL && simb_busca->escopo == simb->escopo &&
      strcmp(simb_busca->id_funcao, simb->id_funcao) == 0) {
    char erro[500];
    sprintf(erro, "simbolo '%s' ja declarado antes", simb->lexema);
    yyerror(erro);
  }
  struct tabela_simbolos *novo = malloc(sizeof(struct tabela_simbolos));
  novo->simb = simb;
  novo->proximo = ts;
  return novo;
}
struct tabela_simbolos *insere_simbolos_ts(struct tabela_simbolos *ts, struct lista_simbolo *lista) {
  struct lista_simbolo *aux = lista;
  while (aux != NULL) {
    ts = insere_simbolo_ts(ts, aux->simb);
    aux = aux->proximo;
  }
  free_lista_simbolo(lista);
  return ts;
}

struct simbolo *busca_simbolo(struct tabela_simbolos *ts, char *lexema) {
  while (ts != NULL) {
    if (strcmp(ts->simb->lexema, lexema) == 0)
      return ts->simb;
    ts = ts->proximo;
  }
  return NULL;
}

struct tabela_simbolos *remove_simbolos(struct tabela_simbolos *ts, int escopo) {
  struct tabela_simbolos *aux;
  while (ts != NULL && ts->simb->escopo == escopo) {
    free(ts->simb);
    aux = ts;
    ts = ts->proximo;
    free(aux);
  }
  return ts;
}

void insere_func_args(struct simbolo *funcao, struct lista_simbolo *args) {
  struct lista_args *novo, *ultimo;
  while (args != NULL) {
    novo = malloc(sizeof(struct lista_args));
    novo->tipo = args->simb->tipo;
    novo->lexema = args->simb->lexema;
    novo->proximo = NULL;
    if (funcao->args == NULL)
      funcao->args = novo;
    else
      ultimo->proximo = novo;
    ultimo = novo;
    args = args->proximo;
  }
}

void imprime_tipo(FILE *fp, Tipo tipo) {
  switch (tipo) {
  case INT:
    fprintf(fp, "INTEIRO");
    break;
  case FLOAT:
    fprintf(fp, "REAL");
    break;
  case VAZIO:
    fprintf(fp, "VAZIO");
    break;
  }
}
void imprime_funcao(FILE *fp, struct simbolo *func) {
  if (func->tipo_simb == FUNCAO)
    fprintf(fp, "FUNCAO; ");
  else
    fprintf(fp, "PROCEDURE; ");
  fprintf(fp, "lexema = %s; id_llvm = %d, escopo = %d; tipo = ", func->lexema, func->id_llvm, func->escopo);
  imprime_tipo(fp, func->tipo);
  fprintf(fp, "; args:{");
  struct lista_args *args = func->args;
  while (args != NULL) {
    fprintf(fp, "%s: ", args->lexema);
    imprime_tipo(fp, args->tipo);
    if (args->proximo != NULL) {
      fprintf(fp, ", ");
    }
    args = args->proximo;
  }
  fprintf(fp, "}\n");
}
void imprime_variavel(FILE *fp, struct simbolo *var) {
  fprintf(fp, "VARIAVEL; lexema = %s; escopo = %d; tipo = ", var->lexema,
          var->escopo);
  imprime_tipo(fp, var->tipo);
  fprintf(fp, "; id_llvm = %d", var->id_llvm);
  if (var->id_funcao != NULL)
    fprintf(fp, "; id_funcao = %s", var->id_funcao);
  else
    fprintf(fp, "; id_funcao = NULL");
  fprintf(fp, "\n");
}
void imprime_tabela_simbolos(FILE *fp, struct tabela_simbolos *ts) {
  fprintf(fp, "--------------TS--------------\n");
  while (ts != NULL) {
    if (ts->simb->tipo_simb == FUNCAO || ts->simb->tipo_simb == PROC)
      imprime_funcao(fp, ts->simb);
    else
      imprime_variavel(fp, ts->simb);
    ts = ts->proximo;
    fprintf(fp, "---------------\n");
  }
  fprintf(fp, "--------------FIM-TS----------\n");
}

void imprime_lista_simbolos(FILE *fp, struct lista_simbolo *lista) {
  fprintf(fp, "--------------LISTA SIMBOLOS--------------\n");
  while (lista != NULL) {
    if (lista->simb->tipo_simb == FUNCAO || lista->simb->tipo_simb == PROC)
      imprime_funcao(fp, lista->simb);
    else
      imprime_variavel(fp, lista->simb);
    lista = lista->proximo;
    fprintf(fp, "---------------\n");
  }
  fprintf(fp, "--------------FIM-LISTA SIMBOLOS----------\n");
}

void materializa_atribuicao(FILE *fp, struct expressao *esq, struct expressao *dir) {
  struct simbolo *simb_esq = busca_simbolo(esq->id_tabela, esq->lexema);
  if (simb_esq == NULL) {
    char erro[500];
    sprintf(erro, "simbolo '%s' nao declarado", esq->lexema);
    yyerror(erro);
    exit(1);
  }
  if (esq->tipo != dir->tipo) {
    char erro[500];
    sprintf(erro, "atribuição entre tipos diferentes");
    yyerror(erro);
    exit(1);
  }
  char buffer_llvm[500];
  sprintf(buffer_llvm, "\t%%%s = %s", simb_esq->lexema, dir->lexema);
  fprintf(fp, "%s\n", buffer_llvm);
}

void materializa_funcao(FILE *fp, struct lista_simbolo *args, struct simbolo *funcao, int *contador_simbolos) {
  (*contador_simbolos)++;
  funcao->id_llvm = *contador_simbolos;
  struct lista_args *aux = funcao->args;
  if (funcao->tipo_simb == FUNCAO) {
    if (funcao->tipo == INT) {
      fprintf(fp, "define i32 @%s(", funcao->lexema);
      while (args != NULL) {
        (*contador_simbolos)++;
        args->simb->id_llvm = *contador_simbolos;
        fprintf(fp, "%s %%%s", args->simb->tipo == INT ? "i32" : "float", args->simb->lexema);
        args = args->proximo;
        if (args != NULL)
          fprintf(fp, ", ");
      }
      fprintf(fp, ") {\n");
    } else if (funcao->tipo == FLOAT) {
      fprintf(fp, "define float @%s(", funcao->lexema);
      while (args != NULL) {
        (*contador_simbolos)++;
        args->simb->id_llvm = *contador_simbolos;
        fprintf(fp, "%s %%%s", args->simb->tipo == INT ? "i32" : "float", args->simb->lexema);
        args = args->proximo;
        if (args != NULL)
          fprintf(fp, ", ");
      }
      fprintf(fp, ") {\n");
    }
  }
  funcao->args = aux;
  fprintf(fp, "\n");
}

void materializa_simbolos(FILE *fp, struct lista_simbolo *lista,
                          int *contador_simbolos) {
  while (lista != NULL) {
    (*contador_simbolos)++;
    if (lista->simb->tipo_simb == VARIAVEL) {
      if (lista->simb->tipo == INT) {
        if (lista->simb->escopo == 0) { // variavel global alocada dinamicamente
          fprintf(fp, "@%d = global i32 %d\n", *contador_simbolos, lista->simb->valor_int);
          lista->simb->id_llvm = *contador_simbolos;
        } else { // variavel local
          fprintf(fp, "\t%%%s = i32, %d\n", lista->simb->lexema, lista->simb->valor_int);
          lista->simb->id_llvm = *contador_simbolos;
        }

      } else if (lista->simb->tipo == FLOAT)
        fprintf(fp, "simbolo %d: %s\n", *contador_simbolos, lista->simb->lexema);
    } else // retorno da funcao
      fprintf(fp, "simbolo %d: %s\n", *contador_simbolos, lista->simb->lexema);
    fprintf(fp, "\n");
    lista = lista->proximo;
  }
}