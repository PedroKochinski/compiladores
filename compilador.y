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
int contador_if = 0;
int contador_else = 0;
int contador_fim_if = 0;
int contador_while = 0;
int contador_fim_while = 0;
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
%token OR  EOL PONTO_FINAL VAR FUNCTION PROCEDURE DOIS_PONTOS BEGIN_TOKEN END IF THEN ELSE DO WHILE OPERADOR_ATRIBUICAO
%token <lexema> ID <lexema> NUM <lexema> OPERADOR_MULTIPLICATIVO <lexema> MAIS <lexema> MENOS <lexema> OPERADOR_RELACIONAL
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
  {
    // materializar as funcoes de escrita e leitura e colocar os simbolos na tabela
    fprintf(out_file, "declare i32 @printf(ptr noundef, ...)\n");
    fprintf(out_file, "declare i32 @__isoc99_scanf(ptr noundef, ...)\n");
    fprintf(out_file, "@read_int = private unnamed_addr constant [3 x i8] c\"%%d\\00\", align 1\n");
    fprintf(out_file, "@write_int = private unnamed_addr constant [4 x i8] c\"%%d\\0A\\00\", align 1\n");
    fprintf(out_file, "@read_float = private unnamed_addr constant [3 x i8] c\"%%f\\00\", align 1\n");
    fprintf(out_file, "@write_float = private unnamed_addr constant [4 x i8] c\"%%f\\0A\\00\", align 1\n\n");
    // tab_simbolos = insere_simbolo_ts(tab_simbolos, novo_simbolo5("write", FUNCAO, 0, VAZIO));
  } DECLARACOES
  DECLARACOES_DE_SUBPROGRAMAS {
    fprintf(out_file, "define i32 @main() {\nentry:\n");
  }
  ENUNCIADO_COMPOSTO
  PONTO_FINAL {
    fprintf(out_file, "\tret i32 0\n");
    fprintf(out_file, "}\n");
    tab_simbolos = remove_simbolos(tab_simbolos, escopo_atual);
    --escopo_atual;
    nome_funcao_atual = "SEM_ESCOPO_FUNCAO";
    fclose(log_file);
    fclose(out_file);
    printf("Compilacao finalizada com sucesso!\n");
    printf("Arquivo de saida: saida.ll\n");
    printf("Arquivo de log: compilador.log\n");
  }
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
  if(escopo_atual == 0)  materializa_simbolos_globais(out_file, $3, &contador_simbolos);
  else materializa_simbolos(out_file, $3, &contador_simbolos);
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

DECLARACAO_DE_SUBPROGRAMA: CABECALHO_DE_SUBPROGRAMA DECLARACOES  ENUNCIADO_COMPOSTO {
  struct simbolo *s = busca_simbolo3(tab_simbolos, nome_funcao_atual, 0);
  printf("DECLARACAO DE SUBPROGRAMA: %s (tipo_simb %d e tipo %d)\n", s->lexema, s->tipo_simb, s->tipo);
  if(s->tipo_simb == PROC || s->tipo == VAZIO) {
    fprintf(out_file, "\tret void\n");
  }
  else if(s->tipo_simb == FUNCAO) {
    ++(contador_simbolos);
    fprintf(out_file, "\t%%%d = load %s, ptr %%%s\n", contador_simbolos, s->tipo == INT ? "i32" : "float", s->lexema);
    fprintf(out_file, "\tret i32 %%%d\n", contador_simbolos);
  }
  fprintf(out_file, "}\n");
  }
  ;

CABECALHO_DE_SUBPROGRAMA: FUNCTION {++escopo_atual;} ID {nome_funcao_atual = $3;} ARGUMENTOS DOIS_PONTOS TIPO PONTO_VIRGULA {
  struct simbolo *nova_funcao = novo_simbolo4($3, FUNCAO, 0, $7);
  materializa_funcao(out_file, $5, nova_funcao, &contador_simbolos);
  insere_func_args(nova_funcao, $5);
  tab_simbolos = insere_simbolos_ts(tab_simbolos, $5);
  tab_simbolos = insere_simbolo_ts(tab_simbolos, nova_funcao);
  }
  | PROCEDURE {++escopo_atual;} ID {nome_funcao_atual = $3;} ARGUMENTOS PONTO_VIRGULA {
  struct simbolo *nova_procedure = novo_simbolo4($3, PROC, 0, VAZIO);
  materializa_funcao(out_file, $5, nova_procedure, &contador_simbolos);
  insere_func_args(nova_procedure, $5);
  tab_simbolos = insere_simbolos_ts(tab_simbolos, $5);
  tab_simbolos = insere_simbolo_ts(tab_simbolos, nova_procedure);
  }
  ;

