%top {
#include <stdio.h>
#include <string>
#include "test.h"
}

%{
#define YYSTYPE         BASE_YYSTYPE
#define YYLTYPE         BASE_YYLTYPE
%}

%option reentrant
%option bison-bridge
%option bison-locations
%option 8bit
%option never-interactive
%option nodefault
%option noinput
%option nounput
%option noyywrap
%option noyyalloc
%option noyyrealloc
%option noyyfree
%option warn
%option prefix="base_yy"

digit			[0-9]
ident_start		[A-Za-z\200-\377_]
ident_cont		[A-Za-z\200-\377_0-9\$]

identifier		{ident_start}{ident_cont}*
decimal			(({digit}*\.{digit}+)|({digit}+\.{digit}*))
%%
decimal         {
                    yylval->ival = atoi(yytext);
                    return NUM;
                }

identifier      {
                    yylval->str.assign(yytext,yyleng);
                    return IDENT;
                }
%%