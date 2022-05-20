/*
 $Author: All $
 $RCSfile: essential.h,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.6 $
 */
#pragma once

#include "membug.h"

#include <sys/types.h>

#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <type_traits>

#ifdef _WINDOWS
    #include <ctype.h>
char *crypt(char *salt, char *password);
#endif

enum log_level
{
    LOG_OFF,
    LOG_BRIEF,
    LOG_EXTENSIVE,
    LOG_ALL,
    LOG_DIL
};

#define MAX_STRING_LENGTH 12288
#define MAX_INPUT_LENGTH 512

#define TRUE 1
#define FALSE 0

template<typename T>
[[maybe_unused]] T BITCONV16(T i)
{
    return (((uint16_t)i >> 8) | (((uint16_t)i & 255) << 8));
}
template<typename T>
[[maybe_unused]] T BITCONV32(T i)
{
    return (((uint32_t)i >> 16) | (((uint32_t)i & 65535) << 16));
}

inline bool ISNEWL(char ch)
{
    return ch == '\n' || ch == '\r';
}

[[maybe_unused]] inline int SGN(int64_t i)
{
    return i >= 0 ? 1 : -1;
}

template<typename Type>
inline bool ODD(Type i)
{
    return i & 1;
}
template<typename Type>
[[maybe_unused]] inline bool EVEN(Type i)
{
    return !(i & 1);
}

template<typename T>
inline void TAIL(T *&str)
{
    for (; *str; str++)
    {
        ;
    }
}

[[maybe_unused]] inline void STRCAT(char *&p, const char *c)
{
    strcat(p, c);
    TAIL(p);
}

inline const char *STR(const char *str)
{
    return str ? str : "(NIL POINTER)";
}

inline const char *IF_STR(const char *st)
{
    return st ? st : "";
}

inline const char *ANA(char c)
{
    return strchr("aeiouyAEIOUY", c) ? "an" : "a";
}

inline void CAPC(char *st)
{
    *st = static_cast<char>(toupper(*st));
}

inline char *CAP(char *st)
{
    CAPC(st);
    return st;
}

template<typename Flag, typename Bit>
inline bool IS_SET(Flag flag, Bit bit)
{
    return flag & bit;
}

template<typename Flag, typename Bit>
inline void SET_BIT(Flag &&var, Bit bit)
{
    var |= bit;
}

template<typename Flag, typename Bit>
inline Flag REMOVE_BIT(Flag &&var, Bit bit)
{
    var &= ~bit;
    return var;
}

template<typename Flag, typename Bit>
[[maybe_unused]] inline void TOGGLE_BIT(Flag &&var, Bit bit)
{
    var ^= bit;
}

template<typename T>
[[maybe_unused]] inline void SWITCH(T &a, T &b)
{
    a ^= b;
    b ^= a;
    a ^= b;
}
#define DEBUG(a)                                                                                                                           \
    do                                                                                                                                     \
    {                                                                                                                                      \
        fprintf(stderr, a);                                                                                                                \
    } while (0)

using sbit8 = int8_t;
using ubit8 = uint8_t;
using sbit16 = int16_t;
using ubit16 = uint16_t;
using sbit32 = int32_t;
using ubit32 = uint32_t;
using sbit64 = int64_t;  // MS2020
using ubit64 = uint64_t; // MS2020
using ubit1 = bool;      /* Boolean */

template<typename Type, typename Result>
inline void create_memory(Result &&res, size_t num)
{
    /* Make sure CREATE isn't being used to create the new classes instead of the structs they used to be */
    static_assert(std::is_pod_v<Type>);
    if (res = reinterpret_cast<Type *>(calloc(num, sizeof(Type))); res == nullptr)
    {
        assert(FALSE);
    }
}
#define CREATE(res, type, num) create_memory<type>(res, num)

template<typename Type, typename Result>
inline void recreate_memory(Result &&res, size_t num)
{
    /* Make sure RECREATE isn't being used to create the new classes instead of the structs they used to be */
    static_assert(std::is_pod_v<Type>);
    if (res = reinterpret_cast<Type *>(realloc(res, sizeof(Type) * num)); res == nullptr)
    {
        assert(FALSE);
    }
}
#define RECREATE(res, type, num) recreate_memory<type>(res, num)

template<typename Type>
inline void FREE(Type &&p)
{
    /* Make sure FREE isn't being used to free class items created with EMPLACE */
    static_assert(std::is_same_v<std::remove_reference_t<std::remove_all_extents_t<decltype(p)>>, void *> ||
                  std::is_pod_v<std::remove_pointer_t<std::remove_reference_t<std::remove_all_extents_t<decltype(p)>>>>);
    free(p);
    p = nullptr;
}
