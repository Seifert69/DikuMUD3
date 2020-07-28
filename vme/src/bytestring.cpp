/*
 $Author: tperry $
 $RCSfile: bytestring.cpp,v $
 $Date: 2002/06/09 23:27:39 $
 $Revision: 2.2 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "essential.h"
#include "bytestring.h"
#include "textutil.h"

/* =================================================================== */
/* =================================================================== */

CByteBuffer::CByteBuffer(ubit32 nSize)
{
    assert(nSize > 0);
    m_nLength = 0;
    m_nReadPos = 0;
    m_nAllocated = nSize;
    CREATE(m_pData, ubit8, nSize);

    assert(m_pData != NULL);
    *m_pData = 0;
}

CByteBuffer::~CByteBuffer(void)
{
    FREE(m_pData);
}

void CByteBuffer::SetReadPosition(ubit32 nReadPosition)
{
    m_nReadPos = nReadPosition;
}

void CByteBuffer::SetLength(ubit32 nLen)
{
    m_nLength = nLen;
}

void CByteBuffer::SetSize(ubit32 nSize)
{
    assert(nSize > 0);
    RECREATE(m_pData, ubit8, nSize);
    assert(m_pData);
    m_nAllocated = nSize;
}

void CByteBuffer::SetData(ubit8 *pData, ubit32 nSize)
{
    assert(nSize > 0);

    if (m_pData)
        FREE(m_pData);
    Clear();

    m_pData = pData;
    assert(m_pData);

    m_nAllocated = nSize;
    m_nLength = nSize;
}

void CByteBuffer::IncreaseSize(ubit32 nAdd)
{
    if (nAdd > m_nAllocated)
        m_nAllocated += nAdd + 128;
    else
        m_nAllocated *= 2;

    SetSize(m_nAllocated);
}

int CByteBuffer::FileRead(FILE *f, ubit32 nLength)
{
    Clear();

    if (m_nAllocated < nLength)
        IncreaseSize(nLength - m_nAllocated + 1);

    int n = fread(m_pData, 1, nLength, f);

    if (n > 0)
        m_nLength = n;

    return n;
}

int CByteBuffer::FileWrite(FILE *f)
{

    if (m_nLength > 0)
        return fwrite(m_pData, 1, m_nLength, f);
    else
        return 0;
}

int CByteBuffer::FileRead(FILE *f, long nOffset, ubit32 nLength)
{
    Clear();

    if (m_nAllocated < nLength)
        IncreaseSize(nLength - m_nAllocated + 1);

    if (fseek(f, nOffset, SEEK_SET))
        return -1;

    int n = fread(m_pData, 1, nLength, f);

    if (n > 0)
        m_nLength = nLength;

    return n;
}

// Return 0 if OK, return 1 if not OK
int CByteBuffer::Read(ubit8 *pBuf, ubit32 nLen)
{
    if (m_nReadPos + nLen > m_nLength)
        return 1;

    memcpy(pBuf, m_pData + m_nReadPos, nLen);
    m_nReadPos += nLen;

    return 0;
}

int CByteBuffer::Read8(ubit8 *pNum)
{
    return Read(pNum, sizeof(ubit8));
}

int CByteBuffer::Read8(sbit8 *pNum)
{
    return Read((ubit8 *)pNum, sizeof(ubit8));
}

int CByteBuffer::Read16(ubit16 *pNum)
{
    return Read((ubit8 *)pNum, sizeof(ubit16));
}

int CByteBuffer::Read16(sbit16 *pNum)
{
    return Read((ubit8 *)pNum, sizeof(sbit16));
}

int CByteBuffer::Read32(ubit32 *pNum)
{
    return Read((ubit8 *)pNum, sizeof(ubit32));
}

int CByteBuffer::Read32(sbit32 *pNum)
{
    return Read((ubit8 *)pNum, sizeof(sbit32));
}

int CByteBuffer::ReadFloat(float *pFloat)
{
    return Read((ubit8 *)pFloat, sizeof(float));
}

int CByteBuffer::ReadStringCopy(char *pStr, ubit32 nMaxSize)
{
    ubit32 nLen = 1 + strlen((char *)m_pData + m_nReadPos);

    if (nLen > nMaxSize)
    {
        pStr[0] = 0;
        return 1;
    }

    return Read((ubit8 *)pStr, nLen);
}

int CByteBuffer::ReadStringAlloc(char **ppStr)
{
    int nLen = 1 + strlen((char *)m_pData + m_nReadPos);

    if (nLen == 1)
    {
        *ppStr = NULL;
        return Skip8();
    }

    *ppStr = (char *)malloc(nLen);

    assert(*ppStr);

    return Read((ubit8 *)*ppStr, nLen);
}

