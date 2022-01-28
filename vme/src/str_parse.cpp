/*
 $Author: tperry $
 $RCSfile: str_parse.cpp,v $
 $Date: 2001/04/29 03:46:07 $
 $Revision: 2.1 $
 */

#include "essential.h"
#include "textutil.h"

#include <cassert>
#include <cctype>
#include <cstdlib>
#include <cstring>

char *parse_match(const char *pData, const char *pMatch)
{
    char *pTmp1 = nullptr;
    char *pTmp2 = nullptr;

    pTmp1 = (char *)str_cstr(pData, pMatch);
    if (pTmp1 == nullptr)
    {
        return nullptr;
    }

    pTmp2 = skip_blanks(pTmp1 + strlen(pMatch));
    if (*pTmp2 != '=')
    {
        return nullptr;
    }

    pTmp2 = skip_blanks(pTmp2 + 1);

    if (*pTmp2)
    {
        return pTmp2;
    }
    else
    {
        return nullptr;
    }
}

char *parse_name(char **pData)
{
    char Buf[MAX_STRING_LENGTH];
    char *pTmp1 = nullptr;
    char *pTmp2 = nullptr;

    Buf[0] = 0;

    pTmp1 = skip_blanks(*pData);
    if (*pTmp1 != '~')
    {
        return nullptr;
    }

    pTmp1++;

    for (pTmp2 = pTmp1; *pTmp2; pTmp2++)
    {
        if (*pTmp2 == '~')
        {
            break;
        }
    }

    if (*pTmp2 != '~')
    {
        return nullptr;
    }

    memcpy(Buf, pTmp1, pTmp2 - pTmp1);
    Buf[pTmp2 - pTmp1] = 0;

    *pData = pTmp1 + (1 + pTmp2 - pTmp1); /* +1 to skip the last ~ */

    return str_dup(Buf);
}

int parse_num(char **pData, int *pNum)
{
    char *pTmp1 = nullptr;

    *pNum = 0;

    pTmp1 = skip_blanks(*pData);
    if (!isdigit(*pTmp1) && *pTmp1 != '+' && *pTmp1 != '-')
    {
        return FALSE;
    }

    *pNum = atoi(pTmp1);
    if (*pTmp1 == '+' || *pTmp1 == '-')
    {
        pTmp1++;
    }

    while (isdigit(*pTmp1))
    {
        pTmp1++;
    }

    if (!isspace(*pTmp1) && *pTmp1 != 0)
    {
        return FALSE;
    }

    *pData = pTmp1;

    return TRUE;
}

int *parse_numlist(char **pData, int *int_count)
{
    int *nums = nullptr;
    int count = 0;
    int i = 0;

    while (parse_num(pData, &i))
    {
        count++;
        if (count == 1)
        {
            CREATE(nums, int, count);
        }
        else
        {
            RECREATE(nums, int, count);
        }
        nums[count - 1] = i;
        while ((isspace(**pData) || ispunct(**pData)))
        {
            (*pData)++;
        }
    }

    *int_count = count;
    return nums;
}

char **parse_namelist(char **pData)
{
    char *pTmp1 = nullptr;
    char *pTmp2 = nullptr;
    char **pNamelist = nullptr;

    pTmp1 = *pData;

    pNamelist = create_namelist();

    while ((pTmp2 = parse_name(&pTmp1)))
    {
        pNamelist = add_name(pTmp2, pNamelist);
        while (*pTmp1 != '~' && (isspace(*pTmp1) || ispunct(*pTmp1)))
        {
            pTmp1++;
        }
        FREE(pTmp2);
    }

    if (pNamelist[0] == nullptr)
    {
        free_namelist(pNamelist);
        return nullptr;
    }

    *pData = pTmp1;

    return pNamelist;
}

int parse_match_num(const char **pData, const char *pMatch, int *pResult)
{
    char *pTmp = nullptr;

    pTmp = parse_match(*pData, pMatch);
    if (pTmp == nullptr)
    {
        return FALSE;
    }

    if (!(parse_num(&pTmp, pResult)))
    {
        return FALSE;
    }

    *pData = pTmp;

    return TRUE;
}

int *parse_match_numlist(const char **pData, const char *pMatch, int *count)
{
    char *pTmp = nullptr;
    int *i = nullptr;

    *count = 0;
    pTmp = parse_match(*pData, pMatch);
    if (pTmp == nullptr)
    {
        return nullptr;
    }

    i = parse_numlist(&pTmp, count);

    *pData = pTmp;

    return i;
}

char *parse_match_name(const char **pData, const char *pMatch)
{
    char *pName = nullptr;
    char *pTmp = nullptr;

    pTmp = parse_match(*pData, pMatch);
    if (pTmp == nullptr)
    {
        return nullptr;
    }

    pName = parse_name(&pTmp);

    *pData = pTmp;

    return pName;
}

char **parse_match_namelist(const char **pData, const char *pMatch)
{
    char **pNamelist = nullptr;
    char *pTmp = nullptr;

    pTmp = parse_match(*pData, pMatch);
    if (pTmp == nullptr)
    {
        return nullptr;
    }

    pNamelist = parse_namelist(&pTmp);

    *pData = pTmp;

    return pNamelist;
}
