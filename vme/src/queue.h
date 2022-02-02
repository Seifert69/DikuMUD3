/*
 $Author: tperry $
 $RCSfile: queue.h,v $
 $Date: 2001/04/29 03:46:07 $
 $Revision: 2.1 $
 */
#pragma once

#include "essential.h"

// To use, inherit this type into your data structure.
class cQueueElem
{
    friend class cQueue;

public:
    cQueueElem(char *c, int bCopy = TRUE);
    cQueueElem(ubit8 *d, ubit32 n, int bCopy = TRUE);
    ~cQueueElem()
    {
        if (pData)
            FREE(pData);
    }

    ubit32 Bytes() { return nSize; }
    ubit8 *Data() { return pData; }
    void SetNull()
    {
        pData = nullptr;
        nSize = 0;
    }

    cQueueElem *PeekNext() { return pNext; }

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

    int IsEmpty();
    ubit32 Size();
    ubit32 Bytes();

    void Copy(ubit8 *data, ubit32 nLen);
    void CutCopy(ubit8 *data, ubit32 nLen);
    void Cut(ubit32 nLen);

    void Append(cQueueElem *pe);
    void Prepend(cQueueElem *pe);

    cQueueElem *GetHead();

    const cQueueElem *PeekHead();
    const cQueueElem *PeekTail();

    void Flush();

private:
    cQueueElem *pHead;
    cQueueElem *pTail;
    cQueueElem *pPreTail;

    ubit32 nEntries;
    ubit32 nBytes;
};
