/*
 $Author: tperry $
 $RCSfile: queue.cpp,v $
 $Date: 2001/04/29 03:46:07 $
 $Revision: 2.1 $
 */

/*
   I will make a GetTail operation if / when it is needed. In such case,
   the linked list should be a doubly linked list for speed.
*/

#include "queue.h"

#include "utility.h"

#include <cassert>
#include <cstring>

cQueueElem::cQueueElem(char *pStr, int bCopy)
{
    assert(pStr);

    ubit32 n = strlen(pStr) + 1;

    if (bCopy)
    {
        CREATE(pData, ubit8, n);
        memcpy(pData, pStr, n);
    }
    else
    {
        pData = (ubit8 *)pStr;
    }

    nSize = n;
}

cQueueElem::cQueueElem(ubit8 *d, ubit32 n, int bCopy)
{
    if (bCopy)
    {
        if (n > 0)
        {
            CREATE(pData, ubit8, n);
            memcpy(pData, d, n);
        }
        else
        {
            pData = nullptr;
        }
    }
    else
    {
        pData = d;
    }

    nSize = n;
}

cQueue::cQueue()
{
    pHead = nullptr;
    pTail = nullptr;
    nEntries = 0;
    nBytes = 0;
}

cQueue::~cQueue()
{
    Flush();
}

ubit32 cQueue::Size()
{
    return nEntries;
}

ubit32 cQueue::Bytes()
{
    return nBytes;
}

int cQueue::IsEmpty()
{
    return nEntries == 0;
}

void cQueue::Copy(ubit8 *data, ubit32 nLen)
{
    assert(nLen <= nBytes);

    int now = 0;
    cQueueElem *qe = (cQueueElem *)PeekHead();

    do
    {
        assert(qe);
        now = MIN(nLen, qe->Bytes());
        memcpy(data, qe->Data(), now);
        nLen -= now;
        data += now;
        qe = qe->PeekNext();
    } while (nLen > 0);
}

void cQueue::Cut(ubit32 nLen)
{
    if (nLen < 1)
    {
        return;
    }

    assert(nLen <= nBytes);

    cQueueElem *qe = nullptr;

    do
    {
        qe = GetHead();

        assert(qe);

        if (nLen < qe->Bytes())
        {
            Prepend(new cQueueElem(qe->Data() + nLen, qe->Bytes() - nLen));
        }

        nLen -= MIN(qe->Bytes(), nLen);
        delete qe;
    } while (nLen > 0);
}

void cQueue::CutCopy(ubit8 *data, ubit32 nLen)
{
    Copy(data, nLen);
    Cut(nLen);
}

void cQueue::Append(cQueueElem *pe)
{
    if (IsEmpty())
    {
        pHead = pTail = pe;
    }
    else
    {
        pTail->pNext = pe;
        pTail = pe;
    }

    pe->pNext = nullptr;

    nEntries++;
    nBytes += pe->Bytes();
}

/*  Quinn's Immortal Remark...
 *
 *  Listen...
 *  The sound of a thousand comp.sci. professors rotating in their graves.
 */
void cQueue::Prepend(cQueueElem *pe)
{
    if (IsEmpty())
    {
        pe->pNext = nullptr;
        pTail = pe;
    }
    else
    {
        pe->pNext = pHead;
    }

    pHead = pe;

    nEntries++;
    nBytes += pe->Bytes();
}

const cQueueElem *cQueue::PeekHead()
{
    return pHead;
}

const cQueueElem *cQueue::PeekTail()
{
    return pTail;
}

cQueueElem *cQueue::GetHead()
{
    cQueueElem *pTmp = nullptr;

    if (IsEmpty())
    {
        return nullptr;
    }

    pTmp = pHead;

    if (nEntries == 1)
    {
        pHead = pTail = nullptr;
    }
    else
    {
        pHead = pHead->pNext;
    }

    nEntries--;
    nBytes -= pTmp->Bytes();

    return pTmp;
}

void cQueue::Flush()
{
    cQueueElem *pTmp = nullptr;

    while ((pTmp = GetHead()))
    {
        delete pTmp;
    }
}