int CByteBuffer::ReadNames(char ***pppStr)
{
    char *c;
    *pppStr = create_namelist();
    assert(*pppStr);

    for (;;)
    {
        if (SkipString(&c))
            return 1;

        if (*c)
            *pppStr = add_name(c, *pppStr);
        else
            break;
    }

    return 0;
}

int CByteBuffer::ReadIntList(int **ilist)
{
    sbit32 len;
    int c, i;
    int *intlist;
    intlist = *ilist;

    if (Read32(&len))
        return 1;

    CREATE(intlist, int, len + 1)
    intlist[0] = len;
    for (i = 1; i <= len; i++)
    {
        if (Read32(&c) == 1)
            assert(FALSE);

        intlist[i] = c;
    }
    *ilist = intlist;
    return 0;
}

int CByteBuffer::ReadBlock(ubit8 **ppData, ubit32 *pnLen)
{
    *ppData = NULL;

    if (Read32(pnLen))
        return 1;

    *ppData = (ubit8 *)malloc(*pnLen);

    if (ppData == NULL)
        return 1;

    return Read(*ppData, *pnLen);
}

int CByteBuffer::Skip(int nLen)
{
    if (m_nReadPos + nLen > m_nLength)
        return 1;

    m_nReadPos += nLen;

    return 0;
}

int CByteBuffer::Skip8(void)
{
    return Skip(sizeof(ubit8));
}

int CByteBuffer::Skip16(void)
{
    return Skip(sizeof(ubit16));
}

int CByteBuffer::Skip32(void)
{
    return Skip(sizeof(ubit32));
}

int CByteBuffer::SkipFloat(void)
{
    return Skip(sizeof(float));
}

int CByteBuffer::SkipString(char **ppStr)
{
    if (ppStr)
        *ppStr = (char *)m_pData + m_nReadPos;

    return Skip(1 + strlen((char *)m_pData + m_nReadPos));
}

int CByteBuffer::SkipNames(void)
{
    char *c;
    unsigned int len, i;

    if (Read32(&len) == 1)
        assert(FALSE);

    for (i = 0; i < len; i++)
        SkipString(&c);

    return 0;
}

int CByteBuffer::SkipVals(void)
{
    unsigned int len, i;

    if (Read32(&len) == 1)
        assert(FALSE);

    for (i = 0; i < len; i++)
    {
        if (Skip32())
            return 1;
    }

    return 0;
}

void CByteBuffer::Append(const ubit8 *pData, ubit32 nLen)
{
    if (nLen + m_nLength > m_nAllocated)
        IncreaseSize(nLen);

    memcpy(m_pData + m_nLength, pData, nLen);

    m_nLength += nLen;
}

void CByteBuffer::Append(CByteBuffer *pBuf)
{
    Append(pBuf->GetData(), pBuf->GetLength());
}

void CByteBuffer::Append8(ubit8 i)
{
    Append(&i, sizeof(i));
}

void CByteBuffer::Append16(ubit16 i)
{
    Append((ubit8 *)&i, sizeof(i));
}

void CByteBuffer::Append32(ubit32 i)
{
    Append((ubit8 *)&i, sizeof(i));
}

void CByteBuffer::AppendFloat(float f)
{
    Append((ubit8 *)&f, sizeof(f));
}

void CByteBuffer::AppendBlock(const ubit8 *pData, ubit32 nLen)
{
    Append32(nLen);
    Append(pData, nLen);
}

void CByteBuffer::AppendString(const char *pStr)
{
    if (pStr)
        Append((ubit8 *)pStr, strlen(pStr) + 1);
    else
        AppendString("");
}

void CByteBuffer::AppendDoubleString(const char *pStr)
{
    if (pStr)
    {
        AppendString(pStr);
        TAIL(pStr);
        AppendString(pStr + 1);
    }
    else
    {
        AppendString("");
        AppendString("");
    }
}

void CByteBuffer::AppendNames(const char **ppNames)
{
    if (ppNames)
        for (; (const char *)*ppNames && **ppNames; ppNames++)
            AppendString(*ppNames);

    AppendString("");
}

/* =================================================================== */
/* =================================================================== */

ubit8 bread_ubit8(ubit8 **b)
{
    ubit8 i;

    memcpy((ubit8 *)&i, *b, sizeof(ubit8));
    *b += sizeof(ubit8);

    return i;
}

ubit16 bread_ubit16(ubit8 **b)
{
    ubit16 i;

    memcpy((ubit8 *)&i, *b, sizeof(ubit16));
    *b += sizeof(ubit16);

    return i;
}

ubit32 bread_ubit32(ubit8 **b)
{
    ubit32 i;

    memcpy((ubit8 *)&i, *b, sizeof(ubit32));
    *b += sizeof(ubit32);

    return i;
}

