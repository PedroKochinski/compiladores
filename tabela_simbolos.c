
#include "tabela_simbolos.h"

#include <ctype.h>
#include <stdio.h>

#include "compilador.h"
FILE *fout = NULL;
struct simbolo *novo_simbolo1(char *lexema) {
  struct simbolo *novo = malloc(sizeof(struct simbolo));
  novo->lexema = strdup(lexema);
  return novo;
}

struct simbolo *novo_simbolo2(char *lexema, TipoSimbolo tipo_simb) {
  struct simbolo *novo = malloc(sizeof(struct simbolo));
  novo->lexema = strdup(lexema);
  novo->tipo_simb = tipo_simb;
  return novo;
}

struct simbolo *novo_simbolo3(char *lexema, TipoSimbolo tipo_simb, int escopo) {
  struct simbolo *novo = malloc(sizeof(struct simbolo));
  novo->lexema = strdup(lexema);
  novo->tipo_simb = tipo_simb;
  novo->escopo = escopo;
  return novo;
}

struct simbolo *novo_simbolo4(char *lexema, TipoSimbolo tipo_simb, int escopo, Tipo tipo) {
  struct simbolo *novo = malloc(sizeof(struct simbolo));
  novo->lexema = strdup(lexema);
  novo->tipo_simb = tipo_simb;
  novo->escopo = escopo;
  novo->tipo = tipo;
  return novo;
}

struct simbolo *novo_simbolo5(char *lexema, TipoSimbolo tipo_simb, int escopo, char *id_funcao) {
  struct simbolo *novo = malloc(sizeof(struct simbolo));
  novo->lexema = strdup(lexema);
  novo->tipo_simb = tipo_simb;
  novo->escopo = escopo;
  novo->id_funcao = id_funcao;
  return novo;
}

struct expressao *nova_expressao(char *lexema, TipoSimbolo tipo_simb) {
  struct expressao *novo = malloc(sizeof(struct expressao));
  struct simbolo *novo_simb = novo_simbolo2(lexema, tipo_simb);
  novo->lexema = strdup(lexema);
  novo->tipo_simb = tipo_simb;
  return novo;
}

struct expressao *nova_expressao2(struct tabela_simbolos *ts, char *lexema,
                                  TipoSimbolo tipo_simb, int escopo) {
  if (tipo_simb == NUMERO) {
    struct expressao *novo = malloc(sizeof(struct expressao));
    novo->lexema = strdup(lexema);
    novo->tipo_simb = tipo_simb;
    novo->valor_int = atoi(lexema);
    novo->valor_float = atof(lexema);
    novo->escopo = escopo;
    return novo;
  }
  struct simbolo *aux = busca_simbolo(ts, lexema);
  if (aux == NULL) {
    char erro[500];
    sprintf(erro, "simbolo '%s' nao declarado", lexema);
    yyerror(erro);
    exit(1);
  } else if (aux->escopo != escopo && aux->tipo_simb != FUNCAO) {
    char erro[500];
    sprintf(erro, "simbolo '%s' nao declarado nesse escopo", lexema);
    yyerror(erro);
    exit(1);
  }
  struct expressao *novo = malloc(sizeof(struct expressao));
  novo->lexema = strdup(lexema);
  novo->tipo_simb = tipo_simb;
  novo->escopo = escopo;
  return novo;
}

struct expressao *nova_expressao_int(char *valor_int, TipoSimbolo tipo_simb) {
  struct expressao *novo = malloc(sizeof(struct expressao));
  novo->valor_int = atoi(valor_int);
  novo->valor_float = atof(valor_int);
  novo->lexema = strdup(valor_int);
  novo->tipo_simb = tipo_simb;
  return novo;
}

