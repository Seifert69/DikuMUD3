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
#include <cstdint>
#include <cstdio>
#include <cstdlib>
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

#define BITCONV16(i) ((((ubit16)i) >> 8) | ((((ubit16)i) & 255) << 8))
#define BITCONV32(i) ((((ubit32)i) >> 16) | ((((ubit32)i) & 65535) << 16))

#define ISNEWL(ch) ((ch) == '\n' || (ch) == '\r')

#define SGN(i) (((i) >= 0) ? 1 : -1)

#define ODD(i) ((i)&1)
#define EVEN(i) (!((i)&1))

#define TAIL(str) for (; *(str); (str)++)

#define STRCAT(p, c)                                                                                                                       \
    strcat(p, c);                                                                                                                          \
    TAIL(p)

#define STR(str) (str ? str : "(NIL POINTER)")

#define IF_STR(st) ((st) ? (st) : "")

#define ANA(c) (strchr("aeiouyAEIOUY", c) ? "an" : "a")

#define CAPC(st) (*(st) = toupper(*(st)))
#define CAP(st) (CAPC(st), (st))

/*define CREATE(res, type, num) \
  do { if (((res) = (type *) calloc((num), sizeof(type))) == NULL) \
         assert(FALSE); } while(0)
*/
/*define RECREATE(res, type, num) \
  do { if (((res) = (type *) realloc((res), sizeof(type) * (num))) == NULL) \
         assert(FALSE); } while(0)*/

#define IS_SET(flag, bit) ((flag) & (bit))

#define SET_BIT(var, bit) ((var) |= (bit))

#define REMOVE_BIT(var, bit) ((var) &= ~(bit))

#define TOGGLE_BIT(var, bit) ((var) ^= (bit))

#define SWITCH(a, b)                                                                                                                       \
    {                                                                                                                                      \
        (a) ^= (b);                                                                                                                        \
        (b) ^= (a);                                                                                                                        \
        (a) ^= (b);                                                                                                                        \
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

#define CREATE(res, type, num)                                                                                                             \
    if (((res) = (type *)calloc((num), sizeof(type))) == nullptr)                                                                          \
    {                                                                                                                                      \
        /* Make sure CREATE isn't being used to create the new classes instead of the structs they used to be */                           \
        static_assert(std::is_pod_v<type>);                                                                                                \
        assert(FALSE);                                                                                                                     \
    }

#define RECREATE(res, type, num)                                                                                                           \
    if (((res) = (type *)realloc((res), sizeof(type) * (num))) == nullptr)                                                                 \
    {                                                                                                                                      \
        /* Make sure RECREATE isn't being used to create the new classes instead of the structs they used to be */                         \
        static_assert(std::is_pod_v<type>);                                                                                                \
        assert(FALSE);                                                                                                                     \
    }

#define FREE(p)                                                                                                                            \
    {                                                                                                                                      \
        /* Make sure FREE isn't being used to free class items created with EMPLACE */                                                     \
        static_assert(std::is_same_v<std::remove_reference_t<std::remove_all_extents_t<decltype(p)>>, void *> ||                           \
                      std::is_pod_v<std::remove_pointer_t<std::remove_reference_t<std::remove_all_extents_t<decltype(p)>>>>);              \
        free(p);                                                                                                                           \
        p = nullptr;                                                                                                                       \
    }
