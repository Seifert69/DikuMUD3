#pragma once

#include <cassert>
#include <cstdlib>
#include <iostream>

#define DEFAULT_T_ARRAY_SIZE 10
#define DEFAULT_T_ARRAY_EXTEND 5

template<class T>
class t_array
{
private:
    int size;  // Malloc'd Size of the current array
    int esize; // Extending size, how many elements to allocate
    int len;   // Current Number of Elements
    T **t_ary; // The actual array
public:
    t_array();
    ~t_array();
    t_array(int esz);
    void init(int esz);
    void destroy();
    void push(T *ob);
    T *pop();
    inline int length() { return len; }
    T *operator[](int i);
};

#ifndef CREATE
    #define CREATE(res, type, num)                                                                                                         \
        if (((res) = (type *)calloc((num), sizeof(type))) == NULL)                                                                         \
            assert(0);
#endif

#ifndef RECREATE
    #define RECREATE(res, type, num)                                                                                                       \
        if (((res) = (type *)realloc((res), sizeof(type) * (num))) == NULL)                                                                \
            assert(0);
#endif
// Indexs of the elements are from 1-x not 0-x, this allows for
// negative indexs as well as positive, ie:  myarray[-1] would be the
// last element in the list, and myarray[1] would be the first element.
// Since zero would be an unknown on the intended top or bottom it will
// return NULL, anything outside the length will also return NULL.
template<class T>
T *t_array<T>::operator[](int i)
{
    if (i > 0)
    {
        if (i <= len)
            return (t_ary[i - 1]);
    }
    else if (-i <= len)
        return (t_ary[len + i]);
    assert(1 == 0);
    return NULL;
}

template<class T>
t_array<T>::t_array()
{
    size = DEFAULT_T_ARRAY_SIZE;
    esize = DEFAULT_T_ARRAY_EXTEND;
    len = 0;
    CREATE(t_ary, T *, size);
}

template<class T>
t_array<T>::~t_array<T>()
{
    for (int i = 0; i < len; i++)
        delete t_ary[i];

    if (t_ary)
        free(t_ary);
    t_ary = NULL;
}

template<class T>
void t_array<T>::destroy()
{
    for (int i = 0; i < len; i++)
        delete t_ary[i];

    if (t_ary)
        free(t_ary);
    t_ary = NULL;
}

template<class T>
t_array<T>::t_array(int esz)
{
    size = esz;
    esize = esz;
    len = 0;
    if (esz == 0)
    {
        esize++;
        t_ary = NULL;
    }
    else
        CREATE(t_ary, T *, size);
}

template<class T>
void t_array<T>::init(int esz)
{
    size = esz;
    esize = esz;
    len = 0;
    if (esz == 0)
    {
        esize++;
        t_ary = NULL;
    }
    else
        CREATE(t_ary, T *, size);
}

template<class T>
void t_array<T>::push(T *ob)
{
    if ((size - len) < 1)
    {
        size += esize;
        RECREATE(t_ary, T *, size);
    }

    t_ary[len++] = ob;
}

template<class T>
T *t_array<T>::pop()
{
    assert(len > 0);
    /*	if ( (size-len) > esize)
        {
                  size-=esize;
                        RECREATE(t_ary, T *, size);
        } */

    return (t_ary[--len]);
}
