
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

struct simbolo *novo_simbolo4(char *lexema, TipoSimbolo tipo_simb, int escopo,
                              Tipo tipo) {
    struct simbolo *novo = malloc(sizeof(struct simbolo));
    novo->lexema = strdup(lexema);
    novo->tipo_simb = tipo_simb;
    novo->escopo = escopo;
    novo->tipo = tipo;
    return novo;
}

struct simbolo *novo_simbolo5(char *lexema, TipoSimbolo tipo_simb, int escopo,
                              char *id_funcao) {
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
struct expressao *nova_expressao_float(float valor_float,
                                       TipoSimbolo tipo_simb) {
    struct expressao *novo = malloc(sizeof(struct expressao));
    novo->valor_float = valor_float;
    novo->tipo_simb = tipo_simb;
    return novo;
}

struct expressao *nova_expressao_operador_multiplicativo(struct expressao *esq,
                                                         struct expressao *dir,
                                                         char *operador) {
    struct expressao *novo = malloc(sizeof(struct expressao));
    novo->lexema = strdup(strcat(
        strcat(strcat(strcat(esq->lexema, " "), operador), " "), dir->lexema));
    novo->tipo_simb = esq->tipo_simb;
    if (strcmp(operador, "*") == 0) {
        novo->valor_int =
            esq->valor_int * dir->valor_int;  // verificar se os tipos batem
    } else if (strcmp(operador, "/") ==
               0) {  // Em C, se ambos os operandos forem int, o resultado é //
                     // inteiro
        novo->valor_float = esq->valor_float / dir->valor_float;
    } else if (strcmp(operador, "mod") == 0) {
        novo->valor_int = esq->valor_int % dir->valor_int;
    } else if (strcmp(operador, "div") == 0) {
        novo->valor_int = esq->valor_int / dir->valor_int;

    } else if (strcmp(operador, "and") == 0) {
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
        if (ts->simb->id_llvm == id_llvm) return ts->simb;
        ts = ts->proximo;
    }
    return NULL;
}

void materializa_variavel_dinamica(FILE *fp, struct expressao *exp,
                                   int *contador_simbolos) {
    char *tipo_var = exp->tipo == INT ? "i32" : "float";
    (*contador_simbolos)++;
    fprintf(fp, "\t%%%d = load %s ptr %%%s\n", *contador_simbolos, tipo_var,
            exp->lexema);
    exp->id_llvm = *contador_simbolos;
}

void materializa_variavel(FILE *fp, char* buffer, struct expressao *expr, int *contador_simbolos) {
    if (expr->tipo_simb == PONTEIRO || expr->escopo == 0) {
        materializa_variavel_dinamica(fp, expr, contador_simbolos);
        sprintf(buffer, "%%%d", expr->id_llvm);
    } else if (expr->tipo_simb == NUMERO)
        sprintf(buffer, "%s", expr->lexema);
    else if (expr->tipo_simb == EXPR_SOMA)
    sprintf(buffer, "%%%d", expr->id_llvm);
      
}

struct expressao *nova_expressao_operador_relacional(FILE *fp, struct tabela_simbolos *ts, struct expressao *esq, struct expressao *dir, char *operador_relacional, int *contador_simbolos) {
    struct expressao *novo = malloc(sizeof(struct expressao));
    char operador[500] = "";
    char buffer_esq[1000];
    char buffer_dir[1000];
    char *tipo_var = esq->tipo == INT ? "i32" : "float";
    char *tipo_icmp = esq->tipo == INT ? "i" : "f";
    if(strcmp(operador_relacional, "<") == 0) sprintf(operador, "slt");
    else if(strcmp(operador_relacional, "<=") == 0) sprintf(operador, "sle");
    else if(strcmp(operador_relacional, ">") == 0) sprintf(operador, "sgt");
    else if(strcmp(operador_relacional, ">=") == 0) sprintf(operador, "sge");
    else if(strcmp(operador_relacional, "=") == 0) sprintf(operador, "eq");
    else if(strcmp(operador_relacional, "<>") == 0) sprintf(operador, "ne");
    materializa_variavel(fp, buffer_esq, esq, contador_simbolos);
    materializa_variavel(fp, buffer_dir, dir, contador_simbolos);
    (*contador_simbolos)++;
    fprintf(fp, "\t%%%d = %scmp %s %s %s, %s\n\n", *contador_simbolos, tipo_icmp, operador, tipo_var, buffer_esq, buffer_dir);
    novo->id_llvm = *contador_simbolos;
    novo->tipo_simb = EXPR_OP_RELACIONAL;
    novo->escopo = 1;
    return novo;
}

struct expressao *nova_expressao_operador_aditivo( FILE *fp, struct tabela_simbolos *ts, struct expressao *esq, struct expressao *dir, char *operador, int *contador_simbolos) {
    struct expressao *novo = malloc(sizeof(struct expressao));
    int tamanho = snprintf(NULL, 0, "add float %.2f, %.2f", esq->valor_float, dir->valor_float);  // conta o tamanho da string a ser escrita
    char *saida_llvm = malloc(tamanho + 1);
    char buffer_esq[1000];
    char buffer_dir[1000];
    struct simbolo *simbolo_esq = busca_simbolo(ts, esq->lexema);
    struct simbolo *simbolo_dir = busca_simbolo(ts, dir->lexema);

    if (esq->tipo == dir->tipo) {
        if (esq->tipo_simb == VARIAVEL || esq->tipo_simb == PONTEIRO) {
            if (simbolo_esq->escopo == 0 ||
                simbolo_esq->tipo_simb == PONTEIRO) {
                (*contador_simbolos)++;
                fprintf(fp, "\t%%%d = load %s, ptr %%%s\n", *contador_simbolos,
                        (esq->tipo == INT ? "i32" : "float"), esq->lexema);
                sprintf(buffer_esq, "%%%d", *contador_simbolos);
            } else if (esq->tipo_simb == NUMERO)
                sprintf(buffer_esq, "%%%s", esq->lexema);
        } else if (esq->tipo_simb == NUMERO)
            sprintf(buffer_esq, "%s", esq->lexema);
        else if (esq->tipo_simb == EXPR_SOMA)
            sprintf(buffer_esq, "%%%d", esq->id_llvm);
        // printf("dir->lexema %s dir->tipo_simb %d\n", dir->lexema,
        // dir->tipo_simb);
        if (dir->tipo_simb == VARIAVEL || dir->tipo_simb == PONTEIRO) {
            if (simbolo_dir->escopo == 0 ||
                simbolo_dir->tipo_simb == PONTEIRO) {
                (*contador_simbolos)++;
                fprintf(fp, "\t%%%d = load %s, ptr %%%s\n", *contador_simbolos,
                        (dir->tipo == INT ? "i32" : "float"), dir->lexema);
                sprintf(buffer_dir, "%%%d", *contador_simbolos);
            } else
                sprintf(buffer_dir, "%%%s", dir->lexema);
        } else if (dir->tipo_simb == NUMERO)
            sprintf(buffer_dir, "%s", dir->lexema);
        else if (dir->tipo_simb == FUNCAO) {
            (*contador_simbolos)++;
            sprintf(buffer_dir, "%%%d", dir->id_llvm);
            char buffer_args[1000];
            struct lista_expressoes *args = dir->args;
            while (args) {
                printf("args %s\n", args->exp->lexema);
                args = args->proximo;
            }
            fprintf(fp, "\t%%%d = call %s @%s()\n", *contador_simbolos,
                    (dir->tipo == INT ? "i32" : "float"), dir->lexema);
            // sprintf(buffer_dir, "store %s %%%d, ptr %%%s", dir->tipo == INT ?
            // "i32" : "float", esq->lexema, *contador_simbolos); fprintf(fp,
            // "%s\n", buffer_dir);
        } else if (dir->tipo_simb == EXPR_SOMA)
            sprintf(buffer_dir, "%%%d", dir->id_llvm);
        if (strcmp(operador, "+") == 0) {
            (*contador_simbolos)++;
            sprintf(saida_llvm, "%%%d = add %s %s, %s", (*contador_simbolos),
                    (esq->tipo == INT ? "i32" : "float"), buffer_esq,
                    buffer_dir);
            novo->valor_int = esq->valor_int + dir->valor_int;
            novo->valor_float = esq->valor_float + dir->valor_float;
            fprintf(fp, "\t%s\n", saida_llvm);

        } else if (strcmp(operador, "-") == 0) {
            (*contador_simbolos)++;
            sprintf(saida_llvm, "%%%d = sub %s %s, %s", (*contador_simbolos),
                    (esq->tipo == INT ? "i32" : "float"), buffer_esq,
                    buffer_dir);
            novo->valor_int = esq->valor_int - dir->valor_int;
            novo->valor_float = esq->valor_float - dir->valor_float;
            fprintf(fp, "\t%s\n", saida_llvm);
        } else {
            char erro[500];
            sprintf(erro, "operador '%s' invalido", operador);
            yyerror(erro);
            exit(1);
        }
    } else {
        char erro[500];
        sprintf(erro, "operacao '%s' entre tipos diferentes", operador);
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

struct lista_expressoes *insere_lista_expressoes(struct lista_expressoes *lista,
                                                 struct expressao *exp) {
    // insere no final
    struct lista_expressoes *aux,
        *novo = malloc(sizeof(struct lista_expressoes));
    novo->exp = exp;
    novo->proximo = NULL;
    if (lista == NULL) {
        return novo;
    }
    aux = lista;
    while (aux->proximo != NULL) aux = aux->proximo;
    aux->proximo = novo;
    return lista;
}

struct lista_simbolo *insere_lista_simbolo(struct lista_simbolo *lista,
                                           struct simbolo *simb) {
    // insere no final
    struct lista_simbolo *aux, *novo = malloc(sizeof(struct lista_simbolo));
    novo->simb = simb;
    novo->proximo = NULL;
    if (lista == NULL) {
        return novo;
    }
    aux = lista;
    while (aux->proximo != NULL) aux = aux->proximo;
    aux->proximo = novo;
    return lista;
}

void tratar_erro_funcao(struct tabela_simbolos *ts, char *func_id,
                        struct lista_expressoes *args) {
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

struct expressao *executar_funcao(FILE *fp, struct tabela_simbolos *ts,
                                  char *func_id, struct lista_expressoes *args,
                                  int *contador_simbolos) {
    tratar_erro_funcao(ts, func_id, args);
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
    if (lista == NULL) return;
    free_lista_simbolo(lista->proximo);
    free(lista);
}

struct lista_simbolo *concatena_lista_simbolos(struct lista_simbolo *lista1,
                                               struct lista_simbolo *lista2) {
    if (lista1 == NULL) return lista2;
    struct lista_simbolo *aux = lista1;
    while (aux->proximo != NULL) aux = aux->proximo;
    aux->proximo = lista2;
    return lista1;
}

struct tabela_simbolos *insere_simbolo_ts(struct tabela_simbolos *ts,
                                          struct simbolo *simb) {
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
struct tabela_simbolos *insere_simbolos_ts(struct tabela_simbolos *ts,
                                           struct lista_simbolo *lista) {
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

struct simbolo *busca_simbolo2(struct tabela_simbolos *ts, char *lexema,
                               TipoSimbolo tipo_simb) {
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

struct tabela_simbolos *remove_simbolos(struct tabela_simbolos *ts,
                                        int escopo) {
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
    if (var->tipo == NUMERO)
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

void materializa_atribuicao(struct tabela_simbolos *ts, FILE *fp,
                            struct expressao *esq, struct expressao *dir,
                            int *contador_simbolos) {
    if (esq->tipo != dir->tipo) {
        char erro[500];
        sprintf(erro, "atribuição entre tipos diferentes");
        yyerror(erro);
        exit(1);
    }
    char buffer_llvm[1000];
    printf("esq->lexema %s esq->tipo_simb %d\n", esq->lexema, esq->tipo_simb);
    printf("dir->lexema %s dir->tipo_simb %d\n", dir->lexema, dir->tipo_simb);
    // jogar valor da expr dir na variavel temporaria e entao fazer o store na
    // variavel da esquerda printf("lexema da expr dir: %s %d\n", dir->lexema,
    // dir->tipo_simb); sprintf(buffer_llvm, "\t%%%d = %d", *contador_simbolos,
    // dir->escopo); fprintf(fp, "%s\n", buffer_llvm);
    if (esq->tipo_simb == RETORNO) {
        if (dir->tipo_simb == FUNCAO) {
            (*contador_simbolos)++;
            char buffer_args[1000];
            while (dir->args) {
                if (dir->args->exp->tipo_simb == NUMERO) {
                }
                printf("dir->args %s %d\n", dir->args->exp->lexema,
                       dir->args->exp->tipo_simb);
                sprintf(buffer_args, "%%%d = add %s 0, %d", *contador_simbolos,
                        dir->args->exp->tipo == INT ? "i32" : "float",
                        dir->args->exp->valor_int);
                fprintf(fp, "%s\n", buffer_args);

                dir->args->exp->id_llvm = *contador_simbolos;
                (*contador_simbolos)++;
                dir->args = dir->args->proximo;
            }
            sprintf(buffer_llvm, "\t%%%d = call %s @%s()\n", *contador_simbolos,
                    (dir->tipo == INT ? "i32" : "float"), dir->lexema);
            fprintf(fp, "%s\n", buffer_llvm);
            sprintf(buffer_llvm, "\tret %s %%%d\n",
                    esq->tipo == INT ? "i32" : "float", *contador_simbolos);
            fprintf(fp, "%s\n", buffer_llvm);
        } else if (dir->tipo_simb == PONTEIRO || dir->escopo == 0) {
            (*contador_simbolos)++;
            sprintf(buffer_llvm, "\t%%%d = load %s, ptr %%%s",
                    *contador_simbolos, dir->tipo == INT ? "i32" : "float",
                    dir->lexema);
            fprintf(fp, "%s\n", buffer_llvm);
            sprintf(buffer_llvm, "\tret %s %%%d\n",
                    esq->tipo == INT ? "i32" : "float", *contador_simbolos);
            fprintf(fp, "%s\n", buffer_llvm);
        } else if (dir->tipo_simb == NUMERO) {
            sprintf(buffer_llvm, "\tstore %s %f, %%%s",
                    dir->tipo == INT ? "i32" : "float",
                    dir->tipo == INT ? dir->valor_int : dir->valor_float,
                    esq->lexema);
            fprintf(fp, "%s\n", buffer_llvm);
            sprintf(buffer_llvm, "\tret %s %%%s\n",
                    esq->tipo == INT ? "i32" : "float", esq->lexema);
            fprintf(fp, "%s\n", buffer_llvm);
        } else if (dir->tipo_simb == EXPR_SOMA) {
            sprintf(buffer_llvm, "\tstore %s %%%d, ptr %%%s",
                    dir->tipo == INT ? "i32" : "float", dir->id_llvm,
                    esq->lexema);
            fprintf(fp, "%s\n", buffer_llvm);
            sprintf(buffer_llvm, "\tret %s %%%s\n",
                    esq->tipo == INT ? "i32" : "float", esq->lexema);
            fprintf(fp, "%s\n", buffer_llvm);
        } else {
            sprintf(buffer_llvm, "\tret %s %%%s\n",
                    esq->tipo == INT ? "i32" : "float", esq->lexema);
            fprintf(fp, "%s\n", buffer_llvm);
        }
    } else if (esq->tipo_simb == PONTEIRO || esq->tipo_simb == VARIAVEL || esq->escopo == 0) {
        if (dir->tipo_simb == FUNCAO) {
            char buffer_args[1000];
            char *buffer_args_ids = (char *)malloc(1000 * sizeof(char));
            char *args_func = (char *)malloc(1000 * sizeof(char));
            (*contador_simbolos)++;
            while (dir->args) {
                if (dir->args->exp->tipo_simb == NUMERO) {
                    sprintf(buffer_args, "\t%%%d = add %s 0, %d",
                            *contador_simbolos,
                            dir->args->exp->tipo == INT ? "i32" : "float",
                            dir->args->exp->valor_int);
                }
                fprintf(fp, "%s\n", buffer_args);
                sprintf(buffer_args_ids, "%s %%%d, ",
                        dir->args->exp->tipo == INT ? "i32" : "float",
                        *contador_simbolos);
                strcat(args_func, buffer_args_ids);
                dir->args->exp->id_llvm = *contador_simbolos;
                (*contador_simbolos)++;
                dir->args = dir->args->proximo;
            }
            args_func[strlen(args_func) - 2] = '\0';  // remove a ultima virgula
            sprintf(buffer_llvm, "\t%%%d = call %s @%s(%s)", *contador_simbolos,
                    (dir->tipo == INT ? "i32" : "float"), dir->lexema,
                    args_func);
            fprintf(fp, "%s\n", buffer_llvm);
            sprintf(buffer_llvm, "\tstore %s %%%d, ptr %%%s\n",
                    esq->tipo == INT ? "i32" : "float", *contador_simbolos,
                    esq->lexema);
            fprintf(fp, "%s\n", buffer_llvm);
        } else if (dir->tipo_simb == PONTEIRO || dir->escopo == 0) {
            // coloco dir uma variavel e depois faço o store na esquerda
            (*contador_simbolos)++;
            sprintf(buffer_llvm, "\t%%%d = load %s, ptr %%%s",
                    *contador_simbolos, dir->tipo == INT ? "i32" : "float",
                    dir->lexema);
            fprintf(fp, "%s\n", buffer_llvm);
            sprintf(buffer_llvm, "\tstore %s %%%d, ptr %%%s\n",
                    esq->tipo == INT ? "i32" : "float", *contador_simbolos,
                    esq->lexema);
            fprintf(fp, "%s\n", buffer_llvm);
        } else if (dir->tipo_simb == NUMERO) {
            sprintf(buffer_llvm, "\tstore %s %s, ptr %%%s",
                    esq->tipo == INT ? "i32" : "float", dir->lexema,
                    esq->lexema);
            fprintf(fp, "%s\n\n", buffer_llvm);
        } else {
            sprintf(buffer_llvm, "\tstore %s %%%d, ptr %%%s",
                    esq->tipo == INT ? "i32" : "float", *contador_simbolos,
                    esq->lexema);
            fprintf(fp, "%s\n\n", buffer_llvm);
        }
    
    }  else {
        if (dir->tipo_simb == FUNCAO) {
            (*contador_simbolos)++;
            // char buffer_args[1000];
            // while (dir->args) {
            //   (*contador_simbolos)++;
            //   dir->args->exp->id_llvm = *contador_simbolos;
            //   if(dir->args->exp->tipo_simb == NUMERO){
            //   }
            //   printf("dir->args %s %d\n", dir->args->exp->lexema,
            //   dir->args->exp->tipo_simb); sprintf(buffer_args, "%%%d = add %s
            //   0, %d", *contador_simbolos, dir->args->exp->tipo == INT ? "i32"
            //   : "float", dir->args->exp->valor_int); fprintf(fp, "%s\n",
            //   buffer_args);

            //   dir->args = dir->args->proximo;
            // }
            sprintf(buffer_llvm, "\t%%%d = call %s @%s()\n", *contador_simbolos,
                    (dir->tipo == INT ? "i32" : "float"), dir->lexema);
            fprintf(fp, "%s\n", buffer_llvm);
        } else if (dir->tipo_simb == PONTEIRO || dir->escopo == 0) {
            (*contador_simbolos)++;
            sprintf(buffer_llvm, "\t%%%d = load %s, ptr %%%s",
                    *contador_simbolos, dir->tipo == INT ? "i32" : "float",
                    dir->lexema);
            fprintf(fp, "%s\n", buffer_llvm);
            sprintf(buffer_llvm, "\t%s = %%%d", esq->lexema,
                    *contador_simbolos);
            fprintf(fp, "%s\n\n", buffer_llvm);
        } else {
            sprintf(buffer_llvm, "\n%s = %%%s\n", dir->lexema, esq->lexema);
            fprintf(fp, "%s\n\n", buffer_llvm);
        }
    }
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
                if (args != NULL) fprintf(fp, ", ");
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
                if (args != NULL) fprintf(fp, ", ");
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
                0) {  // variavel global alocada dinamicamente
                fprintf(fp, "@%s = global %s\n", lista->simb->lexema,
                        lista->simb->tipo == INT ? "i32" : "float");
                lista->simb->id_llvm = *contador_simbolos;
            } else {  // variavel local
                fprintf(fp, "\t%%%s = alloca %s\n", lista->simb->lexema,
                        lista->simb->tipo == INT ? "i32" : "float");
                lista->simb->id_llvm = *contador_simbolos;
            }
        } else  // retorno da funcao
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
        if (lista->simb->escopo ==
            0) {  // variavel global alocada dinamicamente
            fprintf(fp, "@%s = global %s, %d\n", lista->simb->lexema,
                    lista->simb->tipo == INT ? "i32" : "float",
                    lista->simb->valor_int);
            lista->simb->id_llvm = *contador_simbolos;
        }
        lista = lista->proximo;
    }
    fprintf(fp, "\n");
}