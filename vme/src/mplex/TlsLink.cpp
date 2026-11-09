#include "TlsLink.h"

#include <openssl/err.h>

#include <cerrno>
#include <cstring>

namespace mplex
{

namespace
{
// What OpenSSL queued about the last failure, oldest first; the queue is
// drained. When it is empty (SSL_ERROR_SYSCALL) errno is described instead,
// so a log line never gets a NULL for its %s.
std::string openssl_errors(int err, int ret)
{
    std::string s;
    unsigned long e = 0;

    while ((e = ERR_get_error()) != 0)
    {
        const char *reason = ERR_reason_error_string(e); // NULL for errors OpenSSL has no text for

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
            ERR_error_string_n(e, buf, sizeof(buf));
            s += buf;
        }
    }

    if (s.empty())
    {
        if (err == SSL_ERROR_SYSCALL)
        {
            s = ret == 0 ? "closed without close_notify" : (errno ? strerror(errno) : "I/O error");
        }
        else
        {
            s = "error " + std::to_string(err);
        }
    }

    return s;
}
} // namespace

cTlsLink::cTlsLink(SSL_CTX *ctx)
{
    ERR_clear_error();
    m_pSSL = SSL_new(ctx);

    if (m_pSSL == nullptr)
    {
        Fail("SSL_new", SSL_ERROR_SSL, -1);
        return;
    }

    m_pIn = BIO_new(BIO_s_mem());
    m_pOut = BIO_new(BIO_s_mem());

    if (m_pIn == nullptr || m_pOut == nullptr)
    {
        BIO_free(m_pIn);
        BIO_free(m_pOut);
        SSL_free(m_pSSL);
        m_pSSL = nullptr;
        Fail("BIO_new", SSL_ERROR_SSL, -1);
        return;
    }

    // An empty input BIO means "nothing yet", not end of stream: SSL_read()
    // and SSL_do_handshake() then report WANT_READ and are simply called
    // again after the next Feed().
    BIO_set_mem_eof_return(m_pIn, -1);
    SSL_set_bio(m_pSSL, m_pIn, m_pOut); // m_pSSL owns both from here on
    SSL_set_accept_state(m_pSSL);
}

cTlsLink::~cTlsLink()
{
    if (m_pSSL)
    {
        SSL_free(m_pSSL);
    }
}

bool cTlsLink::Done() const
{
    return m_pSSL != nullptr && SSL_is_init_finished(m_pSSL);
}

bool cTlsLink::Fail(const char *what, int err, int ret)
{
    m_bFailed = true;
    m_sError = std::string(what) + " failed: " + openssl_errors(err, ret);
    return false;
}

// One step of the handshake. True while it needs more from the peer and once
// it is complete; false when it failed. On completion the plaintext kept
// meanwhile is encrypted, so the peer gets it first.
bool cTlsLink::Step()
{
    if (Done())
    {
        return true;
    }

    ERR_clear_error();

    int r = SSL_do_handshake(m_pSSL);

    if (r == 1)
    {
        m_bJustCompleted = true;

        if (!m_sPending.empty())
        {
            std::string pending;
            pending.swap(m_sPending);
            Write(reinterpret_cast<const ubit8 *>(pending.data()), static_cast<int>(pending.size()));
        }

        return !m_bFailed;
    }

    int e = SSL_get_error(m_pSSL, r);

    if (e == SSL_ERROR_WANT_READ)
    {
        return true; // WANT_WRITE cannot happen: the output BIO never fills
    }

    return Fail("handshake", e, r);
}

bool cTlsLink::Feed(const ubit8 *data, int len)
{
    if (m_bFailed)
    {
        return false;
    }

    if (len > 0 && BIO_write(m_pIn, data, len) != len)
    {
        return Fail("buffering input", SSL_ERROR_SSL, -1);
    }

    return Step();
}

int cTlsLink::Read(ubit8 *buf, int count)
{
    if (m_bFailed || m_bPeerClosed)
    {
        return -1;
    }

    if (!Done())
    {
        return 0;
    }

    ERR_clear_error();

    int r = SSL_read(m_pSSL, buf, count);

    if (r > 0)
    {
        return r;
    }

    int e = SSL_get_error(m_pSSL, r);

    if (e == SSL_ERROR_WANT_READ)
    {
        return 0;
    }

    if (e == SSL_ERROR_ZERO_RETURN)
    {
        m_bPeerClosed = true;
        return -1;
    }

    Fail("read", e, r);
    return -1;
}

bool cTlsLink::HasInput() const
{
    if (m_pSSL == nullptr || m_bFailed || m_bPeerClosed || !Done())
    {
        return false;
    }

    return SSL_pending(m_pSSL) > 0 || BIO_ctrl_pending(m_pIn) > 0;
}

void cTlsLink::Write(const ubit8 *data, int len)
{
    if (m_bFailed || len <= 0)
    {
        return;
    }

    if (!Done())
    {
        m_sPending.append(reinterpret_cast<const char *>(data), len);
        return;
    }

    ERR_clear_error();

    // The output BIO takes everything, so this writes all of it (as records of
    // at most 16 KB) or fails.
    int r = SSL_write(m_pSSL, data, len);

    if (r <= 0)
    {
        Fail("write", SSL_get_error(m_pSSL, r), r);
    }
}

int cTlsLink::TakeOutput(ubit8 *buf, int cap)
{
    if (m_pOut == nullptr || BIO_ctrl_pending(m_pOut) == 0)
    {
        return 0;
    }

    int r = BIO_read(m_pOut, buf, cap);
    return r > 0 ? r : 0;
}

void cTlsLink::Shutdown()
{
    if (m_pSSL == nullptr || m_bFailed || !Done())
    {
        return; // OpenSSL says not to after a fatal error, and there is nothing to notify before the handshake
    }

    ERR_clear_error();
    SSL_shutdown(m_pSSL); // our close_notify, into the output
    ERR_clear_error();
}

bool cTlsLink::JustCompleted()
{
    bool b = m_bJustCompleted;
    m_bJustCompleted = false;
    return b;
}

const char *cTlsLink::Version() const
{
    return m_pSSL ? SSL_get_version(m_pSSL) : "none";
}

} // namespace mplex
