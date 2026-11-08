/*
 $Author: All $
 $RCSfile: mplex.cpp,v $
 $Date: 2003/12/31 01:21:59 $
 $Revision: 2.4 $
 */

#ifdef _WINDOWS
    #include "string.h"
    #include "telnet.h"
    #include "winbase.h"
    #include "winsock2.h"

    #include <time.h>
#endif

#include "ClientConnector.h"
#include "MUDConnector.h"
#include "color.h"
#include "echo_server.h"
#include "essential.h"
#include "formatter.h"
#include "hook.h"
#include "mplex.h"
#include "network.h"
#include "protocol.h"
#include "queue.h"
#include "slog.h"
#include "textutil.h"
#include "translate.h"

#include <arpa/telnet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cassert>
#include <cctype>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>

/*
   In Mplex main MotherHook is opened and put into CaptainHook.
   Mother listens on 4242 for new players connecting
   in the mplex's Control() loop in MUDConnector.cpp
   which calls CaptainHook.Wait();

    When a new player connects to the mother port,
    CaptainHook.Wait() in turn calls MotherHook.Input().
    MotherHook calls new cConHook() which is the player's
    client connection. The constructor adds itself to the
    g_connection_list list and sets itself to the function
    dumbMenuSelect();
    For a player connecting with Telnet the contructor
    adds the player to CaptainHook.Hook
    For a player connecting with WebSockets the constructor
    stops before that.

    If the connection to the MUD goes down, all cConHooks
    have their m_nId set to zero.

*/
namespace mplex
{

cConHook *g_connection_list = nullptr;

void dumbPlayLoop(cConHook *con, const char *cmd)
{
    con->PlayLoop(cmd);
}

void cConHook::PlayLoop(const char *cmd)
{
    if (m_nId == 0)
    {
        if (m_nState == 0)
        {
            auto buf = diku::format_to_str("Your ID to %s was reset due to connection lost.<br/>", g_mudname);
            WriteCon(buf);
            return;
        }

        if (m_nState++ < 50)
        {
            auto buf = diku::format_to_str("Waiting to receive an ID from %s.<br/>", g_mudname);
            WriteCon(buf);
            return;
        }

        WriteCon("Giving up... You should try to disconnect and try again.<br/>");
        return;
    }
    else // m_nId != 0
    {
        m_nState = 0;
        TransmitCommand(cmd);
    }
}

void dumbMenuSelect(cConHook *con, const char *cmd)
{
    con->MenuSelect(cmd);
}

// This is where players are when initially connecting only.
void cConHook::MenuSelect(const char *cmd)
{
    if (!g_MudHook.IsHooked())
    {
        auto buf = diku::format_to_str("%s is unreachable right now...<br/>", g_mudname);
        SendCon(buf);
        test_mud_up(); // Wonder if I have multi threading issues here :o)
        return;
    }

    if (m_nId == 0)
    {
        auto buf = diku::format_to_str("Requesting an ID from %s.<br/>", g_mudname);
        SendCon(buf);
        m_nState = 1; // This means we're waiting for a response
        m_pFptr = dumbPlayLoop;
        protocol_send_request(&g_MudHook);
    }
    else
    {
        auto buf = diku::format_to_str("You got ID from %s...<br/>", g_mudname);
        SendCon(buf);
        m_nState = 0;
        m_pFptr = dumbPlayLoop;
    }
}

void dumbMudDown(cConHook *con, const char *cmd)
{
    con->MudDown(cmd);
}

void cConHook::MudDown(const char *cmd)
{
    auto buf = diku::format_to_str("There is still no connection to %s.<br/>", g_mudname);
    SendCon(buf);

    test_mud_up(); // Wonder if I have multi threading issues here :o)
}

void Idle(cConHook *con, const char *cmd)
{
    /* This should not happen, but I am tryng to find a bug... */

    slog(LOG_ALL, 0, "AHA! Ilde was called! Tell Development team about this!");
}

void dumbPressReturn(cConHook *con, const char *cmd)
{
    con->PressReturn(cmd);
}

void cConHook::PressReturn(const char *cmd)
{
    int oldmode = m_nPromptMode;

    if (*skip_blanks(cmd))
    {
        SendCon("<br/><div class='return'>*** Read aborted ***</div><br/>");

        m_qPaged.Flush();
        m_pFptr = dumbPlayLoop;
        m_nPromptMode = 0;
        PlayLoop(cmd);
        return;
    }

    ShowChunk();

    if (m_nPromptMode == 1)
    {
    }
    else if (m_nPromptMode == 0)
    {
        m_qPaged.Flush();
        m_pFptr = dumbPlayLoop;
        m_nPromptMode = 0;

        if (oldmode != m_nPromptMode)
        {
            char buf[1000];
            strcpy(buf, ParseOutput("<br/><div class='return'>*** Read Done ***</div><br/>"));
            Write((ubit8 *)buf, strlen(buf));
            PlayLoop("");
        }
    }
}

// ===============================

// Destroy all clients that are no longer connected
void ClearUnhooked()
{
    cConHook *con = nullptr;
    cConHook *nextcon = nullptr;

    for (con = g_connection_list; con; con = nextcon)
    {
        nextcon = con->m_pNext;

        if (!con->IsHooked())
        {
            con->Close(TRUE);
            delete con;
        }
    }
}

/* ================== VIRTUAL OVERRIDE ===================== */

int cConHook::IsHooked()
{
    if (this->m_fWebsSend)
    {
        return TRUE;
    }
    else
    {
        return cHook::IsHooked();
    }
}

void cConHook::Unhook()
{
    if (IsHooked())
    {
        slog(LOG_OFF, 0, "Unhooking player connection");
        g_CaptainHook.Unhook(this);
    }

    TlsFree(); // before the socket closes, so close_notify can still go out

    if (!this->m_fWebsSend)
    {
        cHook::Unhook();
    }
}

/* ================= TLS on the telnet port (telnets) ============================ */
//
// mplex -z wraps every accepted telnet socket in TLS, for clients such as
// TinTin++ (#ssl) and Mudlet (the "Secure" box). The sockets are
// non-blocking, so the select loop drives the handshake: InputOnce() steps
// it when the socket is readable, PushWrite() when it is writable, and
// WantWrite() asks for the latter only while OpenSSL has something to send.
// Whatever the mplex queues meanwhile (the telnet negotiation, "Requesting
// an ID", the banner) waits in qTX and is flushed, in that order, the moment
// the handshake completes. The plain path is untouched: m_pSSL stays nullptr
// on a plain or websocket hook and every override falls through to cHook.
//
// One SSL_CTX serves the process. It is rebuilt when the PEM file changes (a
// renewed certificate); connections already up keep the old one alive
// through their SSL objects until they close.

SSL_CTX *g_tls_ctx = nullptr;

namespace
{
std::string g_tls_pem_path;              // what tls_init_telnet() loaded
struct stat g_tls_pem_stat = {};         // and how the file looked then
volatile sig_atomic_t g_tls_exiting = 0; // process on its way out: no OpenSSL calls

// The password callback the wss listener has always had: an encrypted key
// opens only if its passphrase happens to be "test" (mplex.md says -nodes).
int tls_password_cb(char *buf, int size, int /*rwflag*/, void * /*userdata*/)
{
    const char *pw = "test";
    int n = (int)strlen(pw);

    if (n > size)
    {
        n = size;
    }
    memcpy(buf, pw, n);
    return n;
}

// Everything OpenSSL queued about the last failure, oldest first; the queue
// is drained, so nothing leaks into the next SSL_get_error() on this thread.
// When it is empty (the usual case for SSL_ERROR_SYSCALL) errno is described
// instead, so a log line never gets a NULL for its %s.
std::string tls_errors(int err)
{
    std::string s;
    unsigned long e = 0;

    while ((e = ERR_get_error()) != 0)
    {
        const char *reason = ERR_reason_error_string(e); // NULL for errors OpenSSL has no text for, system errors among them

        if (!s.empty())
        {
            s += "; ";
        }

        if (reason)
        {
            s += reason;
        }
        else
        {
            char buf[256];
            ERR_error_string_n(e, buf, sizeof(buf)); // "error:80000002:system library::No such file or directory"
            s += buf;
        }
    }

    if (s.empty())
    {
        if (err == SSL_ERROR_SYSCALL)
        {
            s = errno ? strerror(errno) : "connection closed by peer";
        }
        else
        {
            s = "no detail";
        }
    }

    return s;
}

SSL_CTX *tls_new_ctx(const char *path)
{
    ERR_clear_error();

    SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());

    if (ctx == nullptr)
    {
        slog(LOG_ALL, 0, "TLS: SSL_CTX_new failed: %s", tls_errors(SSL_ERROR_SSL).c_str());
        return nullptr;
    }

    SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);

    // cHook::PushWrite() refills its buffer between short writes and a TLS
    // record may go out in pieces, so OpenSSL must accept both.
    SSL_CTX_set_mode(ctx, SSL_MODE_ENABLE_PARTIAL_WRITE | SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);

    long opts = 0;
#ifdef SSL_OP_IGNORE_UNEXPECTED_EOF
    opts |= SSL_OP_IGNORE_UNEXPECTED_EOF; // most mud clients drop the socket without close_notify: a close, not an error
#endif
#ifdef SSL_OP_NO_RENEGOTIATION
    opts |= SSL_OP_NO_RENEGOTIATION; // so SSL_write() never waits for a read, and WantWrite() stays "is output queued"
#endif
    SSL_CTX_set_options(ctx, opts);

#if OPENSSL_VERSION_NUMBER >= 0x10101000L
    // Nothing resumes a mud session. Without tickets the TLS 1.3 handshake
    // ends with the client's Finished and has nothing left to send.
    SSL_CTX_set_num_tickets(ctx, 0);
#endif

    if (!tls_load_pem(ctx, path))
    {
        SSL_CTX_free(ctx);
        return nullptr;
    }

