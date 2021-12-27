/*
 $Author: tperry $
 $RCSfile: queue.h,v $
 $Date: 2001/04/29 03:46:07 $
 $Revision: 2.1 $
 */

#ifndef _MUD_QUEUE_HCC
#define _MUD_QUEUE_HCC

#include "essential.h"

// To use, inherit this type into your data structure.
class cQueueElem
{
    friend class cQueue;

public:
    cQueueElem(char *c, int bCopy = TRUE);
    cQueueElem(ubit8 *d, ubit32 n, int bCopy = TRUE);
    ~cQueueElem(void)
    {
        if(pData)
            FREE(pData);
    }

    ubit32 Bytes(void) { return nSize; }
    ubit8 *Data(void) { return pData; }
    void SetNull(void)
    {
        pData = NULL;
        nSize = 0;
    }

    cQueueElem *PeekNext(void) { return pNext; }

private:
    ubit8 *pData;
    ubit32 nSize; // Optional number of bytes
    cQueueElem *pNext;
};

class cQueue
{
public:
    cQueue();
    ~cQueue();

    int IsEmpty(void);
    ubit32 Size(void);
    ubit32 Bytes(void);

    void Copy(ubit8 *data, ubit32 nLen);
    void CutCopy(ubit8 *data, ubit32 nLen);
    void Cut(ubit32 nLen);

    void Append(class cQueueElem *pe);
    void Prepend(class cQueueElem *pe);

    cQueueElem *GetHead(void);

    const cQueueElem *PeekHead(void);
    const cQueueElem *PeekTail(void);

    void Flush(void);

private:
    cQueueElem *pHead;
    cQueueElem *pTail;
    cQueueElem *pPreTail;

    ubit32 nEntries;
    ubit32 nBytes;
};

#endif
