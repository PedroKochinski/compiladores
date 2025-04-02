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
%}

%define parse.error detailed

%union {
    char *lexema;
    struct lista_simbolo * lista_s;
    Tipo tipo;
}

%token PROGRAM ABRE_PARENTESES FECHA_PARENTESES PONTO_VIRGULA VIRGULA OPERADOR_MULTIPLICATIVO
%token OR MENOS MAIS OPERADOR_RELACIONAL EOL PONTO_FINAL VAR FUNCTION PROCEDURE DOIS_PONTOS BEGIN_TOKEN END IF THEN ELSE DO WHILE OPERADOR_ATRIBUICAO
%token <lexema> ID <lexema> NUM
%type <lista_s> LISTA_DE_IDENTIFICADORES
%left '+' '-'
%left '*' '/'

%%

PROGRAMA: PROGRAM ID ABRE_PARENTESES LISTA_DE_IDENTIFICADORES FECHA_PARENTESES PONTO_VIRGULA {imprime_tabela_simbolos(log_file, tab_simbolos);}
    ;

LISTA_DE_IDENTIFICADORES: ID {$$ = insere_lista_simbolo(NULL, novo_simbolo1($1));}
    | LISTA_DE_IDENTIFICADORES VIRGULA ID {$$ = insere_lista_simbolo($1, novo_simbolo1($3));}
    ;
%%

int main(int argc, char ** argv) {
  log_file = fopen ("compilador.log", "w");
  printf("> ");
  yyparse();
  return 0;
}

int yyerror(const char *s){
  fprintf(stderr, "Erro: %s\n", s);
  return 0;
}