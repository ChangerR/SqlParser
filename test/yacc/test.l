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
/*%option nodefault*/
%option noinput
%option nounput
%option noyywrap
/*%option noyyalloc*/
/*%option noyyrealloc*/
/*%option noyyfree*/
%option warn
%option prefix="base_yy"

space			[ \t\n\r\f]
horiz_space		[ \t\f]
newline			[\n\r]
non_newline		[^\n\r]

comment			("//"{non_newline}*)

whitespace		({space}+|{comment})

digit			[0-9]
ident_start		[A-Za-z\200-\377_]
ident_cont		[A-Za-z\200-\377_0-9\$]

identifier		{ident_start}{ident_cont}*
decimal			(({digit}*\.{digit}+)|({digit}+\.{digit}*))

other			.
%%
{whitespace}	{
					/* ignore */
				}

{decimal}       {
                    yylval->ival = atoi(yytext);
                    return NUM;
                }

{identifier}    {
                    yylval->str.assign(yytext,yyleng);
                    return IDENT;
                }

{other}         {
                    return yytext[0];
                }

<<EOF>>			{
					yyterminate();
				}
%%

//end