struct expressao *nova_expressao_float(float valor_float, TipoSimbolo tipo_simb) {
  struct expressao *novo = malloc(sizeof(struct expressao));
  novo->valor_float = valor_float;
  novo->tipo_simb = tipo_simb;
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

void materializa_variavel_dinamica(FILE *fp, struct expressao *exp, int *contador_simbolos) {
  char *tipo_var = exp->tipo == INT ? "i32" : "float";
  char *tipo_ptr = exp->escopo == 0 ? "@" : "%";
  (*contador_simbolos)++;
  fprintf(fp, "\t%%%d = load %s, ptr %s%s\n", *contador_simbolos, tipo_var, tipo_ptr,exp->lexema);
  exp->id_llvm = *contador_simbolos;
}

// quando usar para retorno de funcao, o buffer deve ser o que deve ser retornado
void materializa_variavel(FILE *fp, char *buffer, struct expressao *expr, int *contador_simbolos) {
  fprintf(fp, "\t; materializando variavel %s (tipo_simb %d)\n", expr->lexema, expr->tipo_simb);
  if (expr->tipo_simb == PONTEIRO || (expr->tipo_simb == VARIAVEL && expr->escopo == 0)) {
    materializa_variavel_dinamica(fp, expr, contador_simbolos);
    sprintf(buffer, "%%%d", expr->id_llvm);
  }

  else if (expr->tipo_simb == NUMERO)
    sprintf(buffer, "%s", expr->lexema);

  else if (expr->tipo_simb == EXPR_SOMA)
    sprintf(buffer, "%%%d", expr->id_llvm);

  else if (expr->tipo_simb == FUNCAO) {
    char buffer_args[1000];
    char *buffer_args_ids = (char *)malloc(1000 * sizeof(char));
    char *args_func = (char *)malloc(1000 * sizeof(char));
    char *tipo_arg;
    (*contador_simbolos)++;
    fprintf(fp, "\t; chamando funcao %s\n", expr->lexema);
    while (expr->args) {
      int tipo_simb = expr->args->exp->tipo_simb;
      if(tipo_simb == INT) tipo_arg = "i32";
      else if(tipo_simb == FLOAT) tipo_arg = "float";
      materializa_variavel(fp, buffer_args, expr->args->exp, contador_simbolos);
      // fprintf(fp, "%s\n", buffer_args);
      sprintf(buffer_args_ids, "%s %s, ", expr->args->exp->tipo == INT ? "i32" : "float", buffer_args);
      strcat(args_func, buffer_args_ids);
      expr->args->exp->id_llvm = *contador_simbolos;
      (*contador_simbolos)++;
      expr->args = expr->args->proximo;
    }
    args_func[strlen(args_func) - 2] = '\0'; // remove a ultima virgula
    fprintf(fp, "\t%%%d = call %s @%s(%s)\n", *contador_simbolos, (expr->tipo == INT ? "i32" : "float"), expr->lexema, args_func);
    sprintf(buffer, "%%%d", *contador_simbolos);
  }

  else if (expr->tipo_simb == RETORNO) {
    char *tipo_var = expr->tipo == INT ? "i32" : "float";
    fprintf(fp, "\tstore %s %s, ptr %%%s\n", tipo_var, buffer, expr->lexema);
  }
}

void materializa_atribuicao(struct tabela_simbolos *ts, FILE *fp, struct expressao *esq, struct expressao *dir,
                            int *contador_simbolos) {
  if (esq->tipo != dir->tipo) {
    char erro[500];
    sprintf(erro, "atribuição entre tipos diferentes");
    yyerror(erro);
    exit(1);
  }
  char buffer_llvm[1000];
  char buffer_esq[1000];
  char buffer_dir[1000];
  char *tipo_var = esq->tipo == INT ? "i32" : "float";
  char *tipo_ptr = esq->escopo == 0 ? "@" : "%";
    materializa_variavel(fp, buffer_dir, dir, contador_simbolos);
    sprintf(buffer_llvm, "\tstore %s %s, ptr %s%s\n", tipo_var, buffer_dir, tipo_ptr, esq->lexema);
    fprintf(fp, "%s\n", buffer_llvm);
  
}

struct expressao *nova_expressao_operador_relacional(FILE *fp, struct tabela_simbolos *ts, struct expressao *esq, struct expressao *dir, char *operador_relacional, int *contador_simbolos) {
  struct expressao *novo = malloc(sizeof(struct expressao));
  char operador[500] = "";
  char buffer_esq[1000];
  char buffer_dir[1000];
  char *tipo_var = esq->tipo == INT ? "i32" : "float";
  char *tipo_icmp = esq->tipo == INT ? "i" : "f";
  if (strcmp(operador_relacional, "<") == 0)
    sprintf(operador, "slt");
  else if (strcmp(operador_relacional, "<=") == 0)
    sprintf(operador, "sle");
  else if (strcmp(operador_relacional, ">") == 0)
    sprintf(operador, "sgt");
  else if (strcmp(operador_relacional, ">=") == 0)
    sprintf(operador, "sge");
  else if (strcmp(operador_relacional, "=") == 0)
    sprintf(operador, "eq");
  else if (strcmp(operador_relacional, "<>") == 0)
    sprintf(operador, "ne");
  materializa_variavel(fp, buffer_esq, esq, contador_simbolos);
  materializa_variavel(fp, buffer_dir, dir, contador_simbolos);
  (*contador_simbolos)++;
  fprintf(fp, "\t%%%d = %scmp %s %s %s, %s\n\n", *contador_simbolos, tipo_icmp, operador, tipo_var, buffer_esq, buffer_dir);
  novo->id_llvm = *contador_simbolos;
  novo->tipo_simb = EXPR_OP_RELACIONAL;
  novo->escopo = 1;
  return novo;
}

struct expressao *nova_expressao_operador_aditivo_e_multiplicativo(FILE *fp, struct tabela_simbolos *ts, struct expressao *esq, struct expressao *dir, char *operador, int *contador_simbolos) {
  struct expressao *novo = malloc(sizeof(struct expressao));
  int tamanho = snprintf(NULL, 0, "add float %f, %f", esq->valor_float, dir->valor_float); // conta o tamanho da string a ser escrita
  char *saida_llvm = malloc(tamanho + 1);
  char buffer_esq[1000];
  char buffer_dir[1000];

