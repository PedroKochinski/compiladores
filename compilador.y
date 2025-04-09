%{
#include <stdio.h>
#include <stdlib.h>
#include "tabela_simbolos.h"
#include "compilador.h"

int yylex();
extern FILE *yyin;
extern int yylineno;
FILE *log_file, *out_file;

struct tabela_simbolos * tab_simbolos = NULL;
int escopo_atual = 0;
char *nome_funcao_atual = "SEM_ESCOPO_FUNCAO";
struct lista_simbolo *lista_identificadores = NULL;
struct lista_expressoes *lista_expressoes_atual = NULL;
int contador_simbolos = 0;
%}

%define parse.error detailed

%union {
    char *lexema;
    struct lista_simbolo * lista_s;
    struct expressao * expr;
    struct lista_expressoes * lista_expr;
    Tipo tipo;
}

%token PROGRAM ABRE_PARENTESES FECHA_PARENTESES PONTO_VIRGULA VIRGULA INTEIRO REAL
%token OR OPERADOR_RELACIONAL EOL PONTO_FINAL VAR FUNCTION PROCEDURE DOIS_PONTOS BEGIN_TOKEN END IF THEN ELSE DO WHILE OPERADOR_ATRIBUICAO
%token <lexema> ID <lexema> NUM <lexema> OPERADOR_MULTIPLICATIVO <lexema> MAIS <lexema> MENOS
%type <tipo>TIPO
%type <lista_s> LISTA_DE_IDENTIFICADORES
%type <lista_s> ARGUMENTOS
%type <lista_s> LISTA_DE_PARAMETROS
%type <expr> VARIAVEL <expr> FATOR <expr> TERMO <expr> EXPRESSAO <expr> EXPRESSAO_SIMPLES
%type <lista_expr> LISTA_DE_EXPRESSOES
%type <lexema> SINAL
/* %type <expr> EXPRESSAO <expr> FATOR <expr> TERMO <expr> EXPRESSAO_SIMPLES <expr> ENUNCIADO
%type <lista_expr> LISTA_DE_EXPRESSOES */
%left '+' '-'
%left '*' '/'

%%

PROGRAMA: PROGRAM ID ABRE_PARENTESES LISTA_DE_IDENTIFICADORES FECHA_PARENTESES PONTO_VIRGULA 
  DECLARACOES
  DECLARACOES_DE_SUBPROGRAMAS
  ;

LISTA_DE_IDENTIFICADORES: ID {
  lista_identificadores = insere_lista_simbolo(NULL, novo_simbolo5($1, VARIAVEL, escopo_atual, nome_funcao_atual));
  $$ = lista_identificadores;
  }
  | LISTA_DE_IDENTIFICADORES VIRGULA ID {
    lista_identificadores = insere_lista_simbolo($1, novo_simbolo5($3, VARIAVEL, escopo_atual, nome_funcao_atual));
    $$ = lista_identificadores;
    }
  ;

DECLARACOES: DECLARACOES VAR LISTA_DE_IDENTIFICADORES DOIS_PONTOS TIPO PONTO_VIRGULA {
  atualiza_tipo_simbolos($3,$5);
  materializa_simbolos(out_file, $3, &contador_simbolos);
  tab_simbolos = insere_simbolos_ts(tab_simbolos, $3);
  imprime_tabela_simbolos(log_file, tab_simbolos);
  }
  | /* empty */
  ;

TIPO: INTEIRO {$$ = 0;} /*como numero pq o enum tava conflitando*/
  | REAL {$$ = 1;}
  ;


DECLARACOES_DE_SUBPROGRAMAS: DECLARACOES_DE_SUBPROGRAMAS DECLARACAO_DE_SUBPROGRAMA PONTO_VIRGULA {
  tab_simbolos = remove_simbolos(tab_simbolos, escopo_atual);
  --escopo_atual;
  nome_funcao_atual = "SEM_ESCOPO_FUNCAO";
  imprime_tabela_simbolos(log_file, tab_simbolos);
  }
  | /* empty */
  ;

