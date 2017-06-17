/*
    sql yacc parser
*/
%{
#include <stdio.h>
#include <string>
#include <vector>
#include <string.h>

#include "gramparser.h"


#define YYLLOC_DEFAULT(Current, Rhs, N) \
	do { \
		if ((N) > 0) \
			(Current) = (Rhs)[1]; \
		else \
			(Current) = (-1); \
	} while (0)

static void base_yyerror(YYLTYPE *yylloc, core_yyscan_t yyscanner,
						 const char *msg);

static StatementBlock* make_stmt_block(SingleStatement* single_statement);

static void release_list_object(List* list);
static char* sql_strdup(const char* source);

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
    core_YYSTYPE		core_yystype;

    /* these fields must match core_YYSTYPE: */
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
%type <node> a_expr columnref indirection_el qualified_name relation_expr table_ref
             where_clause
%type <str> ColLabel ColId attr_name opt_alias_clause alias_clause
%type <target> target_el
%type <list> opt_target_list target_list from_clause from_list indirection
%token <str> IDENT FCONST SCONST BCONST XCONST Op
%token <ival>	ICONST PARAM
%token			DOT_DOT
%token			LESS_EQUALS GREATER_EQUALS NOT_EQUALS
%type <keyword> unreserved_keyword type_func_name_keyword
%type <keyword> col_name_keyword reserved_keyword
%token <keyword> SELECT FROM WHERE AS SET INT LEFT LIKE RIGHT
                 OR AND

%left           OR
%left           AND
%nonassoc       '<' '>' '=' LESS_EQUALS GREATER_EQUALS NOT_EQUALS
%left           '+' '-'
%left           '*' '/' '%'
%left           '.'
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
            SELECT opt_target_list from_clause where_clause
            {
                SelectStatement* stmt = new SelectStatement();
                stmt->opt_target_list = $2;
                stmt->from_list = $3;
                stmt->where_clause = $4;
                $$ = stmt;
            }
            ;

/*****************************************************************************
 *
 *	target list for SELECT
 *
 *****************************************************************************/

opt_target_list: target_list						{ $$ = $1; }
			| /* EMPTY */							{ $$ = NULL; }
		    ;

target_list:
			target_el								{ $$ = list_make1($1); }
			| target_list ',' target_el				{ $$ = lappend($1, $3); }
		    ;

target_el:
            a_expr AS ColLabel                    { $$ = new ResTarget($1,$3); }
            | a_expr                                { $$ = new ResTarget($1,NULL); }
            | '*'
            {
                CloumnRef* ref = new CloumnRef();
                char* buf = (char*)Allocator::malloc(8);
                buf[0] = '*';
                buf[1] = 0;
                ref->fields.push_back(new SQLBaseElem(buf));
                $$ = new ResTarget(ref,NULL);
            }
            ;

a_expr:     columnref                               { $$ = $1; }
            | ICONST                                { $$ = new SQLBaseElem($1); }
            | FCONST                                { $$ = new SQLBaseElem($1); }
            | SCONST                                { $$ = new SQLBaseElem($1); }
            | BCONST                                { $$ = new SQLBaseElem($1); }
            | XCONST                                { $$ = new SQLBaseElem($1); }
            | a_expr '+' a_expr                     
                { $$ = new Expression(Expression::AEXPR_OP,"+",$1,$3); }
            | a_expr '-' a_expr
                { $$ = new Expression(Expression::AEXPR_OP,"-",$1,$3); }
            | a_expr '*' a_expr
                { $$ = new Expression(Expression::AEXPR_OP,"*",$1,$3); }
            | a_expr '/' a_expr
                { $$ = new Expression(Expression::AEXPR_OP,"/",$1,$3); }
            | a_expr '%' a_expr
                { $$ = new Expression(Expression::AEXPR_OP,"%",$1,$3); }
            | a_expr '=' a_expr
                { $$ = new Expression(Expression::AEXPR_OP,"=",$1,$3); }
            | a_expr '<' a_expr
                { $$ = new Expression(Expression::AEXPR_OP,"<",$1,$3); }
            | a_expr '>' a_expr
                { $$ = new Expression(Expression::AEXPR_OP,">",$1,$3); }
            | a_expr AND a_expr
                { $$ = new Expression(Expression::AND_EXPR,NULL,$1,$3); }
            | a_expr OR a_expr
                { $$ = new Expression(Expression::OR_EXPR,NULL,$1,$3); }
            ;

columnref:  ColId
            {
                CloumnRef* ref = new CloumnRef();
                ref->fields.push_back(new SQLBaseElem($1));
                $$ = ref;
            }
            | ColId indirection
            {
                CloumnRef* ref = new CloumnRef();
                ref->fields.push_back(new SQLBaseElem($1));

                for ( auto itr = $2->begin() ; itr != $2->end() ; ++itr) {
                    ref->fields.push_back((SQLBaseElem*)*itr); 
                }
                delete $2;
                $$ = ref;
            }
            ;

indirection_el:
			'.' attr_name
				{
					$$ = new SQLBaseElem($2);
				}
            ;

indirection:
			indirection_el							{ $$ = list_make1($1); }
			| indirection indirection_el			{ $$ = lappend($1, $2); }
		    ;

/*****************************************************************************
 *
 *	clauses common to all Optimizable Stmts:
 *		from_clause		- allow list of both JOIN expressions and table names
 *		where_clause	- qualifications for joins or restrictions
 *
 *****************************************************************************/

from_clause:
			FROM from_list							{ $$ = $2; }
			| /*EMPTY*/								{ $$ = NULL; }
		;

from_list:
			table_ref								{ $$ = list_make1($1); }
			| from_list ',' table_ref				{ $$ = lappend($1, $3); }
		;

where_clause:
			WHERE a_expr							{ $$ = $2; }
			| /*EMPTY*/								{ $$ = NULL; }
		;
/*
 * table_ref is where an alias clause can be attached.
 */
table_ref:	relation_expr opt_alias_clause
            {
                ((SQLTable*)$1)->alias_ = ($2 == NULL) ? NULL : new SQLBaseElem($2);
                $$ = $1;
            }
            ;

relation_expr:
			qualified_name
            {
                /* default inheritance */
                $$ = $1;
                ((SQLTable*)$$)->alias_ = NULL;
            }
            ;

alias_clause: ColId                                 { $$ = $1;}
            ;

opt_alias_clause: alias_clause						{ $$ = $1; }
			| /*EMPTY*/								{ $$ = NULL; }
		;

/*****************************************************************************
 *
 *	Names and constants
 *
 *****************************************************************************/

/*
 * The production for a qualified relation name has to exactly match the
 * production for a qualified func_name, because in a FROM clause we cannot
 * tell which we are parsing until we see what comes after it ('(' for a
 * func_name, something else for a relation). Therefore we allow 'indirection'
 * which may contain subscripts, and reject that case in the C code.
 */
qualified_name:
			ColId
            {
                $$ = new SQLTable(NULL, new SQLBaseElem($1));
            }
			| ColId indirection 
            {
                SQLTable* table = new SQLTable(NULL,NULL);
                $$ = table;
                if ( $2->size() > 0 ) {
                    table->schema_ = new SQLBaseElem($1);
                    table->table_ = (SQLBaseElem*)($2->back());
                    $2->pop_back();
                    release_list_object($2);
                } 
            };

attr_name:	ColLabel								{ $$ = $1; };

ColLabel:
            IDENT                                   { $$ = $1; }
            | unreserved_keyword					{ $$ = sql_strdup($1); }
			| col_name_keyword						{ $$ = sql_strdup($1); }
			| type_func_name_keyword				{ $$ = sql_strdup($1); }
			| reserved_keyword						{ $$ = sql_strdup($1); }
		;

ColId:
            IDENT                                   { $$ = $1; }
            | unreserved_keyword					{ $$ = sql_strdup($1); }
			| col_name_keyword						{ $$ = sql_strdup($1); }
		;
/*
 * Keyword category lists.  Generally, every keyword present in
 * the Postgres grammar should appear in exactly one of these lists.
 *
 * Put a new keyword into the first list that it can go into without causing
 * shift or reduce conflicts.  The earlier lists define "less reserved"
 * categories of keywords.
 *
 * Make sure that each keyword's category in kwlist.h matches where
 * it is listed here.  (Someday we may be able to generate these lists and
 * kwlist.h's table from a common master list.)
 */

/* "Unreserved" keywords --- available for use as any kind of name.
 */
unreserved_keyword:
            SET
            ;

/* Column identifier --- keywords that can be column, table, etc names.
 *
 * Many of these keywords will in fact be recognized as type or function
 * names too; but they have special productions for the purpose, and so
 * can't be treated as "generic" type or function names.
 *
 * The type names appearing here are not usable as function names
 * because they can be followed by '(' in typename productions, which
 * looks too much like a function call for an LR(1) parser.
 */
col_name_keyword:
            INT
            ;

/* Type/function identifier --- keywords that can be type or function names.
 *
 * Most of these are keywords that are used as operators in expressions;
 * in general such keywords can't be column names because they would be
 * ambiguous with variables, but they are unambiguous as function identifiers.
 *
 * Do not include POSITION, SUBSTRING, etc here since they have explicit
 * productions in a_expr to support the goofy SQL9x argument syntax.
 * - thomas 2000-11-28
 */
type_func_name_keyword:
            LEFT
            | RIGHT
            | LIKE
            ;

/* Reserved keyword --- these keywords are usable only as a ColLabel.
 *
 * Keywords appear here if they could not be distinguished from variable,
 * type, or function names in some contexts.  Don't put things here unless
 * forced to.
 */
reserved_keyword:
            SELECT
            | AS
            | WHERE
            | FROM
            | AND
            | OR
            ;
%%

static void
base_yyerror(YYLTYPE *yylloc, core_yyscan_t yyscanner, const char *msg)
{
	printf("base_yyerror:%s pos:%d\n",msg,*yylloc);
}

StatementBlock* make_stmt_block(SingleStatement* single_statement) {
    StatementBlock* block = new StatementBlock();
    block->push(single_statement);
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

static void release_list_object(List* list) {
    if ( list ) {
        for ( auto itr = list->begin(); itr != list->end() ; ++ itr) {
            delete (*itr);
        }
        delete list;
    }
}

char* sql_strdup(const char* source) {
    if ( source ) {
        int length = strlen(source);
        char* dup = (char*)Allocator::malloc(length + 1);
        strncpy(dup,source,length);
        dup[length] = 0;
        return dup;
    }
    return NULL;
}