  if (esq->tipo != dir->tipo) {
    char erro[500];
    sprintf(erro, "operacao '%s' entre tipos diferentes", operador);
    yyerror(erro);
    exit(1);
  }

  char *tipo_var = esq->tipo == INT ? "i32" : "float";

  materializa_variavel(fp, buffer_esq, esq, contador_simbolos);
  materializa_variavel(fp, buffer_dir, dir, contador_simbolos);

  if (strcmp(operador, "+") == 0) {
    (*contador_simbolos)++;
    sprintf(saida_llvm, "%%%d = add %s %s, %s", (*contador_simbolos), tipo_var, buffer_esq, buffer_dir);
    fprintf(fp, "\t%s\n", saida_llvm);
    novo->valor_int = esq->valor_int + dir->valor_int;
    novo->valor_float = esq->valor_float + dir->valor_float;
  } else if (strcmp(operador, "-") == 0) {
    (*contador_simbolos)++;
    sprintf(saida_llvm, "%%%d = sub %s %s, %s", (*contador_simbolos), tipo_var, buffer_esq, buffer_dir);
    fprintf(fp, "\t%s\n", saida_llvm);
    novo->valor_int = esq->valor_int - dir->valor_int;
    novo->valor_float = esq->valor_float - dir->valor_float;
  } else if (strcmp(operador, "*") == 0) {
    (*contador_simbolos)++;
    sprintf(saida_llvm, "%%%d = mul %s %s, %s", (*contador_simbolos), tipo_var, buffer_esq, buffer_dir);
    fprintf(fp, "\t%s\n", saida_llvm);
    novo->valor_int = esq->valor_int * dir->valor_int;
    novo->valor_float = esq->valor_float * dir->valor_float;
  } else if (strcmp(operador, "div") == 0) {
    (*contador_simbolos)++;
    sprintf(saida_llvm, "%%%d = div %s %s, %s", (*contador_simbolos), tipo_var, buffer_esq, buffer_dir);
    fprintf(fp, "\t%s\n", saida_llvm);
    novo->valor_int = esq->valor_int / dir->valor_int;
    novo->valor_float = novo->valor_int;
  } else if (strcmp(operador, "/") == 0) {
    (*contador_simbolos)++;
    sprintf(saida_llvm, "%%%d = fdiv %s %s, %s", (*contador_simbolos), tipo_var, buffer_esq, buffer_dir);
    fprintf(fp, "\t%s\n", saida_llvm);
    novo->valor_int = esq->valor_int / dir->valor_int;
    novo->valor_float = esq->valor_float / dir->valor_float;
  } else if (strcmp(operador, "mod") == 0) {
    (*contador_simbolos)++;
    sprintf(saida_llvm, "%%%d = srem %s %s, %s", (*contador_simbolos), tipo_var, buffer_esq, buffer_dir);
    fprintf(fp, "\t%s\n", saida_llvm);
    novo->valor_int = dir->valor_int > 0 ? esq->valor_int % dir->valor_int : 0;
    novo->valor_float = novo->valor_int;
  } else if (strcmp(operador, "and") == 0) {
    (*contador_simbolos)++;
    sprintf(saida_llvm, "%%%d = and %s %s, %s", (*contador_simbolos), tipo_var, buffer_esq, buffer_dir);
    fprintf(fp, "\t%s\n", saida_llvm);
  }
  else{
    char erro[500];
    sprintf(erro, "operador '%s' invalido", operador);
    yyerror(erro);
    exit(1);
  }
  novo->lexema = strdup(saida_llvm);
  novo->tipo = esq->tipo;
  novo->tipo_simb = EXPR_SOMA;
  novo->id_llvm = *contador_simbolos;
  novo->escopo = 1;
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
      sprintf(
          erro,
          "argumento '%s' da funcao '%s' tem tipo diferente do esperado",
          arg->lexema, func_id);
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
    sprintf(erro, "funcao '%s' recebeu mais argumentos do que o esperado",
            func_id);
    yyerror(erro);
    exit(1);
  }
}