DECLARACAO_DE_SUBPROGRAMA: CABECALHO_DE_SUBPROGRAMA DECLARACOES ENUNCIADO_COMPOSTO 
  ;

CABECALHO_DE_SUBPROGRAMA: FUNCTION {++escopo_atual;} ID {nome_funcao_atual = $3;} ARGUMENTOS DOIS_PONTOS TIPO PONTO_VIRGULA {
  struct simbolo *nova_funcao = novo_simbolo4($3, FUNCAO, 0, $7);
  tab_simbolos = insere_simbolo_ts(tab_simbolos, nova_funcao);
  materializa_funcao(out_file, $5, nova_funcao, &contador_simbolos);
  insere_func_args(nova_funcao, $5);
  tab_simbolos = insere_simbolos_ts(tab_simbolos, $5);
  // imprime_tabela_simbolos(stdout, tab_simbolos);
  }
  | PROCEDURE {++escopo_atual;} ID {nome_funcao_atual = $3;} ARGUMENTOS PONTO_VIRGULA {
  struct simbolo *nova_procedure = novo_simbolo4($3, PROC, 0, VAZIO);
  tab_simbolos = insere_simbolo_ts(tab_simbolos, nova_procedure);
  materializa_funcao(out_file, $5, nova_procedure, &contador_simbolos);
  insere_func_args(nova_procedure, $5);
  tab_simbolos = insere_simbolos_ts(tab_simbolos, $5);
  // imprime_tabela_simbolos(log_file, tab_simbolos);
  }
  ;

ARGUMENTOS: ABRE_PARENTESES LISTA_DE_PARAMETROS FECHA_PARENTESES { $$ = $2;}
  | /* empty */
  ;

LISTA_DE_PARAMETROS: LISTA_DE_IDENTIFICADORES DOIS_PONTOS TIPO {
  atualiza_tipo_simbolos($1, $3);
  $$ = $1;
  }
  | VAR LISTA_DE_IDENTIFICADORES DOIS_PONTOS TIPO {  }
  | LISTA_DE_PARAMETROS PONTO_VIRGULA LISTA_DE_IDENTIFICADORES DOIS_PONTOS TIPO {
    atualiza_tipo_simbolos($3, $5);
     $$ = concatena_lista_simbolos($1, $3);
  }
  | LISTA_DE_PARAMETROS PONTO_VIRGULA VAR LISTA_DE_IDENTIFICADORES DOIS_PONTOS TIPO {}
  ;


ENUNCIADO_COMPOSTO: BEGIN_TOKEN ENUNCIADOS_OPCIONAIS END {fprintf(out_file, "}\n");}
                  ;

ENUNCIADOS_OPCIONAIS: LISTA_DE_ENUNCIADOS
                    | /* empty */
                    ;

LISTA_DE_ENUNCIADOS: ENUNCIADO
                   | LISTA_DE_ENUNCIADOS PONTO_VIRGULA ENUNCIADO
                   ;

ENUNCIADO: VARIAVEL OPERADOR_ATRIBUICAO EXPRESSAO {
          printf("ENUNCIADO: %s := %s\n", $1->lexema, $3->lexema);
          materializa_atribuicao(out_file, $1, $3);
          }
         | CHAMADA_DE_PROCEDIMENTO
         | ENUNCIADO_COMPOSTO
         | IF EXPRESSAO THEN ENUNCIADO ELSE ENUNCIADO
         | WHILE EXPRESSAO DO ENUNCIADO 
         ;

VARIAVEL: ID {
          // if(strcmp(nome_funcao_atual, $1) == 0) $$ = nova_expressao($1, RETORNO); // retorno da funcao
          // else $$ = nova_expressao2(tab_simbolos, $1, VARIAVEL, escopo_atual); // variavel

          struct simbolo *s = busca_simbolo(tab_simbolos, $1);
          if(s == NULL) {
            struct simbolo *s = novo_simbolo5($1, VARIAVEL, escopo_atual, nome_funcao_atual);
            tab_simbolos = insere_simbolo_ts(tab_simbolos, s);
          }
          struct expressao *nova;
          if(strcmp(nome_funcao_atual, $1) == 0) nova = nova_expressao($1, RETORNO); // retorno da funcao
          else nova = nova_expressao2(tab_simbolos, $1, VARIAVEL, escopo_atual); // variavel
          nova->id_tabela = tab_simbolos;
          $$ = nova; // variavel ou funcao
        }
        ;

