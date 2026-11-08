/*
 $Author: All $
 $RCSfile: select.cpp,v $
 $Date: 2003/12/11 04:47:11 $
 $Revision: 2.2 $
 */

#ifdef _WINDOWS
    #include <time.h>
    #include <winsock2.h>
#endif

#include "hook.h"
#include "slog.h"

#include <sys/time.h>
#include <unistd.h>

#include <cassert>
#include <cerrno>
#include <csignal>
#include <cstring>
#include <exception>

cCaptainHook g_CaptainHook;

//
// For the MUD server:
//     Listen on the mother port (4999) for connecting Multiplexers:
//        g_CaptainHook.Hook(fdMother, &g_MotherHook);
//     thus Mother listening is in the cCaptainHook:cHook (I think)
//
//     Each connecting Mplex'er, and Mother Connection is stored here:
//        g_CaptainHook.Hook (t, &g_Multi.g_Multi[i]);
//      which is a cMultiMaster::cMulti::cHook
//
// Also used in the Mplex'ers.
//

/* ------------------------------------------------------------------- */
/*                                HOOK NATIVE                          */
/* ------------------------------------------------------------------- */
//
// The basic hook class handles read and writing. Uses a queue for handling
// incoming and outgoing messages. Holds the filedescriptor and knows if it
// is connected or not.
//

cHookNative::cHookNative()
{
    fd = -1;
}

cHookNative::~cHookNative()
{
    Unhook();
}

int cHookNative::get_fd()
{
    return fd;
}

void cHookNative::Hook(int f)
{
    fd = f;
}

int cHookNative::IsHooked()
{
    return fd != -1;
}

void cHookNative::Unhook()
{
    if (!IsHooked())
    {
        return;
    }

    int i = 0;
#ifdef _WINDOWS
    i = closesocket(fd);
#else
    i = close(fd);
#endif

    fd = -1;

    if (i == -1)
    {
        slog(LOG_ALL, 0, "close(%d): close() socket, error %d", fd, errno);
    }
}

/* ------------------------------------------------------------------- */
/*                     NETWORK READ & WRITE                            */
/* ------------------------------------------------------------------- */

//  > 0 number of bytes written
// == 0 try again later (EWOULDBLOCK)
// -1 : error (it's been unhooked)
//
int cHookNative::write(const void *buf, int count)
{
    int sofar = 0;
    int thisround = 0;

    if (!IsHooked())
    {
        return -1;
    }

    sofar = 0;

    for (;;)
    {
#ifdef _WINDOWS

        thisround = send(fd, (char *)buf + sofar, count - sofar, 0);

        if (thisround == 0)
        {
            /* This should never happen! */
            slog(LOG_ALL, 0, "SYSERR: Huh??  write() returned 0???  Please report this!");
            Unhook();
            return -1;
        }

        if (thisround < 0)
        {
            /* Transient error? */
            if (WSAGetLastError() == WSAEWOULDBLOCK || WSAGetLastError() == WSAEINTR)
            {
                return sofar;
            }
            else
            {
                /* Must be a fatal error. */
                slog(LOG_ALL, 0, "PushWrite (%d): Write to socket, error %d", fd, WSAGetLastError());
                Unhook();
                return -1;
            }
        }
#else
        try
        {
            thisround = ::write(fd, (char *)buf + sofar, count - sofar);
        }
        catch (const std::exception &ex)
        {
            slog(LOG_ALL, 0, "cHookNative::write() exception: [%s]", ex.what());
            Unhook();
            return -1;
        }

        if (thisround == 0)
        {
            slog(LOG_ALL, 0, "cHookNative (%d): Write to socket EOF", (int)fd);
            Unhook();
            return -1;
        }
        else if (thisround < 0)
        {
            if (errno == EWOULDBLOCK)
            {
                return sofar;
            }

            slog(LOG_ALL, 0, "cHookNative (%d): Write to socket, error %d", fd, errno);
            Unhook();
            return -1;
        }
#endif
        sofar += thisround;

        if (sofar >= count)
        {
            break;
        }
    }

    return sofar;
}

//  > 0 number of bytes read to buffer
// == 0 try again later (EWOULDBLOCK)
// -1 : error (and it's been unhooked or was unhooked, fd closed)
//
int cHookNative::read(void *buf, int count)
{
    int thisround = 0;

    if (!IsHooked())
    {
        return -1;
    }

    for (;;)
    {
#if defined(_WINDOWS)
        thisround = recv(fd, buf, count - 1, 0);
#else
        thisround = ::read(fd, buf, count);
#endif

        if (thisround > 0)
        {
            return thisround;
        }
        else if (thisround == 0)
        {
            slog(LOG_ALL, 0, "Read to queue: EOF on socket read.");
            Unhook();
            return -1;
        }
        else /* (thisround < 0) */
        {
#ifdef _WINDOWS
            if (WSAGetLastError() == WSAEWOULDBLOCK || WSAGetLastError() == WSAEINTR)
            {
                return 0;
            }
#else
            if (errno == EWOULDBLOCK)
            {
                return 0;
            }
#endif
            slog(LOG_ALL, 0, "Read from socket %d error %d", fd, errno);
            Unhook();
            return -1;
        }
    }

    return -1;
}

