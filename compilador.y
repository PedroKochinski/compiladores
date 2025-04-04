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
char *nome_funcao_atual = "GLOBAL";
%}

%define parse.error detailed

%union {
    char *lexema;
    struct lista_simbolo * lista_s;
    Tipo tipo;
}

%token PROGRAM ABRE_PARENTESES FECHA_PARENTESES PONTO_VIRGULA VIRGULA OPERADOR_MULTIPLICATIVO INTEIRO REAL
%token OR MENOS MAIS OPERADOR_RELACIONAL EOL PONTO_FINAL VAR FUNCTION PROCEDURE DOIS_PONTOS BEGIN_TOKEN END IF THEN ELSE DO WHILE OPERADOR_ATRIBUICAO
%token <lexema> ID <lexema> NUM
%type <tipo>TIPO

%type <lista_s> LISTA_DE_IDENTIFICADORES
%type <lista_s> ARGUMENTOS
%type <lista_s> LISTA_DE_PARAMETROS
%left '+' '-'
%left '*' '/'

%%

PROGRAMA: PROGRAM ID ABRE_PARENTESES LISTA_DE_IDENTIFICADORES FECHA_PARENTESES PONTO_VIRGULA 
  DECLARACOES
  DECLARACOES_DE_SUBPROGRAMAS
  ;

LISTA_DE_IDENTIFICADORES: ID {$$ = insere_lista_simbolo(NULL, novo_simbolo5($1, VARIAVEL, escopo_atual, nome_funcao_atual));}
  | LISTA_DE_IDENTIFICADORES VIRGULA ID {$$ = insere_lista_simbolo($1, novo_simbolo5($3, VARIAVEL, escopo_atual, nome_funcao_atual));}
  ;

DECLARACOES: DECLARACOES VAR LISTA_DE_IDENTIFICADORES DOIS_PONTOS TIPO PONTO_VIRGULA {atualiza_tipo_simbolos($3,$5); tab_simbolos = insere_simbolos_ts(tab_simbolos, $3); imprime_tabela_simbolos(log_file, tab_simbolos);}
  | /* empty */
  ;

TIPO: INTEIRO {$$ = 0;} /*como numero pq o enum tava conflitando*/
  | REAL {$$ = 1;}
  ;


DECLARACOES_DE_SUBPROGRAMAS: DECLARACOES_DE_SUBPROGRAMAS DECLARACAO_DE_SUBPROGRAMA PONTO_VIRGULA {--escopo_atual; nome_funcao_atual = "GLOBAL";}
  | /* empty */
  ;

DECLARACAO_DE_SUBPROGRAMA: CABECALHO_DE_SUBPROGRAMA DECLARACOES ENUNCIADO_COMPOSTO 
  ;

CABECALHO_DE_SUBPROGRAMA: FUNCTION {++escopo_atual;} ID {nome_funcao_atual = $3;} ARGUMENTOS DOIS_PONTOS TIPO PONTO_VIRGULA {
  tab_simbolos = insere_simbolo_ts(tab_simbolos, novo_simbolo4($3, FUNCAO, 0, $7));
  imprime_tabela_simbolos(log_file, tab_simbolos);
}
  | PROCEDURE ID ARGUMENTOS PONTO_VIRGULA 
  ;

ARGUMENTOS: ABRE_PARENTESES LISTA_DE_PARAMETROS FECHA_PARENTESES { $$ = $2;}
  | /* empty */
  ;

LISTA_DE_PARAMETROS: LISTA_DE_IDENTIFICADORES DOIS_PONTOS TIPO {
  atualiza_tipo_simbolos($1, $3);
  tab_simbolos = insere_simbolos_ts(tab_simbolos, $1);
  $$ = $1;
  }
  | VAR LISTA_DE_IDENTIFICADORES DOIS_PONTOS TIPO {  }
  | LISTA_DE_PARAMETROS PONTO_VIRGULA LISTA_DE_IDENTIFICADORES DOIS_PONTOS TIPO 
  | LISTA_DE_PARAMETROS PONTO_VIRGULA VAR LISTA_DE_IDENTIFICADORES DOIS_PONTOS TIPO 
  ;


ENUNCIADO_COMPOSTO: BEGIN_TOKEN ENUNCIADOS_OPCIONAIS END
                  ;

ENUNCIADOS_OPCIONAIS: LISTA_DE_ENUNCIADOS
                    | /* empty */
                    ;

LISTA_DE_ENUNCIADOS: ENUNCIADO
                   | LISTA_DE_ENUNCIADOS PONTO_VIRGULA ENUNCIADO
                   ;

ENUNCIADO: VARIAVEL OPERADOR_ATRIBUICAO EXPRESSAO
         | CHAMADA_DE_PROCEDIMENTO
         | ENUNCIADO_COMPOSTO
         | IF EXPRESSAO THEN ENUNCIADO ELSE ENUNCIADO
         | WHILE EXPRESSAO DO ENUNCIADO 
         ;

VARIAVEL: ID 
        ;

CHAMADA_DE_PROCEDIMENTO: ID
                    | ID ABRE_PARENTESES LISTA_DE_EXPRESSOES FECHA_PARENTESES
                    ;

LISTA_DE_EXPRESSOES: EXPRESSAO
                   | LISTA_DE_EXPRESSOES VIRGULA EXPRESSAO
                   ;

EXPRESSAO: EXPRESSAO_SIMPLES
         | EXPRESSAO_SIMPLES OPERADOR_RELACIONAL EXPRESSAO_SIMPLES
         ;

EXPRESSAO_SIMPLES: TERMO
                 | SINAL TERMO  
                 | EXPRESSAO_SIMPLES MAIS EXPRESSAO_SIMPLES 
                 | EXPRESSAO_SIMPLES MENOS EXPRESSAO_SIMPLES 
                 | EXPRESSAO_SIMPLES OR EXPRESSAO_SIMPLES 
                 ;

TERMO: FATOR
     | TERMO OPERADOR_MULTIPLICATIVO FATOR
     ;

FATOR: ID
     | ID ABRE_PARENTESES LISTA_DE_EXPRESSOES FECHA_PARENTESES
     | NUM 
     | ABRE_PARENTESES EXPRESSAO FECHA_PARENTESES 
     ;

SINAL: MAIS
     | MENOS 
     ;

%%

int main(int argc, char ** argv) {
  log_file = fopen ("compilador.log", "w");
  yyparse();
  return 0;
}

int yyerror(const char *s){
  fprintf(stderr, "Erro: %s\n", s);
  return 0;
}