    return ctx;
}

void tls_at_exit()
{
    g_tls_exiting = 1;
}
} // namespace

bool tls_load_pem(SSL_CTX *ctx, const char *path)
{
    ERR_clear_error();
    SSL_CTX_set_default_passwd_cb(ctx, tls_password_cb);

    if (SSL_CTX_use_certificate_chain_file(ctx, path) != 1 || SSL_CTX_use_PrivateKey_file(ctx, path, SSL_FILETYPE_PEM) != 1 ||
        SSL_CTX_check_private_key(ctx) != 1)
    {
        slog(LOG_ALL,
             0,
             "TLS: cannot load %s (certificate chain and private key in one PEM file): %s",
             path,
             tls_errors(SSL_ERROR_SSL).c_str());
        return false;
    }

    return true;
}

bool tls_init_telnet(const char *path)
{
    static bool bRegistered = false;

    if (!bRegistered)
    {
        // Runs before the static destructors (g_CaptainHook's Close()) and
        // before OpenSSL's own atexit cleanup can pull the rug from under them
        atexit(tls_at_exit);
        bRegistered = true;
    }

    SSL_CTX *ctx = tls_new_ctx(path);

    if (ctx == nullptr)
    {
        return false;
    }

    tls_free_telnet();
    g_tls_ctx = ctx;
    g_tls_pem_path = path;
    stat(path, &g_tls_pem_stat);
    return true;
}

void tls_free_telnet()
{
    if (g_tls_ctx)
    {
        SSL_CTX_free(g_tls_ctx);
        g_tls_ctx = nullptr;
    }
}

void tls_exiting()
{
    g_tls_exiting = 1;
}

SSL_CTX *tls_ctx_for_accept()
{
    if (g_tls_ctx == nullptr)
    {
        return nullptr;
    }

    struct stat st = {};

    if (stat(g_tls_pem_path.c_str(), &st) == 0 &&
        (st.st_mtim.tv_sec != g_tls_pem_stat.st_mtim.tv_sec || st.st_mtim.tv_nsec != g_tls_pem_stat.st_mtim.tv_nsec ||
         st.st_size != g_tls_pem_stat.st_size || st.st_ino != g_tls_pem_stat.st_ino))
    {
        // Renewed. The old context stays alive for the connections that hold
        // it. A file that does not load is logged once, and the previous
        // certificate stays in use until the file changes again.
        g_tls_pem_stat = st;

        SSL_CTX *ctx = tls_new_ctx(g_tls_pem_path.c_str());

        if (ctx)
        {
            SSL_CTX_free(g_tls_ctx);
            g_tls_ctx = ctx;
            slog(LOG_OFF, 0, "TLS: %s changed, certificate reloaded", g_tls_pem_path.c_str());
        }
        else
        {
            slog(LOG_ALL, 0, "TLS: %s changed but does not load, keeping the previous certificate", g_tls_pem_path.c_str());
        }
    }

    return g_tls_ctx;
}

// Whether select() should watch this socket for writing. While the handshake
// runs, only when OpenSSL has part of its flight left to send; after it, as
// for a plain hook, when output is queued. So output queued before the
// handshake is done never makes select() return at once for a socket that
// is writable but cannot be written to yet.
bool cConHook::WantWrite()
{
    if (m_pSSL && !SSL_is_init_finished(m_pSSL))
    {
        return SSL_want_write(m_pSSL) != 0;
    }

    return cHook::WantWrite();
}

// The socket is writable, or something was just queued: step the handshake
// while it runs, flush the queue as usual once it is done.
void cConHook::PushWrite()
{
    if (m_pSSL && !SSL_is_init_finished(m_pSSL))
    {
        TlsHandshake();
        return;
    }

    cHook::PushWrite();
}

// One step of the handshake. True once it is complete and the connection is
// still up; false while it needs more from the peer, and after it has failed
// (TlsFail() has then dropped the connection). On completion the output
// queued so far goes out, the telnet negotiation first.
bool cConHook::TlsHandshake()
{
    if (SSL_is_init_finished(m_pSSL))
    {
        return true;
    }

    ERR_clear_error();

    int r = SSL_do_handshake(m_pSSL);

    if (r == 1)
    {
        slog(LOG_OFF, 0, "TLS: %s handshake done with %s", SSL_get_version(m_pSSL), m_aHost);
        cHook::PushWrite(); // a hard write error ends in TlsFail(), which frees m_pSSL
        return m_pSSL != nullptr && IsHooked();
    }

    int e = SSL_get_error(m_pSSL, r);

    if (e == SSL_ERROR_WANT_READ || e == SSL_ERROR_WANT_WRITE)
    {
        return false;
    }

    TlsFail("handshake", e);
    return false;
}

// The connection is over, from this side: tell the mud if it knows about it,
// and drop the socket either way. Close() alone does nothing before the mud
// has assigned an id, and the handshake runs before that, so a plain client
// on the TLS port would otherwise stay hooked, readable, and in the log on
// every pass of the select loop.
void cConHook::TlsDrop()
{
    Close(TRUE);

    if (IsHooked())
    {
        Unhook();
    }
}

void cConHook::TlsFail(const char *what, int err)
{
    slog(LOG_ALL, 0, "TLS: %s with %s failed (%d): %s", what, m_aHost, err, tls_errors(err).c_str());
    m_bTlsFatal = true; // SSL_shutdown() is not allowed after this
    TlsDrop();
}

// ::read() contract, see cHookNative::readfd(). A close or a fatal error
// drops the connection here (TlsDrop()); InputOnce()'s own Close(TRUE) is
// then a no-op, as it is for a plain socket that is gone already.
int cConHook::readfd(void *buf, int count)
{
    if (m_pSSL == nullptr)
    {
        return cHookNative::readfd(buf, count);
    }

    ERR_clear_error();

    int r = SSL_read(m_pSSL, buf, count);

    if (r > 0)
    {
        return r;
    }

    int e = SSL_get_error(m_pSSL, r);

    if (e == SSL_ERROR_WANT_READ || e == SSL_ERROR_WANT_WRITE)
    {
        errno = EAGAIN;
        return -1;
    }

    if (e == SSL_ERROR_ZERO_RETURN || (e == SSL_ERROR_SYSCALL && r == 0))
    {
        // close_notify, or the bare close most mud clients do: ZERO_RETURN
        // with SSL_OP_IGNORE_UNEXPECTED_EOF, SYSCALL with r == 0 on an
        // OpenSSL that does not have it.
        slog(LOG_OFF, 0, "TLS: %s closed the connection", m_aHost);
        m_bTlsFatal = (e != SSL_ERROR_ZERO_RETURN); // after close_notify our own may still go out
        ERR_clear_error();
        TlsDrop();
        return 0;
    }

    TlsFail("read", e);
    errno = ECONNRESET;
    return -1;
}

// cHookNative::write() contract: bytes written (short means PushWrite()
// retries the rest), 0 try again later, -1 error and unhooked.
int cConHook::write(const void *buf, int count)
{
    if (m_pSSL == nullptr)
    {
        return cHookNative::write(buf, count);
    }

    if (!IsHooked())
    {
        return -1;
    }

    if (!SSL_is_init_finished(m_pSSL))
    {
        return 0; // PushWrite() holds everything until the handshake is done
    }

    ERR_clear_error();

    int r = SSL_write(m_pSSL, buf, count);

    if (r > 0)
    {
        return r;
    }

    int e = SSL_get_error(m_pSSL, r);

    if (e == SSL_ERROR_WANT_WRITE || e == SSL_ERROR_WANT_READ)
    {
        return 0;
    }

    TlsFail("write", e);
    return -1;
}

void cConHook::TlsFree()
{
    if (m_pSSL == nullptr)
    {
        return;
    }

    if (g_tls_exiting)
    {
        // From bye_signal(), or a static destructor after OpenSSL's own
        // atexit cleanup: neither may call OpenSSL. The process is going
        // away and the kernel closes the sockets.
        m_pSSL = nullptr;
        return;
    }

    if (SSL_is_init_finished(m_pSSL) && !m_bTlsFatal)
    {
        ERR_clear_error();
        SSL_shutdown(m_pSSL); // our close_notify, best effort: the socket closes right after
        ERR_clear_error();
    }

    SSL_free(m_pSSL);
    m_pSSL = nullptr;
    m_bTlsFatal = false;
}

void cConHook::Write(ubit8 *pData, ubit32 nLen, int bCopy)
{
    if (this->m_fWebsSend)
    {
        assert(pData[nLen] == 0);
        if (!m_fWebsSend(m_pWebsHdl, (const char *)pData))
        {
            this->Close(TRUE);
        }
    }
    else
    {
        cHook::Write(pData, nLen, bCopy);
    }
}

void cConHook::Close(int bNotifyMud)
{
    if (m_nId == 0)
    {
        return;
    }

    m_mtx.lock();

    if (IsHooked())
    {
        Unhook();
    }

    if (bNotifyMud && m_nId != 0 && g_MudHook.IsHooked())
    {
        slog(LOG_ALL, 0, "Closing con id %d.", m_nId);
        protocol_send_close(&g_MudHook, m_nId);
    }

    m_aOutput[0] = 0;
    m_aInputBuf[0] = 0;
    m_nState = 0;
    m_pFptr = Idle;
    m_nId = 0;

    m_fWebsSend = nullptr;

    m_mtx.unlock();

    // Deliberately outside the m_mtx region: remove_gmap() takes
    // g_cMapHandler_mutex, and the websocket thread (on_close/on_message)
    // takes g_cMapHandler_mutex before calling into the hook. Holding m_mtx
    // here while waiting for the map mutex would be an ABBA deadlock with
    // that thread. Never hold both locks at once.
    remove_gmap(this);

    // Still have the issue that we ought to close the
    // websocket connection here and remove from g_cMapHandler - but what if we were call by
    // websockets.
}

/* ================================================================== */