struct expressao *executar_funcao(FILE *fp, struct tabela_simbolos *ts, char *func_id, struct lista_expressoes *args, int *contador_simbolos) {
  
  if(strcmp(func_id, "read") == 0 || strcmp(func_id, "write") == 0) {
    char tipo_arg[1000];
    char tipo_var[1000];
    char tipo_ptr[1000];
    char buffer[1000];
    while(args){
      sprintf(tipo_ptr, "%s", args->exp->escopo == 0 ? "@" : "%");
      sprintf(tipo_arg, "%s", args->exp->tipo == INT ? "int" : "float");
      sprintf(tipo_var, "%s", args->exp->tipo == INT ? "i32" : "float");
      (*contador_simbolos)++;
      if (strcmp(func_id, "read") == 0) {
        fprintf(fp, "\t%%%d = call i32 (ptr, ...) @__isoc99_scanf(ptr @read_%s, ptr %s%s)\n",(*contador_simbolos), tipo_arg, tipo_ptr, args->exp->lexema);
      } else {
        if(args->exp->tipo == FLOAT){
          (*contador_simbolos)++;
          fprintf(fp, "\t%%%d = fpext float %s to double\n", *contador_simbolos, buffer);
          (*contador_simbolos)++;
          fprintf(fp, "\t%%%d = call i32 (ptr, ...) @printf(ptr @write_float, double %%%d)\n", *contador_simbolos, *contador_simbolos - 1);
        } 
        else {
          materializa_variavel(fp, buffer, args->exp, contador_simbolos);
          (*contador_simbolos)++;
          fprintf(fp, "\t%%%d = call i32 (ptr, ...) @printf(ptr @write_int, i32 %s)\n\n", *contador_simbolos, buffer);
        }
      }
      args = args->proximo;
    }
    return NULL;
  }
  else tratar_erro_funcao(ts, func_id, args);
  struct simbolo *funcao = busca_simbolo(ts, func_id);
  struct expressao *novo = malloc(sizeof(struct expressao));
  // while(args){
  //   printf("args %s\n", args->exp->lexema);
  //   args = args->proximo;
  // }
  novo->lexema = strdup(func_id);
  novo->tipo_simb = FUNCAO;
  novo->tipo = funcao->tipo;
  novo->args = args;
  novo->escopo = funcao->escopo;
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
    if (strcmp(ts->simb->lexema, lexema) == 0) {
      return ts->simb;
    }
    ts = ts->proximo;
  }
  return NULL;
}