CHAMADA_DE_PROCEDIMENTO: ID
                    | ID ABRE_PARENTESES LISTA_DE_EXPRESSOES FECHA_PARENTESES
                    ;

LISTA_DE_EXPRESSOES: EXPRESSAO { lista_expressoes_atual = insere_lista_expressoes(NULL, $1);
                     $$ = lista_expressoes_atual; 
                     }  
                    | LISTA_DE_EXPRESSOES VIRGULA EXPRESSAO {
                      lista_expressoes_atual = insere_lista_expressoes($1, $3);
                      $$ = lista_expressoes_atual;
                    }
                    ;

EXPRESSAO: EXPRESSAO_SIMPLES {$$ = $1;}
         | EXPRESSAO_SIMPLES OPERADOR_RELACIONAL EXPRESSAO_SIMPLES
         ;

EXPRESSAO_SIMPLES: TERMO { $$ = $1; } 
                 | SINAL TERMO {}
                 | EXPRESSAO_SIMPLES MAIS EXPRESSAO_SIMPLES { 
                  //  printf("EXPRESSAO_SIMPLES: %s %s %s\n", $1->lexema, $2, $3->lexema);
                  //  emite_operador_aditivo(&contador_simbolos, $1, $3, $2);
                   $$ = nova_expressao_operador_aditivo($1, $3, $2);
                 }
                 | EXPRESSAO_SIMPLES MENOS EXPRESSAO_SIMPLES {
                    // printf("EXPRESSAO_SIMPLES: %s %s %s\n", $1->lexema, $2, $3->lexema);
                  //  materializa_soma(out_file, &contador_simbolos, $1, $3, $2);
                    $$ = nova_expressao_operador_aditivo($1, $3, $2);
                 }
                 | EXPRESSAO_SIMPLES OR EXPRESSAO_SIMPLES { }
                 ;

TERMO: FATOR {
        $$ = $1;
      }
     | TERMO OPERADOR_MULTIPLICATIVO FATOR {
       printf("TERMO: %s %s %s\n", $1->lexema, $2, $3->lexema);
      $$ = nova_expressao_operador_multiplicativo($1, $3, $2);
      
     }
     ;

FATOR: ID {
      struct simbolo *s = busca_simbolo(tab_simbolos, $1);
      if(s == NULL) {
        struct simbolo *s = novo_simbolo5($1, VARIAVEL, escopo_atual, nome_funcao_atual);
        tab_simbolos = insere_simbolo_ts(tab_simbolos, s);
      }
      struct expressao *nova = nova_expressao($1, VARIAVEL);
      nova->id_tabela = tab_simbolos;
      $$ = nova; // variavel ou funcao
      }
     | ID ABRE_PARENTESES LISTA_DE_EXPRESSOES FECHA_PARENTESES { }
     | NUM {
        struct simbolo *s = busca_simbolo(tab_simbolos, $1);
        if(s == NULL) {
          struct simbolo *s = novo_simbolo5($1, NUMERO, escopo_atual, nome_funcao_atual);
          tab_simbolos = insere_simbolo_ts(tab_simbolos, s);
        }
        struct expressao *nova = nova_expressao($1, NUMERO);
        nova->id_tabela = tab_simbolos;
        $$ = nova; // verificar como adicionar o tipo certo
      }
     | ABRE_PARENTESES EXPRESSAO FECHA_PARENTESES {
        
      }
     ;

SINAL: MAIS {$$ = $1;}
     | MENOS {$$ = $1;}
     ;

%%

int main(int argc, char ** argv) {
  log_file = fopen ("compilador.log", "w");
  out_file = fopen ("saida.ll", "w");
  yyparse();
  return 0;
}

int yyerror(const char *s){
  fprintf(stderr, "Erro: %s Line: %d\n", s, yylineno);
  return 0;
}