// Send command to the MUD server connection
void cConHook::TransmitCommand(const char *text)
{
    char sendText[MAX_INPUT_LENGTH];
    char *d = nullptr;

    strncpy(sendText, text, MAX_INPUT_LENGTH);
    sendText[MAX_INPUT_LENGTH - 1] = 0; // MS2020 moved up to avoid strchr bug.

    if ((d = strchr(sendText, '\r')))
    {
        *d = 0;
    }

    if ((d = strchr(sendText, '\n')))
    {
        *d = 0;
    }

    strip_trailing_spaces(sendText);
    // keep input short.
    // maybe warn user if to long but we are not now.

    m_nPromptLen = 0;

    protocol_send_text(&g_MudHook, m_nId, sendText, MULTI_TEXT_CHAR);
}

/* ======================= TEXT PARSE & ECHO INPUT ====================== */

/* Maybe this would be overall easier? */
char cConHook::AddInputChar(ubit8 c)
{
    char *cp = m_aInputBuf;

    if (c == '\x1B')
    {
        m_nEscapeCode = 1;
        return 0;
    }

    if (m_nEscapeCode > 0)
    {
        if (m_nEscapeCode == 1)
        {
            if (c == '[')
            {
                m_nEscapeCode++;
                return 0;
            }
        }
        else if (m_nEscapeCode == 2)
        {
            if (c == 'D')
            {
                m_nEscapeCode = 0;
                c = '\b';
            }
        }
        else
        {
            m_nEscapeCode = 0;
        }
    }

    TAIL(cp);
    if ((c == '\b') || (c == '\177'))
    {
        if (cp != m_aInputBuf)
        {
            *(cp - 1) = 0;
            return '\b';
        }

        return 0;
    }

    m_nEscapeCode = 0;

    *cp++ = c;
    *cp = 0;

    if ((c < ' ') || (c == 255) || (m_sSetup.telnet && (c > 127)))
    {
        *(cp - 1) = 0;
    }

    return *(cp - 1);
}

void cConHook::AddString(char *str)
{
    char *s = nullptr;
    char echobuf[4096]; // ~= 12 * sizeof(str) = 12 * 256 = 3000

    char *eb = nullptr;
    char c = 0;

    eb = echobuf;

    for (s = str; *s; s++)
    {
        if (ISNEWL(*s) || (strlen(m_aInputBuf) >= MAX_INPUT_LENGTH - 4))
        {
            while (ISNEWL(*(s + 1)))
            {
                s++;
            }
            *eb++ = '\n';
            *eb++ = '\r';
            m_qInput.Append(new cQueueElem(m_aInputBuf));
            m_aInputBuf[0] = 0;
        }
        else
        {
            int x_pos = (m_nPromptLen + strlen(m_aInputBuf)) % m_sSetup.width;
            c = AddInputChar(*s);
            if (c)
            {
                if (c == '\b')
                {
                    if (m_sSetup.emulation == TERM_ANSI)
                    {
                        if (x_pos == 0)
                        {
                            sprintf(eb, "[A[%dC[K", m_sSetup.width - 1);
                            TAIL(eb);
                        }
                        else
                        {
                            strcpy(eb, "[D[K");
                            TAIL(eb);
                        }
                    }
                    else
                    {
                        *eb++ = '\b';
                        *eb++ = ' ';
                        *eb++ = '\b';
                    }
                }
                else
                {
                    *eb++ = c;
                }
            }
        }
    }

    *eb = 0;
    assert(eb - echobuf < (int)sizeof(echobuf) - 1);

    if (m_sSetup.echo)
    {
        Write((ubit8 *)echobuf, eb - echobuf);
    }
}

/* On -1 'con' was destroyed */
void cConHook::ParseInput()
{
    if (m_qInput.Size() > 50)
    {
        SendCon("STOP FLOODING ME!!!<br/>");

        if (m_qInput.Size() > 100)
        {
            Close(TRUE);
            return;
        }
    }
}

/* ======================= STUFF ====================== */

void cConHook::ProcessPaged()
{
    if (m_pFptr != dumbPressReturn)
    {
        m_pFptr = dumbPressReturn;
        PressReturn("");
    }
}

void cConHook::Input(int nFlags)
{
    if (nFlags & SELECT_EXCEPT)
    {
        if (m_pSSL)
        {
            TlsDrop();
        }
        else
        {
            Close(TRUE);
        }
        return;
    }

    if (!(nFlags & SELECT_READ))
    {
        return;
    }

    // One TLS record can hold more than one read()'s worth of bytes, and
    // select() will not fire again for what OpenSSL already holds in its
    // buffer, so drain it here. A plain socket goes round exactly once.
    while (InputOnce())
    {
        if (!m_pSSL || !IsHooked() || SSL_pending(m_pSSL) <= 0)
        {
            break;
        }
    }
}

// One read and its processing. False when there is nothing more to do on
// this wake-up (no bytes yet, or the connection closed); true when bytes
// were consumed and the connection is still up.
bool cConHook::InputOnce()
{
    char *c = nullptr;
    ubit8 buf[1024];

    if (m_pSSL && !TlsHandshake())
    {
        return false; // needs more from the peer, or failed and the connection is gone
    }

    int n = readfd(buf, sizeof(buf) - 1); // ::read() contract, TLS or plain

    if (n == -1)
    {
#if defined(_WINDOWS)
        if (WSAGetLastError() == WSAEWOULDBLOCK || WSAGetLastError() == WSAEINTR)
        {
            return false;
        }
#else
        if ((errno == EWOULDBLOCK) || (errno == EAGAIN))
        {
            return false;
        }
#endif

        Close(TRUE);
        return false;
    }
    else if (n == 0)
    {
        Close(TRUE);
        return false;
    }

    if (m_nSequenceCompare != -1)
    {
        SequenceCompare(buf, &n);
        if (n <= 0)
        {
            return true;
        }
    }

    if (m_nFirst >= 0)
    {
        getLine(buf, &n);
        if (n <= 0)
        {
            return true;
        }
    }

    // Runs deliberately AFTER the two one-shot legacy filters above:
    // the serial-line hack's signature IS telnet bytes (IAC WILL ECHO),
    // and both filters self-disarm on the first non-matching byte
    // without consuming anything, so a modern client's buffer reaches
    // the parser intact. Skipped for TERM_INTERNAL (the W32 client sets
    // telnet = FALSE) and 'set telnet off' users.
    if (m_sSetup.telnet && n > 0)
    {
        TelnetParse(buf, &n);
        if (n <= 0)
        {
            return true;
        }
    }

    buf[n] = 0;
    AddString((char *)buf);

    ParseInput();

    cQueueElem *qe = nullptr;

    while ((qe = m_qInput.GetHead()))
    {
        c = (char *)qe->Data();
        assert(strlen(c) < MAX_INPUT_LENGTH);
        m_pFptr(this, c);
        delete qe;
    }

    return true;
}

/* ================= sending data to client output handling ====================== */

/* -1 on connection closed, 0 on success */
void cConHook::WriteCon(const char *text)
{
    if (text == nullptr)
    {
        return;
    }

    Write((ubit8 *)text, strlen(text));
}

void cConHook::WriteCon(const std::string &text)
{
    WriteCon(text.c_str());
}

void cConHook::SendCon(const char *text)
{
    WriteCon(ParseOutput(text));
}

void cConHook::SendCon(const std::string &text)
{
    SendCon(text.c_str());
}

/* ======================= TEXT FORMATTING OUTPUT ====================== */

std::string mplex_getcolor(cConHook *hook, const char *colorstr)
{
    auto gcolor = hook->color.get(colorstr);

    if (gcolor.empty())
    {
        gcolor = g_cDefcolor.get(colorstr);
    }

    return gcolor;
}

// s is at an ESC: returns just past the final letter of its colour code,
// e.g. ESC [ 1 ; 3 2 m, or to if it ends there
static const char *SkipColourCode(const char *s, const char *to)
{
    for (s++; s < to && !isalpha(static_cast<unsigned char>(*s)); s++)
    {
        ;
    }
    return s < to ? s + 1 : to;
}

