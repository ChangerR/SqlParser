#ifndef __SQLPARSER_ALLOCATOR_H
#define __SQLPARSER_ALLOCATOR_H
#include "c.h"

class Allocator {
public:

    static void* malloc(Size size);

    static void free(void* ptr);

    static void* remalloc(void* ptr,Size size);

};
#endif