ARGUMENTOS: ABRE_PARENTESES LISTA_DE_PARAMETROS FECHA_PARENTESES { $$ = $2;}
  | /* empty */
  ;

LISTA_DE_PARAMETROS: LISTA_DE_IDENTIFICADORES DOIS_PONTOS TIPO {
  atualiza_tipo_simbolos($1, $3);
  $$ = $1;
  }
  | VAR LISTA_DE_IDENTIFICADORES DOIS_PONTOS TIPO { 
    atualiza_tipo_simbolos($2, $4);
    atualiza_tipo_simb_simbolos($2, REFERENCIA);
    $$ = $2;
  }
  | LISTA_DE_PARAMETROS PONTO_VIRGULA LISTA_DE_IDENTIFICADORES DOIS_PONTOS TIPO {
    atualiza_tipo_simbolos($3, $5);
     $$ = concatena_lista_simbolos($1, $3);
  }
  | LISTA_DE_PARAMETROS PONTO_VIRGULA VAR LISTA_DE_IDENTIFICADORES DOIS_PONTOS TIPO {
    atualiza_tipo_simbolos($4, $6);
    atualiza_tipo_simb_simbolos($4, REFERENCIA);
    $$ = concatena_lista_simbolos($1, $4);
  }
  ;


ENUNCIADO_COMPOSTO: BEGIN_TOKEN ENUNCIADOS_OPCIONAIS END 
                  ;

ENUNCIADOS_OPCIONAIS: LISTA_DE_ENUNCIADOS
                    | /* empty */
                    ;

LISTA_DE_ENUNCIADOS: ENUNCIADO
                   | LISTA_DE_ENUNCIADOS PONTO_VIRGULA ENUNCIADO {  }
                   ;

ENUNCIADO: VARIAVEL OPERADOR_ATRIBUICAO EXPRESSAO {
          fprintf(out_file,  "; ENUNCIADO: %s (tipo_simb %d) := %s (tipo_simb %d)\n", $1->lexema, $1->tipo_simb, $3->lexema, $3->tipo_simb);
          materializa_atribuicao(tab_simbolos, out_file, $1, $3, &contador_simbolos);
          }
         | CHAMADA_DE_PROCEDIMENTO 
         | ENUNCIADO_COMPOSTO
         | IF {
           if(contador_else == 0) contador_else = contador_if;
          ++contador_if;
          ++contador_else;
         } EXPRESSAO {
          fprintf(out_file, "\tbr i1 %%%d, label %%then_%d, label %%else_%d\n", $3->id_llvm, contador_if, contador_else);
          }
          THEN {
            fprintf(out_file, "then_%d:\n", contador_if);
          }
          ENUNCIADO {
            fprintf(out_file, "\tbr label %%fim_if_%d\n", contador_if);
          } ELSE {
            fprintf(out_file, "else_%d:\n", contador_else);
          }
          ENUNCIADO {
            ++contador_fim_if;
            fprintf(out_file, "\tbr label %%fim_if_%d\nfim_if_%d:\n", contador_fim_if, contador_fim_if);
            --contador_else;
          }
         | WHILE {
            ++contador_while;
            ++contador_fim_while;
            fprintf(out_file, "\tbr label %%teste_while_%d\n", contador_while);
            fprintf(out_file, "teste_while_%d:\n", contador_while);
          }
          EXPRESSAO {
            fprintf(out_file, "\tbr i1 %%%d, label %%while_%d, label %%fim_while_%d\n", $3->id_llvm, contador_while, contador_while);
            fprintf(out_file, "while_%d:\n", contador_while);
          } DO
           ENUNCIADO {
            fprintf(out_file, "\tbr label %%teste_while_%d\n", contador_fim_while);
            fprintf(out_file, "fim_while_%d:\n", contador_fim_while);
            --contador_fim_while;
           }
         ;