char *cConHook::IndentText(const char *source, char *dest, int dest_size, int width)
{
    const char *last = nullptr; // The last space after text on this line, where it can break
    bool bLineHasText = false;  // Anything but blanks copied on this line
    bool bDropBlanks = false;   // Blanks and tabs after a break go
    const char *current = nullptr;
    char tmpbuf[MAX_STRING_LENGTH * 2];
    int i = 0;
    char *newptr = nullptr;
    int column = 0;

    if (!(current = source))
    {
        return nullptr;
    }

    newptr = dest;

    while (*current)
    {
        if ((newptr + 100) > dest + dest_size)
        {
            *newptr = 0;
            break;
        }

        if (*current == CONTROL_CHAR)
        {
            *newptr++ = *current++; // Copy control char

            for (int j = 0; j < 20; j++) // Skip the control sequence which ends with 'm'
            {
                *newptr++ = *current; // copy char

                if (*current == 0)
                {
                    break;
                }

                if (*current == 'm')
                {
                    current++;
                    break;
                }
                current++;
            }
            continue;
        }

        if (m_bGobble)
        {
            current++;
            continue;
        }

        if (m_bColorDisp == true)
        {
            *tmpbuf = '\0';
            i = 0;

            while (m_bColorDisp == true && *current)
            {
                if (*current == CONTROL_CHAR)
                {
                    current++;
                    protocol_translate(this, *current, &newptr);
                    if (*current == CONTROL_COLOR_END_CHAR)
                    {
                        auto mplex_color = mplex_getcolor(this, tmpbuf);
                        if (mplex_color.empty() == false)
                        {
                            auto cretbuf = mplex_color.begin();
                            while (cretbuf != mplex_color.end())
                            {
                                if (*cretbuf == CONTROL_CHAR)
                                {
                                    cretbuf++;
                                    protocol_translate(this, *cretbuf, &newptr);
                                    cretbuf++;

                                    continue;
                                }
                                else
                                {
                                    cretbuf++;
                                }
                            }
                        }
                    }
                    current++;
                }
                else
                {
                    tmpbuf[i] = tolower(*current);
                    tmpbuf[i + 1] = 0;
                    i++;
                    current++;
                }
            }
            continue;
        }

        if (m_bColorCreate == true)
        {
            *tmpbuf = '\0';
            i = 0;
            while (m_bColorCreate == true && *current)
            {
                if ((*current == CONTROL_CHAR) && (*(current + 1) == CONTROL_COLOR_END_CHAR))
                {
                    current++;
                    protocol_translate(this, *current, &newptr);
                    if (!*current || *current == CONTROL_COLOR_END_CHAR)
                    {
                        color.create(tmpbuf);
                        m_bColorCreate = false;
                    }
                    current++;
                }
                else
                {
                    tmpbuf[i] = tolower(*current);
                    tmpbuf[i + 1] = 0;
                    i++;
                    current++;
                }
            }
            continue;
        }

        if (m_bColorChange == true)
        {
            *tmpbuf = '\0';
            i = 0;
            while (m_bColorChange == true && *current)
            {
                if ((*current == CONTROL_CHAR) && (*(current + 1) == CONTROL_COLOR_END_CHAR))
                {
                    current++;
                    protocol_translate(this, *current, &newptr);
                    if (!*current || *current == CONTROL_COLOR_END_CHAR)
                    {
                        color.change(tmpbuf);
                        m_bColorChange = false;
                    }
                    current++;
                }
                else
                {
                    tmpbuf[i] = tolower(*current);
                    tmpbuf[i + 1] = 0;
                    i++;
                    current++;
                }
            }
            continue;
        }

        if (m_bColorInsert == true)
        {
            *tmpbuf = '\0';
            i = 0;
            while (m_bColorInsert == true && *current)
            {
                if ((*current == CONTROL_CHAR) && (*(current + 1) == CONTROL_COLOR_END_CHAR))
                {
                    current++;
                    protocol_translate(this, *current, &newptr);
                    if (!*current || *current == CONTROL_COLOR_END_CHAR)
                    {
                        color.insert(tmpbuf);
                        m_bColorInsert = false;
                    }
                    current++;
                }
                else
                {
                    tmpbuf[i] = tolower(*current);
                    tmpbuf[i + 1] = 0;
                    i++;
                    current++;
                }
            }
            continue;
        }

        if (m_bColorRemove == true)
        {
            *tmpbuf = '\0';
            i = 0;
            while (m_bColorRemove == true && *current)
            {
                if ((*current == CONTROL_CHAR) && (*(current + 1) == CONTROL_COLOR_END_CHAR))
                {
                    current++;
                    protocol_translate(this, *current, &newptr);
                    if (!*current || *current == CONTROL_COLOR_END_CHAR)
                    {
                        color.remove(tmpbuf);
                        m_bColorRemove = false;
                    }
                    current++;
                }
                else
                {
                    tmpbuf[i] = tolower(*current);
                    tmpbuf[i + 1] = 0;
                    i++;
                    current++;
                }
            }
            continue;
        }

        if (bDropBlanks)
        {
            if (*current == ' ' || *current == '\t')
            {
                current++;
                continue;
            }
            bDropBlanks = false;
        }

        if (isaspace(*current))
        { /* Remember last space, if breaking there leaves no empty line */
            last = bLineHasText ? current : nullptr;
        }

        if (*current == '\n' || *current == '\r')
        { /* Newlines signify new col, and take none themselves */
            column = 0;
            last = nullptr;
            bLineHasText = false;
            *(newptr++) = *(current++);
            continue;
        }

        if (++column > width && last)
        { /* Out of space: the newline replaces the last space */
            newptr -= (current - last);
            current = last + 1;

            // Unless a newline follows anyway, past blanks, tabs and colour codes
            const char *next = current;
            while (*next == ' ' || *next == '\t' || *next == CONTROL_CHAR)
            {
                next = *next == CONTROL_CHAR ? SkipColourCode(next, next + strlen(next)) : next + 1;
            }
            if (*next != '\n' && *next != '\r')
            {
                *(newptr++) = '\n';
                *(newptr++) = '\r';
            }
            bDropBlanks = true;
            column = 0;
            last = nullptr;
            bLineHasText = false;
            continue;
        }

        // A word is never cut: one too long for the line (e.g. a web address)
        // runs on for the terminal to wrap, so it can still be followed
        if (*current != ' ' && *current != '\t')
        {
            bLineHasText = true;
        }
        *(newptr++) = *(current++);
    }

    *newptr = '\0';

    assert(dest + dest_size > newptr);

    return dest;
}

// Punctuation that can end a sentence right after a web address without
// being read as part of it
static const char *g_pAddressPunctuation = ".,;:!?)]}>\"'";

// The visible text from..to (colour codes skipped) into buf, at most
// nMax - 1 chars (none if nMax is 0); *pbCut tells if it didn't all fit.
// Returns its last char, or 0 if none.
static char VisibleText(const char *from, const char *to, char *buf, size_t nMax, bool *pbCut)
{
    size_t n = 0;
    char cLast = 0;

    *pbCut = false;
    for (const char *s = from; s < to;)
    {
        if (*s == CONTROL_CHAR)
        {
            s = SkipColourCode(s, to);
            continue;
        }
        if (n + 1 < nMax)
        {
            buf[n++] = *s;
        }
        else
        {
            *pbCut = true;
        }
        cLast = *s++;
    }
    if (nMax > 0)
    {
        buf[n] = 0;
    }

    return cLast;
}

// The last visible char from..to (colour codes skipped), or cLast if none
static char LastVisibleChar(const char *from, const char *to, char cLast)
{
    bool bCut = false;
    char c = VisibleText(from, to, nullptr, 0, &bCut);

    return c ? c : cLast;
}

// Where the web address pHref shows in the link text pText, a final '/' and
// the case of the scheme and host aside (the rest of an address can be
// case-sensitive): returns the end of it, or nullptr. Only punctuation may
// follow it before a blank or the end. bCut: pText was cut short.
static const char *FindAddress(const char *pText, bool bCut, const char *pHref)
{
    size_t lHref = strlen(pHref);
    if (lHref > 0 && pHref[lHref - 1] == '/')
    {
        lHref--;
    }

    // The scheme and host end at the first '/' after "://"
    const char *sep = strstr(pHref, "://");
    size_t lHost = sep ? MIN(static_cast<size_t>(sep + 3 - pHref), lHref) : 0;
    while (lHost < lHref && pHref[lHost] != '/')
    {
        lHost++;
    }

    for (const char *t = pText; *t; t++)
    {
        if (strncasecmp(t, pHref, lHost) != 0 || strncmp(t + lHost, pHref + lHost, lHref - lHost) != 0)
        {
            continue;
        }

        const char *end = t + lHref;
        if (*end == '/')
        {
            end++;
        }
        const char *after = end + strspn(end, g_pAddressPunctuation);
        if (*after == 0 ? !bCut : isspace(static_cast<unsigned char>(*after)))
        {
            return end;
        }
    }
    return nullptr;
}

// At the end of a link (its </a>, or the next <a>), add its web address
// pHref to dest as "text: address". The link text is in dest from pLinkText
// on, and cBefore is the last visible char before the link (0 if none).
// Nothing is added when the text already shows the address. The address is
// kept apart from the last visible char by a blank, and from link text by
// ": " unless the text already ends in ':'. *pbAddressLast tells if dest
// now ends with the address. Adds at most strlen(pHref) + 2 bytes, and
// StripHTML() one blank more after it: fewer than the <a href='...'> tag
// replaced, so a link never makes the output longer than the HTML.
static char *AppendLinkAddress(char *dest, const char *pLinkText, char cBefore, const char *pHref, bool *pbAddressLast)
{
    size_t l = strlen(pHref);
    char aText[512];
    bool bCut = false;
    char cLast = VisibleText(pLinkText, dest, aText, sizeof(aText), &bCut);

    const char *pShown = FindAddress(aText, bCut, pHref);
    if (pShown)
    {
        *pbAddressLast = *pShown == 0;
        return dest;
    }

    char c = cLast ? cLast : cBefore;
    if (c != 0 && !isspace(static_cast<unsigned char>(c)))
    {
        if (cLast != 0 && c != ':')
        {
            *dest++ = ':';
        }
        *dest++ = ' ';
    }
    memcpy(dest, pHref, l);
    *pbAddressLast = true;

    return dest + l;
}

// Whether the lowercased tag aTag is name, alone or followed by attributes
// or a '/' (</a/> closes a link in a browser too)
static bool IsTag(const char *aTag, const char *name)
{
    size_t l = strlen(name);

    return strncmp(aTag, name, l) == 0 && (aTag[l] == 0 || aTag[l] == '/' || isspace(static_cast<unsigned char>(aTag[l])));
}

/*  Misleading name. Function changes HTML to TELNET
 *  <br/> becomes \n\r except when only blanks or tabs are left before the
 *  end of the line (\n or \r\n): then it is ignored. Thus
 *  <br/><br/>\n\r becomes \n\r\n\r.
 *  That's by design to support both telnet and HTML reasonably.
 */
