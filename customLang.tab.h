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

#ifndef YY_YY_CUSTOMLANG_TAB_H_INCLUDED
# define YY_YY_CUSTOMLANG_TAB_H_INCLUDED
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
    INT = 258,                     /* INT  */
    FLOAT = 259,                   /* FLOAT  */
    CHAR = 260,                    /* CHAR  */
    STRING = 261,                  /* STRING  */
    BOOL = 262,                    /* BOOL  */
    VOID = 263,                    /* VOID  */
    CONSTANT = 264,                /* CONSTANT  */
    INTVALUE = 265,                /* INTVALUE  */
    FLOATVALUE = 266,              /* FLOATVALUE  */
    CHARVALUE = 267,               /* CHARVALUE  */
    STRINGVALUE = 268,             /* STRINGVALUE  */
    BOOLVALUE = 269,               /* BOOLVALUE  */
    IF = 270,                      /* IF  */
    FOR = 271,                     /* FOR  */
    WHILE = 272,                   /* WHILE  */
    OPEN_PAREN = 273,              /* OPEN_PAREN  */
    CLOSE_PAREN = 274,             /* CLOSE_PAREN  */
    OPEN_BRACE = 275,              /* OPEN_BRACE  */
    CLOSE_BRACE = 276,             /* CLOSE_BRACE  */
    OPEN_BRACKET = 277,            /* OPEN_BRACKET  */
    CLOSE_BRACKET = 278,           /* CLOSE_BRACKET  */
    SEMICOLON = 279,               /* SEMICOLON  */
    COMMA = 280,                   /* COMMA  */
    ASSIGN = 281,                  /* ASSIGN  */
    PLUS = 282,                    /* PLUS  */
    MINUS = 283,                   /* MINUS  */
    MULT = 284,                    /* MULT  */
    DIV = 285,                     /* DIV  */
    AND = 286,                     /* AND  */
    OR = 287,                      /* OR  */
    NOT = 288,                     /* NOT  */
    EQUAL = 289,                   /* EQUAL  */
    NOT_EQUAL = 290,               /* NOT_EQUAL  */
    LESS_THAN = 291,               /* LESS_THAN  */
    GREATER_THAN = 292,            /* GREATER_THAN  */
    CLASS = 293,                   /* CLASS  */
    RETURN = 294,                  /* RETURN  */
    IDENTIFIER = 295               /* IDENTIFIER  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 170 "customLang.y"

    char* stringValue;
    struct ASTNode* node;


#line 110 "customLang.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_CUSTOMLANG_TAB_H_INCLUDED  */
