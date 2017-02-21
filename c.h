#ifndef __SQLPARSER_C_H
#define __SQLPARSER_C_H

typedef short int16;
typedef size_t Size;
typedef int int32;

/*
 * Maximum length for identifiers (e.g. table names, column names,
 * function names).  Names actually are limited to one less byte than this,
 * because the length must include a trailing zero byte.
 *
 * Changing this requires an initdb.
 */
 
#define NAMEDATALEN 64

/*
 * lengthof
 *		Number of elements in an array.
 */
#define lengthof(array) (sizeof (array) / sizeof ((array)[0]))

#endif