void cConHook::StripHTML(char *dest, size_t nDestMax, const char *src)
{
    char *const pDestEnd = dest + nDestMax;

    if (g_mplex_arg.g_bModeRawHTML)
    {
        snprintf(dest, nDestMax, "%s", src);
        return;
    }

    const char *p = nullptr;
    const char *t = nullptr;
    // The web address of the open <a> and where its text starts in dest. A
    // link split over two outputs (the server splits text over 32 KB) loses
    // its address.
    char aHref[256] = "";
    char *pLinkText = dest;
    // The last visible char in dest up to pScanned, so each byte is looked at once
    const char *pScanned = dest;
    char cScanned = 0;
    // After an address printed for a link: how far dest has been looked at
    // for what follows it, and where the first visible char after it is
    char *pAfterAddress = nullptr;
    char *pFirstAfter = nullptr;

    p = src;

    while (*p)
    {
        // Colour codes make the output longer than the HTML (</div> becomes
        // about 22 bytes), so stop short of the end of dest. One step below
        // writes at most what one tag holds: a 256-byte aTag, or up to three
        // colour codes for a div class. Recheck this if a step writes more.
        constexpr ptrdiff_t STEP_MAX = 512;
        if (pDestEnd - dest < STEP_MAX)
        {
            slog(LOG_ALL, 0, "StripHTML(): output too long for a %zu-byte buffer, cut short", nDestMax);
            break;
        }

        // Data for the web client, not text. The server drops it for telnet
        // before splitting text over 32 KB (send_to_descriptor()); this
        // catches what reaches mplex some other way, one output at a time.
        const char *pAfterData = skipHTMLData(p);
        if (pAfterData != p)
        {
            p = pAfterData;
            continue;
        }

        if (*p == '<')
        {
            char aTag[256];
            const char *pTagText = p + 1;

            p = getHTMLTag(p, aTag, sizeof(aTag));

            if (aTag[0] == 0)
            {
                continue;
            }

            // We got a HTML tag
            if (IsTag(aTag, "br"))
            {
                const char *q = p;
                while (*q == ' ' || *q == '\t')
                    q++;
                if (*q == '\n' || (*q == '\r' && *(q + 1) == '\n'))
                { // A real newline follows (maybe after trailing blanks), so dont add <br>
                    continue;
                }

                // the <br/> tag was not followed by \n\r so add \n\r
                *dest++ = '\n';
                *dest++ = '\r';
                continue;
            }
            if (strcmp(aTag, "/tr") == 0)
            {
                *dest++ = '\n';
                *dest++ = '\r';
                continue;
            }
            if (strcmp(aTag, "/td") == 0)
            {
                // Separate table cells so "*Cypress Forest" and "Mainland" don't run together
                *dest++ = ' ';
                *dest++ = ' ';
                continue;
            }
            if (IsTag(aTag, "a") || IsTag(aTag, "/a"))
            {
                // Telnet can't follow links, so a web address is printed after the
                // link text: at its </a>, or where the next <a> closes it (as in a
                // browser).
                if (aHref[0])
                {
                    cScanned = LastVisibleChar(pScanned, pLinkText, cScanned);
                    pScanned = pLinkText;
                    bool bAddressLast = false;
                    dest = AppendLinkAddress(dest, pLinkText, cScanned, aHref, &bAddressLast);
                    if (bAddressLast)
                    {
                        pAfterAddress = dest;
                        pFirstAfter = nullptr;
                    }
                }
                aHref[0] = 0;

                if (aTag[0] == 'a')
                { // aTag is lowercased; the address is read as sent
                    getHTMLLinkTarget(pTagText, aHref, sizeof(aHref));
                    pLinkText = dest;
                }
                continue;
            }
            if (strcmp(aTag, "go-ahead/") == 0)
            {
                // Clients that negotiated EOR (e.g. Mudlet) get the modern
                // prompt terminator; everyone else keeps the classic GA
                *dest++ = IAC;
                *dest++ = m_bEorOk ? EOR : GA;
                continue;
            }
            if (strcmp(aTag, "script") == 0)
            {
                if (strncasecmp(p, "PasswordOn()", 12) == 0)
                {
                    Control_Echo_Off(this, &dest, 0);
                    p += 12;
                }
                else if (strncasecmp(p, "PasswordOff(", 12) == 0)
                {
                    Control_Echo_On(this, &dest, 0);
                    p += 12;
                }
                t = strstr(p, "</script>");
                if (t)
                {
                    p = t + 9;
                }
                continue;
            }
            else if (strncmp(aTag, "/div", 4) == 0)
            {
                // Hm, this'll send a lot of code... but telnet's a hack now :)
                // How on earth will I get the "default color" if someone prefers e.g. yellow... ?
                Control_ANSI_Fg(this, &dest, 'w', FALSE);
                Control_ANSI_Bg(this, &dest, 'n');
            }
            else if ((strncmp(aTag, "div ", 4) == 0) || (strncmp(aTag, "h1 ", 3) == 0))
            {
                char buf[256];
                int l = 0;

                if (m_bGmcpOk)
                {
                    // The prompt div's bars attribute -> GMCP Char.Vitals
                    l = getHTMLValue("bars", aTag, buf, sizeof(buf) - 1);
                    if (l != 0)
                    {
                        GmcpVitals(buf);
                    }

                    // The room header's roomid attribute -> GMCP Room.Info
                    // (peeks ahead at the title text without consuming it;
                    // the visible output below is unaffected)
                    l = getHTMLValue("roomid", aTag, buf, sizeof(buf) - 1);
                    if (l != 0)
                    {
                        GmcpRoomInfo(aTag, buf, p);
                    }
                }

                l = getHTMLValue("class", aTag, buf, sizeof(buf) - 1);

                if (l == 0)
                {
                    continue;
                }

                // We got a single or double color code on our hands
                // e.g. cpg, cg bn, cpy bb, etc.
                if ((l >= 2) && (l <= 6) && ((buf[0] == 'b') || (buf[0] == 'c')))
                {
                    char *p = nullptr;
                    char tmp[256];

                    p = str_next_word(buf, tmp);

                    while (tmp[0])
                    {
                        if (tmp[0] == 'c')
                        {
                            if (tmp[1] == 'p')
                            {
                                Control_ANSI_Fg(this, &dest, tmp[2], TRUE);
                            }
                            else
                            {
                                Control_ANSI_Fg(this, &dest, tmp[1], FALSE);
                            }
                        }
                        else if (tmp[0] == 'b')
                        {
                            Control_ANSI_Bg(this, &dest, tmp[1]);
                        }
                        else
                        {
                            slog(LOG_ALL, 0, "Illegal color code %s", buf);
                            break;
                        }

                        p = str_next_word(p, tmp);
                    }
                }
            }
            continue;
        }

        if (*p == '&')
        {
            if (int nChars = decodeHTMLEntity(p, dest))
            {
                dest += nChars;
            }
            else
            {
                *dest++ = *p++;
            }
        }
        else if (*p == '\n' && *(p + 1) != '\r')
        {
            *dest++ = '\n';
            *dest++ = '\r';
            p++;
        }
        else
        {
            *dest++ = *p++;
        }

        // Text right after a printed address is kept apart from it by a
        // blank, or it would read as part of the address. Punctuation right
        // after it stays attached when a blank, a line break or the end of the
        // output follows: the first visible char that isn't punctuation decides.
        while (pAfterAddress && pAfterAddress < dest)
        {
            if (*pAfterAddress == CONTROL_CHAR)
            {
                pAfterAddress = const_cast<char *>(SkipColourCode(pAfterAddress, dest));
                continue;
            }
            if (pFirstAfter == nullptr)
            {
                pFirstAfter = pAfterAddress;
            }
            if (strchr(g_pAddressPunctuation, *pAfterAddress))
            {
                pAfterAddress++;
                continue;
            }

            if (!isspace(static_cast<unsigned char>(*pAfterAddress)))
            {
                memmove(pFirstAfter + 1, pFirstAfter, dest - pFirstAfter);
                *pFirstAfter = ' ';
                dest++;
                if (pLinkText > pFirstAfter)
                {
                    pLinkText++; // A link opened after the address moved too
                }
                if (pScanned > pFirstAfter)
                {
                    pScanned++;
                }
            }
            pAfterAddress = nullptr;
        }
    }
    *dest = 0;
}

// Parse the string 'text' and prepare it for output on 'con'
// This should only be called for telnet
char *cConHook::ParseOutput(const char *text)
{
    extern arg_type g_mplex_arg;

    static char Outbuf[65536];  // StripHTML() and IndentText() stop at the end of these;
    static char Outbuf2[65536]; // growing them instead of cutting long output is still to do

    assert(strlen(text) < sizeof(Outbuf));

    if (this->m_fWebsSend && !g_mplex_arg.bForceAscii)
    {
        size_t n = strlen(text);
        if (n < sizeof(Outbuf))
        {
            strcpy(Outbuf, text);
        }
        else
        {
            memcpy(Outbuf, text, sizeof(Outbuf));
            Outbuf[sizeof(Outbuf) - 100] = 0;
        }

        return Outbuf;
    }
    else
    {
        StripHTML(Outbuf2, sizeof(Outbuf2), text);

        return IndentText(Outbuf2, Outbuf, sizeof(Outbuf), m_sSetup.width);
    }
}

/* Attempts to erase the characters input after the prompt
void cConHook::PromptErase (void)
{
    int n;
    char buf[MAX_STRING_LENGTH];
    char *b = buf;

    if (m_nPromptLen == 0)
        return;

    *b = 0;

    if ((m_sSetup.emulation == TERM_TTY) ||
            (m_sSetup.emulation == TERM_VT100))
    {
        for (n = 0; m_aInputBuf[n]; n++)
            *b++ = '\b';
        for (n = 0; m_aInputBuf[n]; n++)
            *b++ = ' ';
        *b = 0;
    }
    else if (m_sSetup.emulation == TERM_ANSI)
    {
        int len = strlen (m_aInputBuf);
        int lines = 0;
        int i;

        if (len > 0)
        {
            lines = (m_nPromptLen + len - 1) / m_sSetup.width;

            for (i = 0; i < lines; i++)
            {
                sprintf (b, "[%dD[K[A",
                         m_sSetup.width);
                TAIL (b);
            }

            if (m_nPromptLen > 1)
                sprintf (b, "[%dD[%dC[K",
                         m_sSetup.width,
                         m_nPromptLen - 1);
            else
                sprintf (b, "[%dD[K",
                         m_sSetup.width);

            TAIL (b);
        }
    }

    m_nPromptLen = 0;

    if (*buf)
        Write ((ubit8 *) buf, strlen (buf));
}
*/