/* ------------------------------------------------------------------- */
/*                                HOOK                                 */
/* ------------------------------------------------------------------- */
//
// The basic hook class handles read and writing. Uses a queue for handling
// incoming and outgoing messages. Holds the filedescriptor and knows if it
// is connected or not.
//

cHook::cHook()
{
}

cHook::~cHook()
{
    Unhook();
}

void cHook::Unhook()
{
    cHookNative::Unhook();
}

int cHook::tfd()
{
    return get_fd();
}

/*int cHook::IsHooked(void)
{
    return fd != -1;
}

void cHook::Unhook(void)
{
    if (IsHooked())
        g_CaptainHook.Unhook(this);

    fd = -1;
}*/

/* ------------------------------------------------------------------- */
/*                     NETWORK READ & WRITE                            */
/* ------------------------------------------------------------------- */

void cHook::PushWrite()
{
    int sofar = 0;
    int len = 0;
    int thisround = 0;
    ubit8 buf[1460];

    if (!cHook::IsHooked())
    {
        return;
    }

    while (!qTX.IsEmpty())
    {
        len = MIN(sizeof(buf)-1, qTX.Bytes());

        qTX.CutCopy(buf, len);

        sofar = 0;

        for (;;)
        {
            thisround = this->write(buf + sofar, len - sofar);

            if (thisround == 0) // Try again
            {
                qTX.Prepend(new cQueueElem(buf + sofar, len - sofar));
                return;
            }
            else if (thisround < 0) // Error
            {
                // will now be closed and error logged
                if (IsHooked())
                {
                    slog(LOG_ALL, 0, "PushWrite: Still hooked even though error - not possible");
                }
                return;
            }

            /* if (thisround < len - sofar)
            {
                sofar += thisround;
                qTX.Prepend(new cQueueElem(buf + sofar, len - sofar));
                return;
            }*/

            sofar += thisround;

            if (sofar >= len)
            {
                break;
            }
        }
    }
}

void cHook::Write(ubit8 *pData, ubit32 nLen, int bCopy)
{
    if (nLen <= 0)
    {
        return;
    }

    if (!IsHooked())
    {
        if (!bCopy)
            FREE(pData);
        return;
    }

    cQueueElem *q = new cQueueElem(pData, nLen, bCopy);
    qTX.Append(q);

    PushWrite();
}

// For this hook, read as much as we can from the network
// into the Hook's Rx queue...
// -1 on error, 0 on ok.
//
int cHook::ReadToQueue()
{
    char buf[4 * 1460 - 6];
    int thisround = 0;

    for (;;)
    {
        thisround = this->read(buf, sizeof(buf)-1);

        if (thisround > 0)
        {
            qRX.Append(new cQueueElem((ubit8 *)buf, (ubit32)thisround));
        }
        else if (thisround == 0)
        {
            // try again
            return 0;
        }
        else // (thisround < 0)
        {
            // will now be closed and error logged
            if (IsHooked())
            {
                slog(LOG_ALL, 0, "ReadToQueue: Still hooked even though error - not possible");
            }
            return -1;
        }
    }

    return 0;
}

/* ------------------------------------------------------------------- */
/*                            CAPTAIN HOOK                             */
/* ------------------------------------------------------------------- */

//
// CaptainHook handles the list of hooks (the clients) and has an
// array of pointers to each Hook class.
// The CaptainHook Wait() checks for data (reads and writes) on all
// Hooks in the list.
//

cCaptainHook::cCaptainHook()
{
#ifndef _WINDOWS
    signal(SIGPIPE, SIG_IGN); // Or else pipe fucks the whole thing up...
#endif
    for (int i = 0; i < 256; i++)
    {
        pfHook[i] = nullptr;
    }

    nTop = 0;
    nMax = 0;
}

cCaptainHook::~cCaptainHook()
{
    Close();
}

void cCaptainHook::Close()
{
    for (int i = 0; i < 256; i++)
    {
        if (pfHook[i])
        {
            pfHook[i]->Unhook();
        }
    }

    nTop = 0;
    nMax = 0;
}

