/*
    sql yacc parser
*/
%{
#include <stdio.h>
#include <string>
#include <vector>
#include "SQLNode.h"
#include "StatementBlock.h"
#include "SingleStatement.h"

#define YYLLOC_DEFAULT(Current, Rhs, N) \
	do { \
		if ((N) > 0) \
			(Current) = (Rhs)[1]; \
		else \
			(Current) = (-1); \
	} while (0)

typedef std::vector<SQLNode*> List;

static void base_yyerror(YYLTYPE *yylloc, core_yyscan_t yyscanner,
						 const char *msg);

static StatementBlock* make_stmt_block(SingleStatement* single_statement);

inline List* list_make1(SQLNode* node) {
    List* list = new List();
    list->push_back(node);
    return list;
}

inline List* lappend(List* list,SQLNode* node) {
    list->push_back(node);
    return list;
}

%}
%pure-parser
%expect 0
%name-prefix="base_yy"
%locations

%parse-param {core_yyscan_t yyscanner}
%lex-param   {core_yyscan_t yyscanner}

%union {
    char* str;
    const char* keyword;
    int ival;
    SQLNode* node;
    List* list;
    ResTarget* target;
    StatementBlock* block;
    SingleStatement* stmt;
}

%type <block> stmtblock
%type <stmt> single_statement SelectStmt simple_selectstmt
%type <node> a_expr columnref
%type <str> ColLabel ColId
%type <target> target_el
%type <list> opt_target_list target_list 
%token <str> IDENT FCONST SCONST BCONST XCONST Op
%token <ival>	ICONST PARAM
%token			DOT_DOT
%token			LESS_EQUALS GREATER_EQUALS NOT_EQUALS
%token <keyword> K_SELECT K_FROM K_WHERE K_AS

%%
stmtmuti: stmtblock 
            {
                sp_yyget_extra(yyscanner)->block = $1;
            }
            ;

stmtblock: stmtblock ';' single_statement 
            {
                if ( $3 != NULL ) {
                    ($1)->push($3);
                }
                $$ = $1;
            }
            | single_statement
            {
                if ( $1 != NULL ) {
                    $$ = make_stmt_block($1);
                } else {
                    $$ = NULL;
                }
            }
            ;

single_statement: 
            SelectStmt                              { $$ = $1; }
            | /*empty*/
            {
                $$ = NULL;
            }
            ;

SelectStmt: simple_selectstmt                       { $$ = $1; }
            ;

simple_selectstmt:
            K_SELECT opt_target_list
            {
                $$ = new SelectStatement();
                $$->opt_target_list = $2;
            }
            ;

opt_target_list: target_list						{ $$ = $1; }
			| /* EMPTY */							{ $$ = NULL; }
		    ;

target_list:
			target_el								{ $$ = list_make1($1); }
			| target_list ',' target_el				{ $$ = lappend($1, $3); }
		    ;

target_el:
            a_expr K_AS ColLabel                    { $$ = new ResTarget($1,$3); }
            | a_expr                                { $$ = new ResTarget($1,NULL); }
            | '*'
            {
                CloumnRef* ref = new CloumnRef();
                ref->fields.push_back("*");
                $$ = new ResTarget(ref,NULL);
            }
            ;

a_expr:     columnref                               { $$ = $1; }
            ;

columnref:  ColId
            {
                CloumnRef* ref = new CloumnRef();
                ref->fields.push_back($1);
                $$ = ref;
            }
            ;

ColLabel:
            IDENT                                   { $$ = $1; }
            ;

ColId:
            IDENT                                   { $$ = $1; }
            ;
%%

static void
base_yyerror(YYLTYPE *yylloc, core_yyscan_t yyscanner, const char *msg)
{
	printf(msg);
}

StatementBlock* make_stmt_block(SingleStatement* single_statement) {
    StatementBlock* block = new StatementBlock();
    block->push(block);
    return block;
}

/* parser_init()
 * Initialize to parse one query string
 */
void
parser_init(base_yy_extra_type *yyext)
{
	yyext->block = NULL;		/* in case grammar forgets to set it */
}