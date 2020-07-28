/*
 $Author: tperry $
 $RCSfile: str_parse.cpp,v $
 $Date: 2001/04/29 03:46:07 $
 $Revision: 2.1 $
 */

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "essential.h"
#include "textutil.h"

char *
parse_match (const char *pData, const char *pMatch)
{
    char *pTmp1, *pTmp2;

    pTmp1 = (char *) str_cstr (pData, pMatch);
    if (pTmp1 == NULL)
        return NULL;

    pTmp2 = skip_blanks (pTmp1 + strlen (pMatch));
    if (*pTmp2 != '=')
        return NULL;

    pTmp2 = skip_blanks (pTmp2 + 1);

    if (*pTmp2)
        return pTmp2;
    else
        return NULL;
}

char *
parse_name (char **pData)
{
    char Buf[MAX_STRING_LENGTH];
    char *pTmp1, *pTmp2;

    Buf[0] = 0;

    pTmp1 = skip_blanks (*pData);
    if (*pTmp1 != '~')
        return NULL;

    pTmp1++;

    for (pTmp2 = pTmp1; *pTmp2; pTmp2++)
        if (*pTmp2 == '~')
            break;

    if (*pTmp2 != '~')
        return NULL;

    memcpy (Buf, pTmp1, pTmp2 - pTmp1);
    Buf[pTmp2 - pTmp1] = 0;

    *pData = pTmp1 + (1 + pTmp2 - pTmp1);	/* +1 to skip the last ~ */

    return str_dup (Buf);
}

int
parse_num (char **pData, int *pNum)
{
    char *pTmp1;

    *pNum = 0;

    pTmp1 = skip_blanks (*pData);
    if (!isdigit (*pTmp1) && *pTmp1 != '+' && *pTmp1 != '-')
        return FALSE;

    *pNum = atoi (pTmp1);
    if (*pTmp1 == '+' || *pTmp1 == '-')
        pTmp1++;

    while (isdigit (*pTmp1))
        pTmp1++;

    if (!isspace (*pTmp1) && *pTmp1 != 0)
        return FALSE;

    *pData = pTmp1;

    return TRUE;
}

int *
parse_numlist (char **pData, int *int_count)
{
    int *nums = NULL;
    int count = 0, i;

    while (parse_num (pData, &i))
    {
        count++;
        if (count == 1)
        {
            CREATE (nums, int, count);
        }
        else
        {
            RECREATE (nums, int, count);
        }
        nums[count - 1] = i;
        while ((isspace (**pData) || ispunct (**pData)))
            (*pData)++;
    }

    *int_count = count;
    return nums;
}

char **
parse_namelist (char **pData)
{
    char *pTmp1, *pTmp2;
    char **pNamelist;

    pTmp1 = *pData;

    pNamelist = create_namelist ();

    while ((pTmp2 = parse_name (&pTmp1)))
    {
        pNamelist = add_name (pTmp2, pNamelist);
        while (*pTmp1 != '~' && (isspace (*pTmp1) || ispunct (*pTmp1)))
            pTmp1++;
        FREE (pTmp2);
    }

    if (pNamelist[0] == NULL)
    {
        free_namelist (pNamelist);
        return NULL;
    }

    *pData = pTmp1;

    return pNamelist;
}

int
parse_match_num (const char **pData, const char *pMatch, int *pResult)
{
    char *pTmp;

    pTmp = parse_match (*pData, pMatch);
    if (pTmp == NULL)
        return FALSE;

    if (!(parse_num (&pTmp, pResult)))
        return FALSE;

    *pData = pTmp;

    return TRUE;
}

int *
parse_match_numlist (const char **pData, const char *pMatch, int *count)
{
    char *pTmp;
    int *i;

    *count = 0;
    pTmp = parse_match (*pData, pMatch);
    if (pTmp == NULL)
        return NULL;

    i = parse_numlist (&pTmp, count);

    *pData = pTmp;

    return i;
}

char *
parse_match_name (const char **pData, const char *pMatch)
{
    char *pName, *pTmp;

    pTmp = parse_match (*pData, pMatch);
    if (pTmp == NULL)
        return NULL;

    pName = parse_name (&pTmp);

    *pData = pTmp;

    return pName;
}

char **
parse_match_namelist (const char **pData, const char *pMatch)
{
    char **pNamelist, *pTmp;

    pTmp = parse_match (*pData, pMatch);
    if (pTmp == NULL)
        return NULL;

    pNamelist = parse_namelist (&pTmp);

    *pData = pTmp;

    return pNamelist;
}
