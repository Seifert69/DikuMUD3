/*
 $Author: tperry $
 $RCSfile: intlist.cpp,v $
 $Date: 2001/04/29 03:46:06 $
 $Revision: 2.1 $
 */

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "intlist.h"
#include "textutil.h"

cintlist::cintlist(void)
{
    intlist = NULL;
    length = 0;
}

void cintlist::operator=(class cintlist *cn)
{
    for (sbit32 i = 0; i < cn->Length(); i++)
        Append((sbit32)cn->Value(i));
}

void cintlist::CopyList(class cintlist *cn)
{
    for (sbit32 i = 0; i < cn->Length(); i++)
        Append((sbit32)cn->Value(i));
}

void cintlist::AppendBuffer(CByteBuffer *pBuf)
{
    pBuf->Append32(Length());
    for (sbit32 i = 0; i < length; i++)
        pBuf->Append32((sbit32)Value(i));
}

int cintlist::ReadBuffer(CByteBuffer *pBuf)
{
    sbit32 len, i;
    sbit32 c;
    int n = 0;

    if (pBuf->Read32(&len))
        return 1;

    for (i = 0; i < len; i++)
    {
        n += pBuf->Read32(&c); // MS ought to sum corrupt here
        Append(c);
    }

    return n;
}

void cintlist::bread(ubit8 **b)
{
    sbit32 len, i;
    int c;
    len = bread_ubit32(b);

    for (i = 0; i < len; i++)
    {
        c = bread_ubit32(b);
        Append(c);
    }
}

char *cintlist::catnames()
{
    sbit32 i;
    char *s;
    unsigned int strsize;
    strsize = (Length() * 3) + 3;
    for (i = 0; i < Length(); i++)
        strsize = strsize + strlen(itoa(Value(i)));
    CREATE(s, char, strsize);
    strcpy(s, "{");
    if (Length() > 0)
    {
        for (i = 0; i < Length(); i++)
        {
            strcat(s, itoa(Value(i)));
            if (i < Length() - 1)
                strcat(s, ",");
        }
    }
    strcat(s, "}");
    return s;
}

std::string cintlist::json(void)
{
    string s;

    s = "\"intlist\": [";

    for (sbit32 i = 0; i < Length(); i++)
    {
        s.append(itoa(Value(i)));
        if (i < Length() - 1)
            s.append(",");
    }

    s.append("]");

    return s;
}

void cintlist::Remove(int idx)
{
    if ((idx >= 0) && (idx < length))
    {
        if (idx != length - 1)
            memmove(&intlist[idx], &intlist[idx + 1],
                    sizeof(int) * (length - idx));
        length--;
        if (length == 0)
        {
            FREE(intlist);
        }
        else
            RECREATE(intlist, int, length);
    }
}

void cintlist::Replace(int idx, int val)
{
    if ((idx >= 0) && (idx < length))
        intlist[idx] = val;
}

sbit32 cintlist::Value(int idx)
{
    if ((idx >= 0) && (idx < length))
        return intlist[idx];

    return 0;
}

sbit32 *cintlist::ValuePtr(int idx)
{
    if ((idx >= 0) && (idx < length))
        return &intlist[idx];

    return 0;
}

void cintlist::Free(void)
{
    if (intlist)
    {
        FREE(intlist);
        intlist = NULL;
    }
    length = 0;
}

cintlist::~cintlist(void)
{
    this->Free();
}

void cintlist::Append(int val)
{
    length++;

    if (intlist == NULL)
    {
        CREATE(intlist, int, length);
    }
    else
    {
        RECREATE(intlist, int, length);
    }
    intlist[length - 1] = val;
}

void cintlist::Prepend(int val)
{
    length++;
    if (intlist == NULL)
    {
        CREATE(intlist, int, length);
    }
    else
    {
        RECREATE(intlist, int, length);
    }
    if (length > 1)
        memmove(&intlist[1], &intlist[0], sizeof(int) * (length - 1));

    intlist[0] = val;
}


void cintlist::Insert(int val, int idx)
{
    if ((idx < 0) || (idx > length))
        return;

    if (idx == 0)
    {
        Prepend(val);
        return;
    }

    if (idx == length)
    {
        Append(val);
        return;
    }

    int olen = length;

    // We know the list is at least 2 large
    length++;

    assert(intlist != NULL);
    RECREATE(intlist, int, length);

    memmove(&intlist[idx + 1], &intlist[idx], sizeof(int) * (olen - idx));
    intlist[idx] = val;
}


cintlist *cintlist::Duplicate(void)
{
    cintlist *pNl = new cintlist;
    sbit32 i = 0;

    for (i = 0; i < length; i++)
        pNl->Append(intlist[i]);

    return pNl;
}