/* Assumes that a newline has already been output  */
void cConHook::PromptRedraw(const char *prompt)
{
    if (*prompt)
    {
        Write((ubit8 *)prompt, strlen(prompt));
    }

    if (*m_aInputBuf)
    {
        Write((ubit8 *)m_aInputBuf, strlen(m_aInputBuf));
    }

    m_nPromptLen = strlen(prompt);
}

/* ======================= 'Con' handling ====================== */

void cConHook::SequenceCompare(ubit8 *pBuf, int *pnLen)
{
    // static const char *match = "a";
    static const char *match = "[W32-V/C@SiGn]";
    /*  static int state = 0;*/

    assert(m_nSequenceCompare != -1);

    for (int i = 0; i < *pnLen; i++)
    {
        if (pBuf[i] == match[m_nSequenceCompare])
        {
            if (match[m_nSequenceCompare + 1] == 0)
            {
                m_nSequenceCompare = -1;

                m_sSetup.echo = FALSE;
                m_sSetup.redraw = FALSE;
                m_sSetup.emulation = TERM_INTERNAL;
                m_sSetup.telnet = FALSE;

                memmove(pBuf, pBuf + strlen(match), *pnLen - strlen(match));
                *pnLen -= strlen(match);

                return;
            }
        }
        else
        {
            m_nSequenceCompare = -1;
            return;
        }

        m_nSequenceCompare++;
    }
}

void cConHook::testChar(ubit8 c)
{
    switch (m_nFirst)
    {
        case 0:
            if (c == IAC)
            {
                m_nFirst++;
            }
            else
            {
                m_nFirst = -1;
            }
            break;

        case 1:
            if (c == WILL)
            {
                m_nFirst++;
            }
            else
            {
                m_nFirst = -1;
            }
            break;

        case 2:
            if (c == TELOPT_ECHO)
            {
                m_nFirst++;
            }
            else
            {
                m_nFirst = -1;
            }
            break;

        case 3:
            m_nLine = c;
            m_nFirst++;
            break;

        case 4:
            if (c == IAC)
            {
                m_nFirst++;
            }
            else
            {
                m_nFirst = -1;
                m_nLine = 255;
            }
            break;

        case 5:
            if (c == WONT)
            {
                m_nFirst++;
            }
            else
            {
                m_nFirst = -1;
                m_nLine = 255;
            }
            break;

        case 6:
            if (c == TELOPT_ECHO)
            {
                m_nFirst = -2;
            }
            else
            {
                m_nFirst = -1;
                m_nLine = 255;
            }
            break;

        default:
            m_nFirst = -1;
            m_nLine = 255;
            break;
    }
}

void cConHook::getLine(ubit8 buf[], int *size)
{
    int i = 0;

    for (i = 0; i < *size; i++)
    {
        testChar(buf[i]);

        // slog(LOG_ALL, 0, "Testchar %d, first %d.", buf[i], m_nFirst);

        if (m_nFirst == -1)
        {
            return;
        }
        if (m_nFirst == -2)
        {
            break;
        }
    }

    if (i < *size)
    {
        memmove(buf, buf + i + 1, *size - (i + 1));
    }
    *size -= i;
}

/* ==================== TELNET OPTION NEGOTIATION ==================== */
/*
 * A minimal RFC 854 negotiation core for the telnet path. Before this
 * existed, client negotiation bytes were dropped one at a time by
 * AddInputChar()'s range filter - and option bytes in the printable range
 * leaked into the input line (a TTYPE payload could literally end up as
 * the game command "Mudlet"). mplex also never answered anything, so
 * clients timed out their probes and fell back to dumb defaults.
 *
 * Supported: NAWS (window size -> live wrap/paging), TTYPE incl. the MTTS
 * walk (client name/capabilities), EOR (preferred prompt terminator for
 * e.g. Mudlet - see the go-ahead conversion in StripHTML()), and GMCP
 * (out-of-band JSON packages: Char.Vitals from the prompt bars, Room.Info
 * from the room header - see the Gmcp* methods below). Everything else is
 * politely refused. All negotiation state lives with the TCP connection
 * and survives a MUD reboot (see ResetSessionState()).
 */

// GMCP (Generic MUD Communication Protocol) telnet option - not in
// arpa/telnet.h, which predates it
#define TELOPT_GMCP 201

// Append an IAC <verb> <option> triplet to the response buffer
static void telnet_reply(ubit8 *pOut, int *pnOutLen, ubit8 verb, ubit8 opt)
{
    pOut[(*pnOutLen)++] = IAC;
    pOut[(*pnOutLen)++] = verb;
    pOut[(*pnOutLen)++] = opt;
}

// Append IAC SB TTYPE SEND IAC SE - ask the client for (the next)
// terminal type
static void telnet_ttype_send(ubit8 *pOut, int *pnOutLen)
{
    pOut[(*pnOutLen)++] = IAC;
    pOut[(*pnOutLen)++] = SB;
    pOut[(*pnOutLen)++] = TELOPT_TTYPE;
    pOut[(*pnOutLen)++] = TELQUAL_SEND;
    pOut[(*pnOutLen)++] = IAC;
    pOut[(*pnOutLen)++] = SE;
}

// React to IAC <cmd> <opt>. Responds only on state change so two
// well-behaved endpoints can never enter a negotiation loop, and never
// answers what is merely the ack of our own initial request.
void cConHook::TelnetNegotiate(ubit8 cmd, ubit8 opt, ubit8 *pOut, int *pnOutLen)
{
    switch (cmd)
    {
        case WILL:
            if (opt == TELOPT_NAWS)
            {
                m_bNawsOk = true; // Ack of our DO NAWS; SB follows
            }
            else if (opt == TELOPT_TTYPE)
            {
                if (!m_bTtypeOk)
                {
                    m_bTtypeOk = true;
                    m_nTtypeCount = 0;
                    telnet_ttype_send(pOut, pnOutLen);
                }
            }
            else if (opt == TELOPT_ECHO)
            {
                // Client offering to echo; we always echo server side
                telnet_reply(pOut, pnOutLen, DONT, opt);
            }
            else
            {
                telnet_reply(pOut, pnOutLen, DONT, opt);
            }
            break;

        case WONT:
            if (opt == TELOPT_NAWS)
            {
                m_bNawsOk = false;
            }
            else if (opt == TELOPT_TTYPE)
            {
                m_bTtypeOk = false;
            }
            // No reply: refusing an already-off option needs no ack
            break;

        case DO:
            if (opt == TELOPT_EOR)
            {
                m_bEorOk = true; // Ack of our WILL EOR
            }
            else if (opt == TELOPT_GMCP)
            {
                m_bGmcpOk = true; // Ack of our WILL GMCP; SB packages follow
            }
            else if (opt == TELOPT_ECHO)
            {
                // Echo mode is driven by the PasswordOn()/PasswordOff()
                // script tags (Control_Echo_Off/On in translate.cpp) which
                // embed IAC WILL/WONT ECHO in the output stream. There is no
                // state here to answer from - swallow, so we neither fight
                // the password logic nor leak the bytes into the input line.
            }
            else
            {
                telnet_reply(pOut, pnOutLen, WONT, opt);
            }
            break;

        case DONT:
            if (opt == TELOPT_EOR)
            {
                if (m_bEorOk)
                {
                    m_bEorOk = false;
                    telnet_reply(pOut, pnOutLen, WONT, opt);
                }
            }
            else if (opt == TELOPT_GMCP)
            {
                if (m_bGmcpOk)
                {
                    m_bGmcpOk = false;
                    telnet_reply(pOut, pnOutLen, WONT, opt);
                }
            }
            // TELOPT_ECHO: swallowed, same reason as DO ECHO above.
            // Unknown options: already off, no ack needed.
            break;
    }
}

// A complete IAC SB ... IAC SE arrived in m_aSubneg (option in [0])
void cConHook::TelnetSubneg(ubit8 *pOut, int *pnOutLen)
{
    if (m_aSubneg[0] == TELOPT_NAWS && m_nSubnegLen == 5)
    {
        int w = (m_aSubneg[1] << 8) | m_aSubneg[2];
        int h = (m_aSubneg[3] << 8) | m_aSubneg[4];

        // 0 means "unknown" per RFC 1073 - keep the current dimension.
        // Clamp to the same range the MULTI_SETUP_CHAR handler asserts.
        if (w > 0)
        {
            m_nNawsWidth = MAX(40, MIN(w, 240));
            m_sSetup.width = m_nNawsWidth;
        }
        if (h > 0)
        {
            m_nNawsHeight = MAX(15, MIN(h, 60));
            m_sSetup.height = m_nNawsHeight;
        }
    }
    else if (m_aSubneg[0] == TELOPT_TTYPE && m_nSubnegLen >= 2 && m_aSubneg[1] == TELQUAL_IS)
    {
        char name[sizeof(m_aClientName)];
        int len = MIN(m_nSubnegLen - 2, (int)sizeof(name) - 1);
        int o = 0;

        for (int i = 0; i < len; i++)
        {
            ubit8 c = m_aSubneg[2 + i];
            if (c >= ' ' && c <= 126)
            {
                name[o++] = c;
            }
        }
        name[o] = 0;

        if (m_nTtypeCount == 0)
        {
            strncpy(m_aClientName, name, sizeof(m_aClientName) - 1);
            m_aClientName[sizeof(m_aClientName) - 1] = 0;
            slog(LOG_ALL, 0, "Client terminal type: %s", m_aClientName);
        }

        if (strncmp(name, "MTTS ", 5) == 0)
        {
            m_nMTTS = atoi(name + 5); // End of the MTTS walk
        }
        else if (m_nTtypeCount < 3 && strcmp(name, m_aClientName) != 0)
        {
            m_nTtypeCount++;
            telnet_ttype_send(pOut, pnOutLen);
        }
        else if (m_nTtypeCount == 0)
        {
            // First response: ask once more to walk toward MTTS. The
            // repeat-response check above terminates against clients that
            // answer SEND forever with the same string.
            m_nTtypeCount++;
            telnet_ttype_send(pOut, pnOutLen);
        }
    }
    else if (m_aSubneg[0] == TELOPT_GMCP && m_nSubnegLen >= 2)
    {
        // Client package: "Package.Name {json}". We accept everything and
        // send our packages regardless of Core.Supports (clients ignore
        // packages they did not ask for); Core.Hello is logged next to the
        // TTYPE client identification.
        char payload[256];
        int len = MIN(m_nSubnegLen - 1, (int)sizeof(payload) - 1);

        memcpy(payload, m_aSubneg + 1, len);
        payload[len] = 0;

        if (strncmp(payload, "Core.Hello", 10) == 0)
        {
            slog(LOG_ALL, 0, "GMCP Core.Hello:%s", payload + 10);
        }
    }
    // Other subnegotiations: discard silently
}

