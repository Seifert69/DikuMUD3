#include <string.h>
#include "bytestring.h"

// MS2020 to validate integrity of MUD memory allocation
// 

#ifdef MEMORY_DEBUG

#undef malloc
#undef calloc
#undef realloc
#undef free



void membug_setfree(void *ptr)
{
    size_t size;
    ubit8 *p = (ubit8 *) ptr;

    p-=12;
    p+=8;
    size = bread_ubit32(&p);
    assert(size > 16+4);

    p = (ubit8 *) ptr;
    p -= 12;

    for (size_t i=0; i < 8; i++)
    {
        assert(p[i] == 42);
        p[i] = 43;
    }

    for (size_t i=0; i < 8; i++)
    {
        assert(p[size-8+i] == 42);
        p[size-8+i] = 43;        
    }
}


void membug_verify(void *ptr)
{
    size_t size;
    ubit8 *p = (ubit8 *) ptr;

    if (!ptr)
        return;

    p-=12;
    p+=8;
    size = bread_ubit32(&p);
    assert(size > 16+4);

    p = (ubit8 *) ptr;
    p -= 12;

    for (size_t i=0; i < 8; i++)
    {
        assert(p[i] == 42);
    }

    for (size_t i=0; i < 8; i++)
    {
        assert(p[size-8+i] == 42);
    }
}

void *membug_malloc(size_t size)
{
    ubit8 *p;

    assert(size > 0);

    size += 16+4;
    p = (ubit8 *) malloc(size);
    assert(p);

    // The first and last 8 bytes are 42 per byte
    memset(p,42,8);
    ubit8 *u = p+8;
    bwrite_ubit32(&u, size);
    memset(p+size-8,42,8);
    membug_verify(p+12);

    return p+12;
}




// 
// Use this for new emplacement. 
//
void *membug_new(size_t size)
{
    ubit8 *p;

    assert(size > 0);

    size += 16+4;
    p = (ubit8 *) malloc(size);
    assert(p);

    // The first and last 8 bytes are 42 per byte
    memset(p,42,8);
    ubit8 *u = p+8;
    bwrite_ubit32(&u, size);
    memset(p+size-8,42,8);
    membug_verify(p+12);

    return p+12;
}



void *membug_calloc(size_t nmemb, size_t size)
{
    ubit8 *p;

    assert(size*nmemb > 0);

    size = size * nmemb + 16+4;
    p = (ubit8 *) calloc(size, 1);
    assert(p);

    // The first and last 8 bytes are 42 per byte
    memset(p,42,8);
    ubit8 *u = p+8;
    bwrite_ubit32(&u, size);
    memset(p+size-8,42,8);

    membug_verify(p+12);

    return p+12;
}

void *membug_realloc(void *ptr, size_t size)
{
    ubit8 *p;

    membug_verify(ptr);

    assert(size > 0);

    size += 16+4;

    p = (ubit8 *) ptr;
    p -= 12;
    p = (ubit8 *) realloc(p, size);
    assert(p);

    // The first and last 8 bytes are 42 per byte
    memset(p,42,8);
    ubit8 *u = p+8;
    bwrite_ubit32(&u, size);
    memset(p+size-8,42,8);

    membug_verify(p+12);

    return p+12;
}

void membug_free(void *ptr)
{
    membug_verify(ptr);
    membug_setfree(ptr);
    ubit8 *p = (ubit8 *) ptr;
    p-=12;
    free(p);
}

#endif
