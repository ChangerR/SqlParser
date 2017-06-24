#include "allocator.h"
#include "gperftools/tcmalloc.h"

void *Allocator::malloc(Size size)
{
    return tc_malloc(size);
}

void Allocator::free(void *ptr)
{
    tc_free(ptr);
}

void *Allocator::remalloc(void *ptr, Size size)
{
    return tc_realloc(ptr, size);
}