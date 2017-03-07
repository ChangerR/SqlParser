#ifndef __SQLPARSER_SQLPARSER_H
#define __SQLPARSER_SQLPARSER_H
#include "keywords.h"

/*
 * The YY_EXTRA data that a flex scanner allows us to pass around.
 * Private state needed by the core scanner goes here.  Note that the actual
 * yy_extra struct may be larger and have this as its first component, thus
 * allowing the calling parser to keep some fields of its own in YY_EXTRA.
 */
typedef struct core_yy_extra_type
{
	/*
	 * The string the scanner is physically scanning.  We keep this mainly so
	 * that we can cheaply compute the offset of the current token (yytext).
	 */
	char	   *scanbuf;
	Size		scanbuflen;

	/*
	 * The keyword list to use.
	 */
	const ScanKeyword *keywords;
	int			num_keywords;

	/*
	 * Scanner settings to use.  These are initialized from the corresponding
	 * GUC variables by scanner_init().  Callers can modify them after
	 * scanner_init() if they don't want the scanner's behavior to follow the
	 * prevailing GUC settings.
	 */
	int			backslash_quote;
	bool		escape_string_warning;
	bool		standard_conforming_strings;

	/*
	 * literalbuf is used to accumulate literal values when multiple rules are
	 * needed to parse a single literal.  Call startlit() to reset buffer to
	 * empty, addlit() to add text.  NOTE: the string in literalbuf is NOT
	 * necessarily null-terminated, but there always IS room to add a trailing
	 * null at offset literallen.  We store a null only when we need it.
	 */
	char	   *literalbuf;		/* palloc'd expandable buffer */
	int			literallen;		/* actual current string length */
	int			literalalloc;	/* current allocated buffer size */

	int			xcdepth;		/* depth of nesting in slash-star comments */
	char	   *dolqstart;		/* current $foo$ quote start string */

	/* first part of UTF16 surrogate pair for Unicode escapes */
	int32		utf16_first_part;

	/* state variables for literal-lexing warnings */
	bool		warn_on_first_escape;
	bool		saw_non_ascii;
} core_yy_extra_type;

/*
 * The scanner returns extra data about scanned tokens in this union type.
 * Note that this is a subset of the fields used in YYSTYPE of the bison
 * parsers built atop the scanner.
 */
typedef union core_YYSTYPE
{
	int			ival;			/* for integer literals */
	char	   *str;			/* for identifiers and non-integer literals */
	const char *keyword;		/* canonical spelling of keywords */
} core_YYSTYPE;

/*
 * We track token locations in terms of byte offsets from the start of the
 * source string, not the column number/line number representation that
 * bison uses by default.  Also, to minimize overhead we track only one
 * location (usually the first token location) for each construct, not
 * the beginning and ending locations as bison does by default.  It's
 * therefore sufficient to make YYLTYPE an int.
 */
#define YYLTYPE  int 

/*
 * The type of yyscanner is opaque outside scan.l.
 */
typedef void *core_yyscan_t;

extern core_yyscan_t scanner_init(const char *str,
			 core_yy_extra_type *yyext,
			 const ScanKeyword *keywords,
			 int num_keywords);

extern void scanner_finish(core_yyscan_t yyscanner);

extern int core_yylex(core_YYSTYPE *lvalp, YYLTYPE *llocp,
		   core_yyscan_t yyscanner);

extern void scanner_yyerror(const char *message, core_yyscan_t yyscanner);

#endif