// Parse (and consume) telnet protocol bytes from the read buffer, in
// place, keeping only regular data. Length based - never relies on NUL
// termination - and the state survives across read() calls, so sequences
// split over TCP segments are handled.
void cConHook::TelnetParse(ubit8 *pBuf, int *pnLen)
{
    ubit8 out[128];
    int outlen = 0;
    int j = 0;

    for (int i = 0; i < *pnLen; i++)
    {
        ubit8 c = pBuf[i];

        switch (m_nTelnetState)
        {
            case TS_DATA:
                if (c == IAC)
                {
                    m_nTelnetState = TS_IAC;
                }
                else
                {
                    pBuf[j++] = c;
                }
                break;

            case TS_IAC:
                if (c == IAC)
                {
                    pBuf[j++] = c; // Escaped literal 255
                    m_nTelnetState = TS_DATA;
                }
                else if (c == WILL || c == WONT || c == DO || c == DONT)
                {
                    m_nTelnetCmd = c;
                    m_nTelnetState = TS_CMD;
                }
                else if (c == SB)
                {
                    m_nSubnegLen = 0;
                    m_nTelnetState = TS_SB;
                }
                else
                {
                    // NOP, AYT, GA, EOR, ... - consume silently
                    m_nTelnetState = TS_DATA;
                }
                break;

            case TS_CMD:
                if (outlen < (int)sizeof(out) - 16)
                {
                    TelnetNegotiate(m_nTelnetCmd, c, out, &outlen);
                }
                m_nTelnetState = TS_DATA;
                break;

            case TS_SB:
                if (c == IAC)
                {
                    m_nTelnetState = TS_SB_IAC;
                }
                else if (m_nSubnegLen >= 0)
                {
                    if (m_nSubnegLen < (int)sizeof(m_aSubneg))
                    {
                        m_aSubneg[m_nSubnegLen++] = c;
                    }
                    else
                    {
                        m_nSubnegLen = -1; // Overlong: eat until SE, discard
                    }
                }
                break;

            case TS_SB_IAC:
                if (c == IAC)
                {
                    if (m_nSubnegLen >= 0 && m_nSubnegLen < (int)sizeof(m_aSubneg))
                    {
                        m_aSubneg[m_nSubnegLen++] = c; // Escaped 255 in payload
                    }
                    m_nTelnetState = TS_SB;
                }
                else if (c == SE)
                {
                    if (m_nSubnegLen > 0 && outlen < (int)sizeof(out) - 16)
                    {
                        TelnetSubneg(out, &outlen);
                    }
                    m_nTelnetState = TS_DATA;
                }
                else
                {
                    // Malformed subnegotiation - drop it, reprocess the
                    // byte as if we had just seen a lone IAC
                    m_nTelnetState = TS_IAC;
                    i--;
                }
                break;
        }
    }

    *pnLen = j;

    if (outlen > 0)
    {
        // Raw socket path (qTX/PushWrite) - negotiation bytes must never
        // pass through the HTML/color processing of SendCon()
        Write(out, outlen);
    }
}

// Our opening offer, sent once per TCP connection right after accept():
// please tell us your window size and terminal type, and we can terminate
// prompts with EOR. RFC-compliant clients that support none of it simply
// refuse; clients predating negotiation ignore unknown IAC sequences.
void cConHook::SendTelnetInitialNegotiation()
{
    ubit8 buf[16];
    int len = 0;

    telnet_reply(buf, &len, DO, TELOPT_NAWS);
    telnet_reply(buf, &len, DO, TELOPT_TTYPE);
    telnet_reply(buf, &len, WILL, TELOPT_EOR);
    telnet_reply(buf, &len, WILL, TELOPT_GMCP);

    Write(buf, len);
}

// JSON string escaping for GMCP payloads: quotes, backslash and control
// chars; UTF-8 passes through unchanged
static void json_escape(const char *src, char *dst, int cap)
{
    int o = 0;

    for (const ubit8 *s = (const ubit8 *)src; *s && o < cap - 7; s++)
    {
        if (*s == '"' || *s == '\\')
        {
            dst[o++] = '\\';
            dst[o++] = *s;
        }
        else if (*s < ' ')
        {
            o += sprintf(dst + o, "\\u%04x", *s);
        }
        else
        {
            dst[o++] = *s;
        }
    }
    dst[o] = 0;
}

// Emit one GMCP package: IAC SB GMCP "<package> <json>" IAC SE.
// Sent via the raw Write() path (qTX/PushWrite) - GMCP frames must never
// enter StripHTML's output buffer, because IndentText() would word-wrap
// \n\r sequences into the middle of the JSON. Only reachable on the
// telnet path (m_bGmcpOk can only be set there).
void cConHook::GmcpSend(const char *package, const char *json)
{
    ubit8 frame[1024];
    char payload[900];
    int o = 0;

    frame[o++] = IAC;
    frame[o++] = SB;
    frame[o++] = TELOPT_GMCP;

    snprintf(payload, sizeof(payload), "%s %s", package, json);

    for (const ubit8 *s = (const ubit8 *)payload; *s && o < (int)sizeof(frame) - 4; s++)
    {
        if (*s == IAC)
        {
            frame[o++] = IAC; // Escape 0xFF as IAC IAC per RFC 854
        }
        frame[o++] = *s;
    }

    frame[o++] = IAC;
    frame[o++] = SE;

    Write(frame, o);
}

// The prompt div's bars attribute ("hp/maxhp ep/maxep mp/maxmp", see
// prompt_data() in update.zon) becomes GMCP Char.Vitals
void cConHook::GmcpVitals(const char *bars)
{
    int hp = 0;
    int maxhp = 0;
    int ep = 0;
    int maxep = 0;
    int mana = 0;
    int maxmana = 0;
    char json[192];

    if (strcmp(bars, m_aLastBars) == 0)
    {
        return; // Prompts fire constantly; only send changed vitals
    }

    if (sscanf(bars, "%d/%d %d/%d %d/%d", &hp, &maxhp, &ep, &maxep, &mana, &maxmana) != 6)
    {
        return;
    }

    strncpy(m_aLastBars, bars, sizeof(m_aLastBars) - 1);
    m_aLastBars[sizeof(m_aLastBars) - 1] = 0;

    snprintf(json,
             sizeof(json),
             "{\"hp\":%d,\"maxhp\":%d,\"ep\":%d,\"maxep\":%d,\"mana\":%d,\"maxmana\":%d}",
             hp,
             maxhp,
             ep,
             maxep,
             mana,
             maxmana);

    GmcpSend("Char.Vitals", json);
}

// The room header <h1 class='room_title' zone= exits= map= roomid=>Title</h1>
// becomes GMCP Room.Info. 'aTag' is the full h1 tag, 'roomid' its roomid
// value ("nameidx@zoneidx"), 'text' points just past the tag - the title is
// the element text, read by peeking ahead to the next tag WITHOUT consuming
// (StripHTML continues processing it as visible output afterwards).
void cConHook::GmcpRoomInfo(const char *aTag, const char *roomid, const char *text)
{
    char zone[128] = "";
    char exits[128] = "";
    char map[64] = "";
    char title[256];
    char etitle[512];
    char exjson[256];
    char json[1024];
    int o = 0;

    getHTMLValue("zone", aTag, zone, sizeof(zone) - 1);
    getHTMLValue("exits", aTag, exits, sizeof(exits) - 1);
    getHTMLValue("map", aTag, map, sizeof(map) - 1);

    for (const char *q = text; *q && *q != '<' && o < (int)sizeof(title) - 1; q++)
    {
        title[o++] = *q;
    }
    title[o] = 0;

    // Immortals get a " [name@zone]" suffix appended to the title text -
    // strip it, the id is already a proper field
    char *br = strstr(title, " [");
    if (br && o > 0 && title[o - 1] == ']')
    {
        *br = 0;
    }
    strip_trailing_blanks(title);

    // Mudlet mappers key rooms on an integer: FNV-1a of the symbolic id,
    // masked positive
    ubit32 num = 2166136261u;
    for (const ubit8 *s = (const ubit8 *)roomid; *s; s++)
    {
        num = (num ^ *s) * 16777619u;
    }
    num &= 0x7fffffff;

    // exits "n e s " -> ["n","e","s"] (may be empty: dark rooms)
    o = 0;
    exjson[o++] = '[';
    for (const char *s = exits; *s;)
    {
        while (*s == ' ')
        {
            s++;
        }
        if (*s == 0)
        {
            break;
        }
        if (o > 1)
        {
            exjson[o++] = ',';
        }
        exjson[o++] = '"';
        while (*s && *s != ' ' && o < (int)sizeof(exjson) - 4)
        {
            exjson[o++] = *s++;
        }
        exjson[o++] = '"';
    }
    exjson[o++] = ']';
    exjson[o] = 0;

    json_escape(title, etitle, sizeof(etitle));

    int x = 0;
    int y = 0;
    if (sscanf(map, "%d,%d", &x, &y) == 2)
    {
        snprintf(json,
                 sizeof(json),
                 "{\"num\":%u,\"id\":\"%s\",\"name\":\"%s\",\"area\":\"%s\",\"exits\":%s,\"coords\":[%d,%d]}",
                 num,
                 roomid,
                 etitle,
                 zone,
                 exjson,
                 x,
                 y);
    }
    else
    {
        snprintf(json,
                 sizeof(json),
                 "{\"num\":%u,\"id\":\"%s\",\"name\":\"%s\",\"area\":\"%s\",\"exits\":%s}",
                 num,
                 roomid,
                 etitle,
                 zone,
                 exjson);
    }

    GmcpSend("Room.Info", json);
}