struct simbolo *busca_simbolo2(struct tabela_simbolos *ts, char *lexema, TipoSimbolo tipo_simb) {
  // imprime_tabela_simbolos(stdout, ts);
  while (ts != NULL) {
    printf("buscando %s = %s e %d = %d\n", ts->simb->lexema, lexema,
           ts->simb->tipo_simb, tipo_simb);
    if (strcmp(ts->simb->lexema, lexema) == 0 &&
        ts->simb->tipo_simb == tipo_simb)
      return ts->simb;
    ts = ts->proximo;
  }
  printf("nao achou %s\n", lexema);
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
    novo->lexema = strdup(args->simb->lexema);
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
  case REFERENCIA:
    fprintf(fp, "REFERENCIA");
    break;
  }
}
void imprime_funcao(FILE *fp, struct simbolo *func) {
  if (func->tipo_simb == FUNCAO)
    fprintf(fp, "FUNCAO; ");
  else
    fprintf(fp, "PROCEDURE; ");
  fprintf(fp, "lexema = %s; id_llvm = %d, escopo = %d; tipo = ", func->lexema,
          func->id_llvm, func->escopo);
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
  if (var->tipo_simb == NUMERO)
    fprintf(fp, "NUMERO; lexema = %s; escopo = %d; tipo = ", var->lexema,
            var->escopo);
  else
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

void materializa_funcao(FILE *fp, struct lista_simbolo *args,
                        struct simbolo *funcao, int *contador_simbolos) {
  (*contador_simbolos)++;
  funcao->id_llvm = *contador_simbolos;
  struct lista_simbolo aux = *args;
  if (funcao->tipo_simb == FUNCAO) {
    if (funcao->tipo == INT) {
      fprintf(fp, "define i32 @%s(", funcao->lexema);
      while (args != NULL) {
        (*contador_simbolos)++;
        args->simb->id_llvm = *contador_simbolos;
        args->simb->tipo_simb = PONTEIRO;
        fprintf(fp, "%s %%%d",
                args->simb->tipo == INT ? "i32" : "float",
                args->simb->id_llvm);
        args = args->proximo;
        if (args != NULL)
          fprintf(fp, ", ");
      }
      fprintf(fp, ") {\nentry:\n");
    } else if (funcao->tipo == FLOAT) {
      fprintf(fp, "define float @%s(", funcao->lexema);
      while (args != NULL) {
        (*contador_simbolos)++;
        args->simb->id_llvm = *contador_simbolos;
        args->simb->tipo_simb = PONTEIRO;
        fprintf(fp, "%s %%%d",
                args->simb->tipo == INT ? "i32" : "float",
                args->simb->id_llvm);
        args = args->proximo;
        if (args != NULL)
          fprintf(fp, ", ");
      }
      fprintf(fp, ") {\n");
    }
  }
  args = &aux;
  while (args != NULL) {
    fprintf(fp, "\t%%%s = alloca %s\n", args->simb->lexema,
            args->simb->tipo == INT ? "i32" : "float");
    fprintf(fp, "\tstore %s %%%d, ptr %%%s\n",
            args->simb->tipo == INT ? "i32" : "float", args->simb->id_llvm,
            args->simb->lexema);
    args = args->proximo;
  }
  fprintf(fp, "\t%%%s = alloca %s\n", funcao->lexema,
          funcao->tipo == INT ? "i32" : "float");
  fprintf(fp, "\n");
}

void materializa_simbolos(FILE *fp, struct lista_simbolo *lista,
                          int *contador_simbolos) {
  while (lista != NULL) {
    (*contador_simbolos)++;
    if (lista->simb->tipo_simb == VARIAVEL) {
      if (lista->simb->escopo ==
          0) { // variavel global alocada dinamicamente
        fprintf(fp, "@%s = global %s\n", lista->simb->lexema,
                lista->simb->tipo == INT ? "i32" : "float");
        lista->simb->id_llvm = *contador_simbolos;
      } else { // variavel local
        fprintf(fp, "\t%%%s = alloca %s\n", lista->simb->lexema,
                lista->simb->tipo == INT ? "i32" : "float");
        lista->simb->id_llvm = *contador_simbolos;
      }
    } else // retorno da funcao
      fprintf(fp, "simbolo %d: %s\n", *contador_simbolos,
              lista->simb->lexema);
    fprintf(fp, "\n");
    lista = lista->proximo;
  }
}

void materializa_simbolos_globais(FILE *fp, struct lista_simbolo *lista,
                                  int *contador_simbolos) {
  while (lista != NULL) {
    (*contador_simbolos)++;
    if (lista->simb->escopo == 0) { // variavel global alocada dinamicamente
      if(lista->simb->tipo == INT)
        fprintf(fp, "@%s = global i32 %d\n", lista->simb->lexema,
                lista->simb->valor_int);
      else
        fprintf(fp, "@%s = global float %f\n", lista->simb->lexema,
                lista->simb->valor_float);
      lista->simb->id_llvm = *contador_simbolos;
    }
    lista = lista->proximo;
  }
  fprintf(fp, "\n");
}