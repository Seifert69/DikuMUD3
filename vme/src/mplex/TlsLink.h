#pragma once

#include "essential.h"

#include <openssl/ssl.h>

#include <string>

namespace mplex
{

/**
 * TLS for one telnets connection (mplex -z): sits between the bytes a
 * cConHook reads from and writes to its socket and the plaintext the rest
 * of the hook works with.
 *
 * OpenSSL never touches the socket. It reads from and writes to memory
 * BIOs: ciphertext from the peer is handed in with Feed(), and whatever
 * OpenSSL wants sent (its handshake flight, application records, the reply
 * to a KeyUpdate, close_notify) is collected with TakeOutput() and queued on
 * the hook like any other output. So the select loop knows nothing about
 * TLS: "watch the socket for writing while output is queued" still holds,
 * nothing is polled while waiting on the peer, and OpenSSL never has a
 * half-written record of its own that a later call would have to finish.
 *
 * Plaintext written before the handshake completes (the telnet negotiation,
 * "Requesting an ID", the banner) is kept here and encrypted, in that order,
 * the moment it does.
 */
class cTlsLink
{
public:
    explicit cTlsLink(SSL_CTX *ctx);
    ~cTlsLink();
    cTlsLink(const cTlsLink &) = delete;
    cTlsLink &operator=(const cTlsLink &) = delete;

    /// False when construction failed (Error() says why); nothing else may be called then.
    bool Ok() const { return m_pSSL != nullptr; }

    /// Ciphertext from the peer: steps the handshake, makes the data decryptable.
    /// False on a fatal TLS error; Error() says what, and the connection is over.
    bool Feed(const ubit8 *data, int len);

    /// Decrypted bytes: > 0 and how many, 0 nothing available right now, -1 the
    /// connection is over (PeerClosed() after close_notify, else Failed()).
    int Read(ubit8 *buf, int count);

    /// More may be decrypted without another socket read: Read() again before select().
    bool HasInput() const;

    /// Plaintext to the peer: encrypted now, or kept until the handshake completes.
    /// Failed() afterwards means encrypting failed.
    void Write(const ubit8 *data, int len);

    /// Ciphertext OpenSSL has ready for the peer: how many bytes were copied, 0 when none.
    int TakeOutput(ubit8 *buf, int cap);

    /// Our close_notify, into the output, when the connection ends normally.
    void Shutdown();

    bool Done() const; ///< Handshake complete
    bool Failed() const { return m_bFailed; }
    bool PeerClosed() const { return m_bPeerClosed; }
    /// True once, after the Feed() that completed the handshake.
    bool JustCompleted();
    const char *Version() const;
    /// Where and why it failed: what OpenSSL queued, or the system error. Never empty.
    const std::string &Error() const { return m_sError; }

private:
    bool Step(); // the handshake, while it is not done
    bool Fail(const char *what, int err, int ret);

    SSL *m_pSSL = nullptr;
    BIO *m_pIn = nullptr;  // ciphertext from the peer; owned by m_pSSL
    BIO *m_pOut = nullptr; // ciphertext to the peer; owned by m_pSSL
    std::string m_sPending; // plaintext written before the handshake was done
    std::string m_sError;
    bool m_bFailed = false;
    bool m_bPeerClosed = false;
    bool m_bJustCompleted = false;
};

} // namespace mplex
