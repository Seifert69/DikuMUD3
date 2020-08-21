#ifndef _MEMBUG_H
#define _MEMBUG_H


#ifndef MEMORY_DEBUG
    #define membug_verify(ptr)  // NIL
    #define membug_verify_class(ptr)  // NIL
    #define membug_verify_reg(ptr)  // NIL

    #define EMPLACE(t)
    #define DELETE(cls,ptr) delete ptr;
#else
    #define DELETE(cls, ptr) { ptr->~cls(); membug_delete(ptr); }
    #define EMPLACE(t)  (membug_new(sizeof(t)))

    void membug_verify(void *ptr);

    void *membug_malloc(size_t size);
    void *membug_calloc(size_t nmemb, size_t size);
    void *membug_realloc(void *ptr, size_t size);
    void membug_free(void *ptr);
    void membug_verify_reg(void *ptr);

    void *membug_new(size_t size);
    void membug_delete(void *ptr);
    void membug_verify_class(void *ptr);

    #define free membug_free
    #define calloc membug_calloc
    #define malloc membug_malloc
    #define realloc membug_realloc
#endif

#endif
