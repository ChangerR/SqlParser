#ifndef __SQLPARSER_GRAMPARSER_H
#define __SQLPARSER_GRAMPARSER_H
#include "ast/SQLNode.h"
#include "ast/SingleStatement.h"
#include "ast/SelectStatement.h"
#include "ast/StatementBlock.h"
#include "scanner.h"
#include "sql.hpp"

typedef struct base_yy_extra_type
{
    /*
	 * Fields used by the core scanner.
	 */
	core_yy_extra_type core_yy_extra;

	/*
	 * State variables for base_yylex().
	 */
	bool		have_lookahead; /* is lookahead info valid? */
	int			lookahead_token;	/* one-token lookahead */
	core_YYSTYPE lookahead_yylval;		/* yylval for lookahead token */
	YYLTYPE		lookahead_yylloc;		/* yylloc for lookahead token */
	char	   *lookahead_end;	/* end of current token */
	char		lookahead_hold_char;	/* to be put back at *lookahead_end */

    StatementBlock* block;
} base_yy_extra_type;

/*
 * In principle we should use yyget_extra() to fetch the yyextra field
 * from a yyscanner struct.  However, flex always puts that field first,
 * and this is sufficiently performance-critical to make it seem worth
 * cheating a bit to use an inline macro.
 */
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