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
    int size;  ///< Malloc'd Size of the current array
    int esize; ///< Extending size, how many elements to allocate
    int len;   ///< Current Number of Elements
    T **t_ary; ///< The actual array
public:
    t_array();
    ~t_array();
    explicit t_array(int esz);
    void init(int esz);
    void destroy();
    void push(T *ob);
    T *pop();
    inline int length() { return len; }
    T *operator[](int i);
};

// Indexs of the elements are from 1-x not 0-x, this allows for
// negative indexs as well as positive, ie:  myarray[-1] would be the
// last element in the list, and myarray[1] would be the first element.
// Since zero would be an unknown on the intended top or bottom it will
// return nullptr, anything outside the length will also return nullptr.
template<class T>
T *t_array<T>::operator[](int i)
{
    if (i > 0)
    {
        if (i <= len)
        {
            return (t_ary[i - 1]);
        }
    }
    else if (-i <= len)
    {
        return (t_ary[len + i]);
    }
    assert(1 == 0);
    return nullptr;
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
    {
        if constexpr (std::is_pod_v<T>)
        {
            FREE(t_ary[i]);
        }
        else
        {
            DELETE(T, t_ary[i]);
        }
    }

    if (t_ary)
    {
        FREE(t_ary);
    }
    t_ary = nullptr;
}

template<class T>
void t_array<T>::destroy()
{
    for (int i = 0; i < len; i++)
    {
        if constexpr (std::is_pod_v<T>)
        {
            FREE(t_ary[i]);
        }
        else
        {
            DELETE(T, t_ary[i]);
        }
    }

    if (t_ary)
    {
        FREE(t_ary);
    }
    t_ary = nullptr;
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
        t_ary = nullptr;
    }
    else
    {
        CREATE(t_ary, T *, size);
    }
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
        t_ary = nullptr;
    }
    else
    {
        CREATE(t_ary, T *, size);
    }
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
