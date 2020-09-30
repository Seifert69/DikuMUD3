/*
 $Author: All $
 $RCSfile: namelist.h,v $
 $Date: 2004/09/18 19:52:56 $
 $Revision: 2.6 $
 */

#ifndef _MUD_NAMELIST_H
#define _MUD_NAMELIST_H

#include "bytestring.h"
using namespace std;
#include <string>

class cNamelist
{
public:
    cNamelist (void);
    cNamelist (const char **list);
    ~cNamelist (void);
    void operator = (cNamelist * cn);
    void operator = (const char **);
    void Free (void);
    void CopyList (const char **list);
    void CopyList (class cNamelist *);

    void AppendBuffer (CByteBuffer * pBuf);
    int ReadBuffer (CByteBuffer * pBuf, int unit_version);
    void bread (ubit8 ** b);
    void bwrite (ubit8 ** b);

    char *catnames();
    std::string json(void);
    
    void Remove (ubit32 idx);
    void RemoveName (const char *name);

    void Substitute (ubit32 idx, const char *newname);
    const char *Name (ubit32 idx = 0);
    string *InstanceName (ubit32 idx = 0);

    void AppendName (const char *name);
    void dAppendName (const char *name);
    void PrependName (const char *name);
    void InsertName (const char *name, ubit32 loc);

    cNamelist *Duplicate (void);

    const int IsNameIdx (const char *name);
    const int IsNameRawIdx (const char *name);
    const char *IsNameRaw (const char *name);
    const char *IsName (const char *name);

    const char *StrStrRaw (const char *name);
    const char *StrStr (const char *name);

    inline ubit32 Length (void)
    {
        return length;
    }

private:
    string ** namelist;
    ubit32 length;
};

#endif