// Maybe Hook(*hook, int fd, *webs)
void cCaptainHook::Hook(int nHandle, cHook *hook)
{
    static int newid = 0;

    assert(nHandle < (int)sizeof(pfHook));
    assert(pfHook[nHandle] == nullptr);

    if (hook->fd != -1)
    {
        slog(LOG_ALL, 0, "ODD Hook() called with a non -1 fd (fd == %d).", (int)hook->fd);
    }

    // assert(hook->fd == -1);

    pfHook[nHandle] = hook;
    hook->Hook(nHandle);
    // hook->fd = nHandle;
    hook->id = newid++;

    hook->qTX.Flush();
    hook->qRX.Flush();

    nIdx[nTop] = nHandle;

    nTop++;

    if (nHandle > nMax)
    {
        nMax = nHandle;
    }

    // DEBUG("HOOKED: Fd %d, Flags %d, Max %d\n", nHandle, nFlag, nMax);
}

void cCaptainHook::Unhook(cHook *hook)
{
    if (!hook->IsHooked())
    {
        return;
    }

    int nHandle = hook->fd;
    int i = 0;

    assert(pfHook[nHandle] == hook);

    for (i = 0; i < nTop; i++)
    {
        if (nIdx[i] == nHandle)
        {
            nIdx[i] = nIdx[nTop - 1];
            nIdx[nTop - 1] = 0;
            nTop--;
            break;
        }
    }

    // pfHook[nHandle]->fd = -1; // This should get unset in Unhook on close()
    pfHook[nHandle]->id = -1;
    pfHook[nHandle]->qTX.Flush();
    pfHook[nHandle]->qRX.Flush();
    pfHook[nHandle] = nullptr;

    nMax = 0;
    for (i = 0; i < nTop; i++)
    {
        if (nIdx[i] > nMax)
        {
            nMax = nIdx[i];
        }
    }
}

// If an unhook is performed during the wait, the possible select may be
// postponed until the next call to wait().
//
int cCaptainHook::Wait(timeval *timeout)
{
    int n = 0;

    /* The following two are used in Wait() because the Input & Write
       can cause any descriptor to become unhooked. It is then the job
       of the Unhook to update this table of descriptors accordingly */
    int nTable[256];
    int nId[256];

    int nTableTop = 0;

    memcpy(nTable, nIdx, sizeof(int) * nTop);
    nTableTop = nTop;

    FD_ZERO(&read_set);
    FD_ZERO(&write_set);

    for (int i = 0; i < nTableTop; i++)
    {
        nId[i] = pfHook[nTable[i]]->id;

        assert(nId[i] != -1);

        FD_SET(nTable[i], &read_set);

        if (!pfHook[nTable[i]]->qTX.IsEmpty())
        {
            FD_SET(nTable[i], &write_set);
        }
    }

    n = select(nMax + 1, &read_set, &write_set, nullptr, timeout);

    if (n == -1)
    {
        // Do not set to zero, it means that a timeout occurred.
        //
        if (errno == EAGAIN)
        {
            n = 1;
        }
        else if (errno == EINTR)
        {
            // slog(LOG_ALL, 0, "CaptainHook: Select Interrupted.\n");
            n = 1;
        }
        else
        {
            slog(LOG_ALL, 0, "CaptainHook: Select error %d.\n", errno);
        }
    }
    else if (n > 0)
    {
        int nFlag = 0;
        int tmpfd = 0;
        int i = 0;

        /* We need to do this the hard way, because nTable[] can be
           changed radically by any sequence of read or write */

        for (i = 0; i < nTableTop; i++)
        {
            nFlag = 0;
            tmpfd = nTable[i];

            if (FD_ISSET(tmpfd, &read_set))
            {
                SET_BIT(nFlag, SELECT_READ);
            }

            if (FD_ISSET(tmpfd, &write_set))
            {
                SET_BIT(nFlag, SELECT_WRITE);
            }

            cHook *pfTmpHook = pfHook[tmpfd];

            /* It could have been unhooked by any previous sequence of
               Input() or Write() sequences */

            if (nFlag && pfTmpHook)
            {
                if ((pfTmpHook == pfHook[tmpfd]) && (pfTmpHook->id == nId[i]))
                {
                    if (pfTmpHook->tfd() == -1)
                    {
                        slog(LOG_ALL, 0, "Wait()) SELECT_READ | SELECT_EXCEPT FD is -1");
                    }

                    if (nFlag & (SELECT_READ | SELECT_EXCEPT))
                    {
                        pfTmpHook->Input(nFlag);
                    }
                }

                if ((pfTmpHook == pfHook[tmpfd]) && (pfTmpHook->id == nId[i]))
                {
                    if (pfTmpHook->tfd() == -1)
                    {
                        slog(LOG_ALL, 0, "Wait()) SELECT_WRITE FD is -1");
                    }

                    if (nFlag & SELECT_WRITE)
                    {
                        pfTmpHook->PushWrite();
                    }
                }
            }
        }
    }

    return n;
}