/* =================== END TELNET OPTION NEGOTIATION ================= */

/* Returns new point (NULL if done) */
void cConHook::ShowChunk()
{
    char buffer[160 * 60 + 600];
    char *scan = nullptr;
    char *point = nullptr;
    const int max_lines = m_sSetup.height;
    int lines = 0;

    scan = buffer;

    if (m_qPaged.IsEmpty())
    {
        return;
    }

    cQueueElem *qe = m_qPaged.GetHead();

    point = (char *)qe->Data();

    for (;;)
    {
        if (!*point)
        {
            if (m_qPaged.IsEmpty())
            {
                break;
            }

            delete qe;
            qe = m_qPaged.GetHead();
            point = (char *)qe->Data();
            continue;
        }

        *scan = *point++;

        if (*scan == '\n')
        {
            lines++;
        }

        if (max_lines <= lines)
        {
            break;
        }

        scan++;

        if (scan >= buffer + sizeof(buffer) - 200)
        {
            sprintf(scan, "TRUNCATED!");
            break;
        }
    }

    *scan = '\0';

    /* Insert the rest of the un-paged stringback into buffer */
    if (!str_is_empty(point))
    {
        m_qPaged.Prepend(new cQueueElem(point));
    }

    if (qe)
    {
        delete qe;
    }

    m_nPromptMode = !m_qPaged.IsEmpty();

    if (m_nPromptMode == 1)
    {
        strcat(buffer, ParseOutput("<br/><div class='paged'> *** Return for more *** </div><br/>"));
    }

    assert(strlen(buffer) < sizeof(buffer));

    Write((ubit8 *)buffer, strlen(buffer));
}

// Nice if it could be in the constructor
void cConHook::SetWebsocket(WebsocketSender sender, websocketpp::connection_hdl hdl)
{
    m_fWebsSend = std::move(sender);
    m_pWebsHdl = hdl;
}

// cConHook Constructor
// Reset the per-session state of a connection to the same defaults a brand
// new connection gets. Used by the constructor, and by test_mud_up() when
// the MUD comes back after a reboot: the dead session's terminal setup
// (pushed via MULTI_SETUP_CHAR), prompt bookkeeping and partial buffers must
// not leak into the new session (issue #395).
// Deliberately NOT touched here: the one-shot client detection state
// (m_nSequenceCompare, m_nFirst), the serial line id (m_nLine, re-sent to
// the MUD on reconnect), m_nId, m_aHost, the websocket fields, the
// g_connection_list linkage, and the telnet negotiation state (parser
// state, NAWS/TTYPE/EOR results) - negotiated capabilities belong to the
// TCP connection, not the MUD session.
void cConHook::ResetSessionState()
{
    m_bGobble = false;
    m_bColorCreate = false;
    m_bColorChange = false;
    m_bColorDisp = false;
    m_bColorRemove = false;
    m_bColorDelete = false;
    m_bColorInsert = false;
    m_nEscapeCode = 0;
    m_aOutput[0] = 0;
    m_aInputBuf[0] = 0;

    m_nPromptMode = 0;
    m_nPromptLen = 0;

    m_sSetup.echo = g_mplex_arg.g_bModeEcho;
    m_sSetup.redraw = g_mplex_arg.g_bModeRedraw;
    m_sSetup.telnet = g_mplex_arg.g_bModeTelnet;
    m_sSetup.websockets = g_mplex_arg.bWebSockets;

    if (g_mplex_arg.g_bModeANSI)
    {
        m_sSetup.emulation = TERM_ANSI;
    }
    else
    {
        m_sSetup.emulation = TERM_TTY;
    }

    m_sSetup.height = 15;
    m_sSetup.width = 80;
    m_sSetup.colour_convert = 0;
    m_nBgColor = CONTROL_BG_BLACK_CHAR;

    // A NAWS-negotiated window size is the client's real geometry - it
    // outranks the 80x15 defaults, across MUD reboots too. Guarded
    // separately: RFC 1073 allows either dimension to be 0 (unknown).
    if (m_nNawsWidth)
    {
        m_sSetup.width = m_nNawsWidth;
    }
    if (m_nNawsHeight)
    {
        m_sSetup.height = m_nNawsHeight;
    }
}

cConHook::cConHook()
{
    ResetSessionState();

    m_nState = 0;

    m_nSequenceCompare = 0;
    m_nId = 0;
    m_nFirst = 0;
    m_nLine = 255;

    // m_pWebsHdl = 0;
    m_fWebsSend = nullptr;

    m_pNext = g_connection_list;
    g_connection_list = this;

    m_nState = 0;
    m_pFptr = dumbMenuSelect;
    m_nPromptMode = 0;
    strcpy(m_aHost, "");

    if (g_mplex_arg.bWebSockets)
    {
        return; // Hack for making websockets work
    }

    int fd = 0;
#ifdef _WINDOWS
    int j;
    int size;
#else
    unsigned int j = 0;
    unsigned int size = 0;
#endif
    sockaddr_in conaddr;
    sockaddr_in sock;
    char hostname[49];
    int x = 0;

    assert(g_MotherHook.IsHooked());

    j = sizeof(conaddr);
    // slog(LOG_OFF, 0, "accept() before.");
    fd = accept(g_MotherHook.tfd(), (sockaddr *)&conaddr, &j);
    // slog(LOG_OFF, 0, "accept() after.");
    Assert(fd >= 0, "ACCEPT");

#ifdef LINUX
    x = fcntl(fd, F_SETFL, FNDELAY);
    Assert(x != -1, "Non blocking set error.");
#endif

#ifdef _WINDOWS
    unsigned long val = 1;
    x = ioctlsocket(fd, FIONBIO, &val);
#endif
    /* We *want* Nagle to work on the individual socket connections
       to the outside world, but not between the servers & mplex'ers
       int i;
       j = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &i, sizeof(i));
       if (j == -1)
       error(HERE, "No Setsockopt()"); */

    size = sizeof(sock);
    if (getpeername(fd, (sockaddr *)&sock, &size) == -1)
    {
        slog(LOG_OFF, 0, "getpeername: socket %d error no %d.", fd, errno);
        // MS2020 strncpy (hostname,"NO GETPEERNAME",strlen("NO GETPEERNAME")+1);
        strcpy(hostname, "NO GETPEERNAME");
    }
    else
    {
        /*
           This occasionally causes freezing (for all players) of a duration
           from ~1 - 60 seconds. Too bad... if I get threads I will put it back
           in, I hate having only the numeric address. Quinn suggested an
           alarm of a few seconds, I'll try that out.
         */

        /*alarm(1); */

        /*
           if ((from = gethostbyaddr((char*) &sock.sin_addr,
           sizeof(sock.sin_addr),
           AF_INET)) == NULL)
           {
           slog(LOG_ALL, 0, "No symbolic INET address, h_errno %d.", h_errno);
           addr = (unsigned char *) &sock.sin_addr.s_addr;
        strncpy (hostname,inet_ntoa(sock.sin_addr),49);
           }
           else
           {
           m_aHostname = (char *) from->h_name;
           }
         */
        /* alarm(0); unschedule alarm */

        /* The fix */
        //      addr = (unsigned char *) &sock.sin_addr.s_addr;
        strncpy(hostname, inet_ntoa(sock.sin_addr), sizeof(hostname) - 1);
    }

    strncpy(m_aHost, hostname, sizeof(m_aHost) - 1);
    *(m_aHost + sizeof(m_aHost) - 1) = '\0';

    if (this->tfd() != -1)
    {
        slog(LOG_ALL, 0, "cConHook() called with a non -1 fd.");
    }

    g_CaptainHook.Hook(fd, this);

    if (SSL_CTX *ctx = tls_ctx_for_accept())
    {
        // telnets: the select loop drives the handshake (TlsHandshake());
        // everything written meanwhile is queued until it completes.
        m_pSSL = SSL_new(ctx);

        if (m_pSSL == nullptr)
        {
            slog(LOG_ALL, 0, "TLS: SSL_new for %s failed (%s), dropping the connection", m_aHost, tls_errors(SSL_ERROR_SSL).c_str());
            Unhook();
        }
        else
        {
            SSL_set_fd(m_pSSL, fd);
            SSL_set_accept_state(m_pSSL);
        }
    }

    if (m_sSetup.telnet && IsHooked())
    {
        SendTelnetInitialNegotiation(); // on telnets: the first bytes the client gets once its handshake is done
    }

    g_nConnectionsLeft--;
    m_bTakesConnection = true;
}

// cConHook Destructor
cConHook::~cConHook()
{
    TlsFree();

    if (this == g_connection_list)
    {
        g_connection_list = this->m_pNext;
    }
    else
    {
        cConHook *tmp = nullptr;

        for (tmp = g_connection_list; tmp; tmp = tmp->m_pNext)
        {
            if (tmp->m_pNext == this)
            {
                break;
            }
        }
        tmp->m_pNext = this->m_pNext;
    }

    if (m_bTakesConnection)
    {
        g_nConnectionsLeft++;
    }
}

} // namespace mplex
