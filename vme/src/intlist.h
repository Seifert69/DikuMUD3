/*
 $Author: tperry $
 $RCSfile: intlist.h,v $
 $Date: 2002/05/22 01:35:59 $
 $Revision: 2.2 $
 */

#ifndef _MUD_INTLIST_H
#define _MUD_INTLIST_H

#include "essential.h"
#include "bytestring.h"

using namespace std;
#include <string>

class cintlist
{
public:
    cintlist (void);
    ~cintlist (void);
    void operator = (cintlist * cint);
    void CopyList (cintlist * cint);
    void Free (void);
    cintlist *Duplicate (void);
    void Replace (int idx, int val);
    char *catnames ();
    std::string json(void);
    int Value (int idx);
    sbit32 *ValuePtr (int idx);
    void Remove (int idx);
    void AppendBuffer (CByteBuffer * pBuf);
    int ReadBuffer (CByteBuffer * pBuf);
    void Append (int val);
    void Prepend (int val);
    void Insert (int val, int loc);
    void bread (ubit8 ** b);
    inline sbit32 Length (void)
    {
        return length;
    }

private:
    int *intlist;
    sbit32 length;
};

#endif
