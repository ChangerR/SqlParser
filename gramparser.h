#ifndef __SQLPARSER_GRAMPARSER_H
#define __SQLPARSER_GRAMPARSER_H
#include "sql.hpp"

class StatementBlock;
typedef struct base_yy_extra_type
{
    StatementBlock* block;
} base_yy_extra_type;

#define sp_yyget_extra(yyscanner) (*((base_yy_extra_type **) (yyscanner)))

/*
 * The type of yyscanner is opaque outside scan.l.
 */
typedef void *core_yyscan_t;

/* from parser.c */
extern int base_yylex(YYSTYPE *lvalp, YYLTYPE *llocp,
		   core_yyscan_t yyscanner);

/* from gram.y */
extern void parser_init(base_yy_extra_type *yyext);
extern int	base_yyparse(core_yyscan_t yyscanner);

#endif