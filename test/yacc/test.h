/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

#ifndef YY_BASE_YY_TEST_H_INCLUDED
# define YY_BASE_YY_TEST_H_INCLUDED
/* Debug traces.  */
#ifndef BASE_YYDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define BASE_YYDEBUG 1
#  else
#   define BASE_YYDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define BASE_YYDEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined BASE_YYDEBUG */
#if BASE_YYDEBUG
extern int base_yydebug;
#endif

/* Token type.  */
#ifndef BASE_YYTOKENTYPE
# define BASE_YYTOKENTYPE
  enum base_yytokentype
  {
    NUM = 258,
    IDENT = 259
  };
#endif

/* Value type.  */
#if ! defined BASE_YYSTYPE && ! defined BASE_YYSTYPE_IS_DECLARED

union BASE_YYSTYPE
{
#line 31 "test.y" /* yacc.c:1909  */

	int ival;
	std::string str;
	const char* keyword;
	struct Expr* expr;

#line 74 "test.h" /* yacc.c:1909  */
};

typedef union BASE_YYSTYPE BASE_YYSTYPE;
# define BASE_YYSTYPE_IS_TRIVIAL 1
# define BASE_YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined BASE_YYLTYPE && ! defined BASE_YYLTYPE_IS_DECLARED
typedef struct BASE_YYLTYPE BASE_YYLTYPE;
struct BASE_YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define BASE_YYLTYPE_IS_DECLARED 1
# define BASE_YYLTYPE_IS_TRIVIAL 1
#endif



int base_yyparse (void);

#endif /* !YY_BASE_YY_TEST_H_INCLUDED  */
