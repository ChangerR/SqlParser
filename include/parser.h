#ifndef __SQLPARSER_PARSER_H
#define __SQLPARSER_PARSER_H
class StatementBlock;
typedef enum
{
	BACKSLASH_QUOTE_OFF,
	BACKSLASH_QUOTE_ON,
	BACKSLASH_QUOTE_SAFE_ENCODING
}	BackslashQuoteType;

/* GUC variables in scan.l (every one of these is a bad idea :-() */
extern int	backslash_quote;
extern bool escape_string_warning;
extern bool standard_conforming_strings;


/* Primary entry point for the raw parsing functions */
extern __cdecl StatementBlock *raw_parser(const char *str);

#endif