VARIAVEL: ID {
          printf("VARIAVEL: %s\n", $1);
          struct simbolo *s = busca_simbolo(tab_simbolos, $1);
          if(s == NULL) {
            fprintf(stderr, "Erro: variavel %s nao declarada\n", $1);
            exit(1);
          }
          struct expressao *nova;
          if(strcmp(nome_funcao_atual, $1) == 0){
            printf("VARIAVEL DE RETORNO: %s %s\n", $1, nome_funcao_atual);
            nova = nova_expressao2(tab_simbolos, $1, RETORNO, escopo_atual, s->tipo); // retorno da funcao
          } 
          else nova = nova_expressao2(tab_simbolos, $1, s->tipo_simb, s->escopo, s->tipo); // variavel
          nova->id_tabela = tab_simbolos;
          $$ = nova; // variavel ou funcao
        }
        ;

CHAMADA_DE_PROCEDIMENTO: ID {
                      /*buscar na ts se existe simbolo com esse id e se ele é um proc*/
                      struct simbolo *s = busca_simbolo2(tab_simbolos, $1, PROC);
                      if(s == NULL) {
                        fprintf(stderr, "Erro: procedimento %s nao declarado\n", $1);
                        exit(1);
                      }
                      struct expressao *func = executar_funcao(out_file, tab_simbolos, $1, NULL, &contador_simbolos);
                      materializa_chamada_funcao(out_file, func, &contador_simbolos);
                      }
                    | ID ABRE_PARENTESES LISTA_DE_EXPRESSOES FECHA_PARENTESES {
                      struct simbolo *s = busca_simbolo(tab_simbolos, $1);
                      if(s != NULL) fprintf(out_file, "; CHAMADA DE PROCEDIMENTO: %s\n", s->lexema);
                      struct expressao *func = executar_funcao(out_file, tab_simbolos, $1, $3, &contador_simbolos);
                      if(s != NULL && s->tipo == VAZIO) materializa_chamada_funcao(out_file, func, &contador_simbolos);
                      }
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
         | EXPRESSAO_SIMPLES OPERADOR_RELACIONAL EXPRESSAO_SIMPLES {
           $$ = nova_expressao_operador_relacional(out_file, tab_simbolos, $1, $3, $2, &contador_simbolos);
         }
         ;

EXPRESSAO_SIMPLES: TERMO { $$ = $1; } 
                 | SINAL TERMO {
                  if(strcmp($1, "+") == 0) {
                    $$ = $2; // sinal positivo, retorna o termo
                  } else {
                    $2->valor_int = - $2->valor_int;
                    $2->valor_float = - $2->valor_float;  
                    $$ = $2; 
                  }
                 }
                 | EXPRESSAO_SIMPLES MAIS EXPRESSAO_SIMPLES { 
                   $$ = nova_expressao_operador_aditivo_e_multiplicativo(out_file, tab_simbolos, $1, $3, $2, &contador_simbolos);
                 }
                 | EXPRESSAO_SIMPLES MENOS EXPRESSAO_SIMPLES {
                    $$ = nova_expressao_operador_aditivo_e_multiplicativo(out_file, tab_simbolos, $1, $3, $2, &contador_simbolos);
                 }
                 | EXPRESSAO_SIMPLES OR EXPRESSAO_SIMPLES { }
                 ;

TERMO: FATOR {
        $$ = $1;
      }
     | TERMO OPERADOR_MULTIPLICATIVO FATOR {
        fprintf(out_file, "; TERMO: %s %s %s\n", $1->lexema, $2, $3->lexema);
        struct expressao *nova;
        nova = nova_expressao_operador_aditivo_e_multiplicativo(out_file, tab_simbolos, $1, $3, $2, &contador_simbolos);
        printf("NOVA TERMO: %s (tipo_simb %d)   \n", nova->lexema, nova->tipo_simb);
        $$ = nova;
     }
     ;

FATOR: ID {
      struct simbolo *s = busca_simbolo(tab_simbolos, $1);
      if(s == NULL) {
        fprintf(stderr, "Erro: variavel %s nao declarada\n", $1);
        exit(1);
      }
      struct expressao *nova = nova_expressao2(tab_simbolos, $1, s->tipo_simb, s->escopo, s->tipo);
      nova->id_tabela = tab_simbolos;
      $$ = nova; // variavel ou funcao
      }
     | ID ABRE_PARENTESES LISTA_DE_EXPRESSOES FECHA_PARENTESES {
        $$ =  executar_funcao(out_file, tab_simbolos, $1, $3, &contador_simbolos);
      }
     | NUM {
        struct expressao *nova = nova_expressao(tab_simbolos, $1, NUMERO, escopo_atual);
        $$ = nova; // verificar como adicionar o tipo certo
      }
     | ABRE_PARENTESES EXPRESSAO FECHA_PARENTESES {       
        $$ = $2; // retorna o resultado da expressao
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