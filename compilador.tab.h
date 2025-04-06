/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_COMPILADOR_TAB_H_INCLUDED
# define YY_YY_COMPILADOR_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    PROGRAM = 258,                 /* PROGRAM  */
    ABRE_PARENTESES = 259,         /* ABRE_PARENTESES  */
    FECHA_PARENTESES = 260,        /* FECHA_PARENTESES  */
    PONTO_VIRGULA = 261,           /* PONTO_VIRGULA  */
    VIRGULA = 262,                 /* VIRGULA  */
    OPERADOR_MULTIPLICATIVO = 263, /* OPERADOR_MULTIPLICATIVO  */
    INTEIRO = 264,                 /* INTEIRO  */
    REAL = 265,                    /* REAL  */
    OR = 266,                      /* OR  */
    MENOS = 267,                   /* MENOS  */
    MAIS = 268,                    /* MAIS  */
    OPERADOR_RELACIONAL = 269,     /* OPERADOR_RELACIONAL  */
    EOL = 270,                     /* EOL  */
    PONTO_FINAL = 271,             /* PONTO_FINAL  */
    VAR = 272,                     /* VAR  */
    FUNCTION = 273,                /* FUNCTION  */
    PROCEDURE = 274,               /* PROCEDURE  */
    DOIS_PONTOS = 275,             /* DOIS_PONTOS  */
    BEGIN_TOKEN = 276,             /* BEGIN_TOKEN  */
    END = 277,                     /* END  */
    IF = 278,                      /* IF  */
    THEN = 279,                    /* THEN  */
    ELSE = 280,                    /* ELSE  */
    DO = 281,                      /* DO  */
    WHILE = 282,                   /* WHILE  */
    OPERADOR_ATRIBUICAO = 283,     /* OPERADOR_ATRIBUICAO  */
    ID = 284,                      /* ID  */
    NUM = 285                      /* NUM  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 20 "compilador.y"

    char *lexema;
    struct lista_simbolo * lista_s;
    Tipo tipo;

#line 100 "compilador.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_COMPILADOR_TAB_H_INCLUDED  */