float bread_float(ubit8 **b)
{
    float f;

    memcpy((ubit8 *)&f, *b, sizeof(float));
    *b += sizeof(float);

    return f;
}

ubit8 *
bread_data(ubit8 **b, ubit32 *plen)
{
    ubit32 len;
    ubit8 *data;

    data = NULL;
    len = bread_ubit32(b);
    if (plen)
        *plen = len;

    if (len > 0)
    {
        CREATE(data, ubit8, len);
        memcpy(data, *b, len);
        *b += len;
    }

    return data;
}

/* Stored: as Null terminated string            */
/* Copy string from **b into *str               */
void bread_strcpy(ubit8 **b, char *str)
{
    for (; (*str++ = **b); (*b)++)
        ;
    (*b)++;
}

/*  Stored: as Null terminated string
 *  Will allocate space for string, if the read
 *  string is one or more characters, and return
 *  pointer to allocated string (or 0)
 */
char *
bread_str_alloc(ubit8 **b)
{
    if (**b)
    {
        char *c, *t;
        t = (char *)*b;

        c = str_dup(t);

        *b += strlen(c) + 1;
        return c;
    }

    (*b)++;
    return NULL;
}

/* Returns pointer to the string and skips past the end to next
   point in buffer */
char *
bread_str_skip(ubit8 **b)
{
    char *o = (char *)*b;

    TAIL(*b);
    (*b)++;

    return o;
}

/* Stored: As 'N' strings followed by the empty */
/* string ("")                                  */
/* Returns * to nameblock, nameblock may be     */
/* but is never null ({""}).                    */

char **
bread_nameblock(ubit8 **b)
{
    char buf[MAX_STRING_LENGTH];
    char **nb;

    nb = create_namelist();

    for (;;)
    {
        bread_strcpy(b, buf);
        if (*buf)
            nb = add_name(buf, nb);
        else
            break;
    }

    return nb;
}

void bwrite_ubit8(ubit8 **b, ubit8 i)
{
    **b = i;
    *b += sizeof(ubit8);
}

void bwrite_ubit16(ubit8 **b, ubit16 i)
{
    memcpy(*b, (ubit8 *)&i, sizeof(ubit16));
    *b += sizeof(ubit16);
}

void bwrite_ubit32(ubit8 **b, ubit32 i)
{
    memcpy(*b, (ubit8 *)&i, sizeof(ubit32));
    *b += sizeof(ubit32);
}

void bwrite_sbit32(ubit8 **b, sbit32 i)
{
    int x;
    x = i;
    memcpy(*b, (ubit8 *)&x, sizeof(sbit32));
    fprintf(stderr, "Sbit32 out %d\n", x);
    *b += sizeof(sbit32);
}

void bwrite_float(ubit8 **b, float f)
{
    memcpy(*b, (ubit8 *)&f, sizeof(float));
    *b += sizeof(float);
}

void bwrite_data(ubit8 **b, ubit8 *data, ubit32 len)
{
    bwrite_ubit32(b, len);
    if (len > 0)
    {
        memcpy(*b, data, len);
        *b += len;
    }
}

/* String is stored as Null terminated string   */
/* Space is NOT allocated if string is 0 length */
/* but NIL is returned                          */
void bwrite_string(ubit8 **b, const char *str)
{
    if (str)
    {
        for (; *str; str++, (*b)++)
            **b = *str;

        **b = '\0';
        *b += 1;
    }
    else
    {
        **b = '\0';
        *b += 1;
    }
}

/* Write a string of the format:  ssss\0ssss\0 */
void bwrite_double_string(ubit8 **b, char *str)
{
    int i;

    if (str)
    {
        for (i = 0; i < 2; str++, (*b)++, (*str ? 0 : i++))
            **b = *str;

        **b = '\0';
        *b += 1;
    }
    else
    {
        bwrite_string(b, "");
        bwrite_string(b, "");
    }
}

/* Stored: As 'N' strings followed by the empty string ("") */
void bwrite_nameblock(ubit8 **b, char **nb)
{
    if (nb)
        for (; *nb && **nb; nb++)
            bwrite_string(b, *nb);

    bwrite_string(b, "");
}

/* number of ints, followed by ints */
void bwrite_intblock(ubit8 **b, int *ib)
{
    int i;
    i = ib[0];
    if (i > 0)
    {
        memcpy(*b, ib, i * sizeof(sbit32));
        *b += i * sizeof(sbit32);
    }
    else
    {
        bwrite_ubit32(b, 0);
    }
    /*	  bwrite_sbit32(b,ib[0]);
    	fprintf(stderr, "intlist Len %d\n", ib[0]);
         for (i=1; i<=ib[0]; i++) {
    	fprintf(stderr, "intlist %d=%d\n", i, ib[i]);
           bwrite_sbit32(b,ib[i]);
    	  }*/
}
