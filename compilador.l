%option yylineno 

%{
#include "tabela_simbolos.h"
#include "compilador.h"
#include "compilador.tab.h"
%}

ID [a-zA-Z][a-zA-Z1-9]*
NUM [0-9]+

%%

{NUM} { return NUM; }
"*"|"/"|"div"|"mod"|"and" { return OPERADOR_MULTIPLICATIVO; }
"or" { return OR; }
"-" { return MENOS; }
"+" { return MAIS; }
"="|"<>"|">"|">="|"<"|"<=" { return OPERADOR_RELACIONAL; }
":=" { return OPERADOR_ATRIBUICAO; }
"do" { return DO; }
"while" { return WHILE; }
"else" { return ELSE; }
"then" { return THEN; }
"if" { return IF; }
"end" { return END; }
"begin" { return BEGIN_TOKEN; }
":" { return DOIS_PONTOS; }
";" { return PONTO_VIRGULA; }
")" { return FECHA_PARENTESES; }
"(" { return ABRE_PARENTESES; }
"procedure" { return PROCEDURE; }
"function" { return FUNCTION; }
"real" { return REAL; }
"integer" { return INTEIRO; }
"var" { return VAR; }
"." { return PONTO_FINAL; }
"program" { return PROGRAM; }
"," { return VIRGULA; }
{ID} { yylval.lexema = strdup(yytext); return ID; }
\n { }
[ \t] {/*ignorar espacos em branco, incluindo tabs*/}

%%

// Sem codigo aqui, flex sera usado pelo bison e nao 'standalone'