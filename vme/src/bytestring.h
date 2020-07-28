/*
 $Author: tperry $
 $RCSfile: bytestring.h,v $
 $Date: 2001/04/29 03:46:05 $
 $Revision: 2.1 $
 */

#ifndef _MUD_BYTESTRING_H
#define _MUD_BYTESTRING_H

#include "essential.h"

class CByteBuffer
{
public:
    CByteBuffer (ubit32 nSize = 1024);
    virtual ~ CByteBuffer (void);

    // Informative functions

    inline ubit32 GetLength (void)
    {
        return m_nLength;
    }
    inline ubit32 GetAllocated (void)
    {
        return m_nAllocated;
    }
    inline ubit32 GetReadPosition (void)
    {
        return m_nReadPos;
    }
    inline const ubit8 *GetData (void)
    {
        return m_pData;
    }

    void SetReadPosition (ubit32 nReadPosition);
    void SetLength (ubit32 nLen);
    void SetData (ubit8 * pData, ubit32 nLength);

    inline void Rewind (void)
    {
        m_nReadPos = 0;
    }
    inline void Clear (void)
    {
        m_nReadPos = 0;
        m_nLength = 0;
        assert(m_nAllocated > 0);
        *m_pData = 0;
    }

    // Public
    //
    int FileRead (FILE * f, ubit32 nLength);
    int FileRead (FILE * f, long offset, ubit32 length);
    int FileWrite (FILE * f);

    // Public functions to read from a buffer
    //
    int Read (ubit8 * pBuf, ubit32 nLen);

    int ReadBlock (ubit8 ** pData, ubit32 * nLen);

    int Read8 (ubit8 * pNum);
    int Read16 (ubit16 * pNum);
    int Read32 (ubit32 * pNum);
    int Read8 (sbit8 * pNum);
    int Read16 (sbit16 * pNum);
    int Read32 (sbit32 * pNum);
    int ReadFloat (float *pFloat);
    int ReadStringAlloc (char **pStr);
    int ReadStringCopy (char *pStr, ubit32 nSize);
    int ReadNames (char ***pppNames);
    int ReadIntList (int **ilist);

    int Skip (int n);
    int Skip8 (void);
    int Skip16 (void);
    int Skip32 (void);
    int SkipFloat (void);
    int SkipString (char **ppStr = NULL);
    int SkipNames (void);
    int SkipVals (void);

    // Public functions to write to a buffer
    //
    void Append (const ubit8 * pData, ubit32 nLen);

    void Append (CByteBuffer * pBuf);

    void AppendBlock (const ubit8 * pData, ubit32 nLen);

    void Append8 (ubit8 i);
    void Append16 (ubit16 i);
    void Append32 (ubit32 i);
    void AppendFloat (float f);
    void AppendString (const char *pStr);
    void AppendDoubleString (const char *pStr);
    void AppendNames (const char *pNames[]);

protected:
    void SetSize (ubit32 nSize);
    void IncreaseSize (ubit32 nSize);

private:
    ubit32 m_nReadPos;
    ubit32 m_nLength;
    ubit32 m_nAllocated;
    ubit8 *m_pData;
};

ubit8 bread_ubit8 (ubit8 ** buf);
ubit16 bread_ubit16 (ubit8 ** buf);
ubit32 bread_ubit32 (ubit8 ** buf);
float bread_float (ubit8 ** buf);
ubit8 *bread_data (ubit8 ** b, ubit32 * len);
void bwrite_data (ubit8 ** b, ubit8 * data, ubit32 len);
void bread_strcpy (ubit8 ** b, char *str);
char *bread_str_alloc (ubit8 ** buf);
char *bread_str_skip (ubit8 ** b);
char **bread_nameblock (ubit8 ** b);

void bwrite_ubit8 (ubit8 ** b, ubit8 i);
void bwrite_ubit16 (ubit8 ** b, ubit16 i);
void bwrite_ubit32 (ubit8 ** b, ubit32 i);
void bwrite_float (ubit8 ** b, float f);
void bwrite_string (ubit8 ** b, const char *str);
void bwrite_double_string (ubit8 ** b, char *str);
void bwrite_nameblock (ubit8 ** b, char **nb);
void bwrite_intblock (ubit8 ** b, int *nb);

#endif /* _MUD_BYTESTRING_H */
