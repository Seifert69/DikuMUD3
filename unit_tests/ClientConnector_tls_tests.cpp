#include "ClientConnector.h"
#include "MUDConnector.h"
#include "hook.h"
#include "mplex.h"
#include "translate.h"
#include "utility.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * ClientConnector_TLS_Suite
 *
 * The telnets transport (mplex -z). Each test accepts a cConHook from a real
 * listening socket on 127.0.0.1, puts a non-blocking OpenSSL client on the
 * other end, and drives the server side with g_CaptainHook.Wait(), as
 * mplex's main loop does. No mud is connected, so every hook stays at
 * m_nId == 0: the state in which Close() alone does nothing.
 */
namespace
{
// A self-signed certificate and its key in one PEM, the way mplex reads server.pem
struct SelfSigned
{
    std::string pem;

    explicit SelfSigned(long serial)
    {
        EVP_PKEY_CTX *pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
        BOOST_REQUIRE(pctx);
        BOOST_REQUIRE(EVP_PKEY_keygen_init(pctx) == 1);
        BOOST_REQUIRE(EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pctx, NID_X9_62_prime256v1) == 1);
        EVP_PKEY *key = nullptr;
        BOOST_REQUIRE(EVP_PKEY_keygen(pctx, &key) == 1);
        EVP_PKEY_CTX_free(pctx);

        X509 *x = X509_new();
        X509_set_version(x, 2);
        ASN1_INTEGER_set(X509_get_serialNumber(x), serial);
        X509_gmtime_adj(X509_getm_notBefore(x), -60);
        X509_gmtime_adj(X509_getm_notAfter(x), 3600);
        X509_set_pubkey(x, key);
        X509_NAME *name = X509_get_subject_name(x);
        X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (const unsigned char *)"mplex unit test", -1, -1, 0);
        X509_set_issuer_name(x, name);
        BOOST_REQUIRE(X509_sign(x, key, EVP_sha256()) > 0);

        BIO *bio = BIO_new(BIO_s_mem());
        PEM_write_bio_PrivateKey(bio, key, nullptr, nullptr, 0, nullptr, nullptr);
        PEM_write_bio_X509(bio, x);
        char *p = nullptr;
        long n = BIO_get_mem_data(bio, &p);
        pem.assign(p, n);

        BIO_free(bio);
        X509_free(x);
        EVP_PKEY_free(key);
    }
};

long PeerSerial(SSL *ssl)
{
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
    X509 *x = SSL_get1_peer_certificate(ssl);
#else
    X509 *x = SSL_get_peer_certificate(ssl);
#endif
    BOOST_REQUIRE(x);
    long serial = ASN1_INTEGER_get(X509_get_serialNumber(x));
    X509_free(x);
    return serial;
}

// What another connection's failure leaves on this thread's OpenSSL error queue
void LeaveAnErrorOnTheQueue()
{
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
    ERR_raise(ERR_LIB_SSL, SSL_R_PACKET_LENGTH_TOO_LONG);
#else
    ERR_put_error(ERR_LIB_SSL, 0, SSL_R_PACKET_LENGTH_TOO_LONG, __FILE__, __LINE__);
#endif
}

std::string g_lastCommand; // what the hook handed its state function

void RecordCommand(mplex::cConHook *, const char *cmd)
{
    g_lastCommand = cmd;
}

// A non-blocking TLS client on an already connected socket
struct TlsClient
{
    SSL_CTX *ctx;
    SSL *ssl;

    explicit TlsClient(int fd)
    {
        ctx = SSL_CTX_new(TLS_client_method());
        SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, nullptr);
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, fd);
        SSL_set_connect_state(ssl);
    }

    ~TlsClient()
    {
        SSL_free(ssl);
        SSL_CTX_free(ctx);
    }

    // One step; true when complete
    bool Handshake()
    {
        ERR_clear_error();
        int r = SSL_do_handshake(ssl);
        if (r == 1)
        {
            return true;
        }
        int e = SSL_get_error(ssl, r);
        BOOST_REQUIRE_MESSAGE(e == SSL_ERROR_WANT_READ || e == SSL_ERROR_WANT_WRITE, "client handshake failed: " << e);
        return false;
    }

    // Appends what is available; -1 once the server has closed
    int Read(std::string &out)
    {
        char buf[4096];
        ERR_clear_error();
        int r = SSL_read(ssl, buf, sizeof(buf));
        if (r > 0)
        {
            out.append(buf, r);
            return r;
        }
        int e = SSL_get_error(ssl, r);
        return (e == SSL_ERROR_WANT_READ || e == SSL_ERROR_WANT_WRITE) ? 0 : -1;
    }

    void Write(const std::string &s)
    {
        ERR_clear_error();
        BOOST_REQUIRE_EQUAL(SSL_write(ssl, s.data(), (int)s.size()), (int)s.size());
    }
};

struct TlsFixture
{
    std::string path; // the PEM file tls_init_telnet() reads
    int listenFd = -1;
    int port = 0;
    std::vector<mplex::cConHook *> hooks;
    std::vector<int> clients;
    int savedWebSockets;
    int savedTelnet;
    int savedConnectionsLeft;

    TlsFixture()
    {
        mplex::translate_init(); // the colour codes, as mplex's main() sets them up
        if (g_log_file_fd == nullptr)
        {
            g_log_file_fd = stderr; // slog() writes to it unconditionally
        }
        savedWebSockets = std::exchange(mplex::g_mplex_arg.bWebSockets, FALSE);
        savedTelnet = std::exchange(mplex::g_mplex_arg.g_bModeTelnet, TRUE);
        savedConnectionsLeft = mplex::g_nConnectionsLeft;

        char tmpl[] = "/tmp/mplex-tls-test-XXXXXX";
        int fd = mkstemp(tmpl);
        BOOST_REQUIRE(fd >= 0);
        close(fd);
        path = tmpl;
        WritePem(SelfSigned(1));

        listenFd = socket(AF_INET, SOCK_STREAM, 0);
        BOOST_REQUIRE(listenFd >= 0);
        sockaddr_in a{};
        a.sin_family = AF_INET;
        a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        BOOST_REQUIRE_EQUAL(bind(listenFd, (sockaddr *)&a, sizeof(a)), 0);
        BOOST_REQUIRE_EQUAL(listen(listenFd, 5), 0);
        socklen_t len = sizeof(a);
        BOOST_REQUIRE_EQUAL(getsockname(listenFd, (sockaddr *)&a, &len), 0);
        port = ntohs(a.sin_port);
        fcntl(listenFd, F_SETFL, O_NONBLOCK);
        mplex::g_MotherHook.Hook(listenFd); // cConHook() accepts from g_MotherHook.tfd()
    }

    ~TlsFixture()
    {
        for (auto *h : hooks)
        {
            h->Unhook(); // out of g_CaptainHook's table before it is gone
            delete h;
        }
        for (int c : clients)
        {
            if (c >= 0)
            {
                close(c);
            }
        }
        mplex::g_MotherHook.Hook(-1); // ours to close, not the hook's
        close(listenFd);
        mplex::tls_free_telnet();
        unlink(path.c_str());
        g_lastCommand.clear();
        mplex::g_mplex_arg.bWebSockets = savedWebSockets;
        mplex::g_mplex_arg.g_bModeTelnet = savedTelnet;
        mplex::g_nConnectionsLeft = savedConnectionsLeft;
    }

    void WritePem(const SelfSigned &c, int mtimeOffset = 0)
    {
        FILE *f = fopen(path.c_str(), "w");
        BOOST_REQUIRE(f);
        fwrite(c.pem.data(), 1, c.pem.size(), f);
        fclose(f);

        if (mtimeOffset)
        {
            // So the change shows even on a coarse clock
            struct stat st = {};
            BOOST_REQUIRE_EQUAL(stat(path.c_str(), &st), 0);
            timespec ts[2] = {st.st_atim, st.st_mtim};
            ts[1].tv_sec += mtimeOffset;
            BOOST_REQUIRE_EQUAL(utimensat(AT_FDCWD, path.c_str(), ts, 0), 0);
        }
    }

    bool Init() { return mplex::tls_init_telnet(path.c_str()); }

    // A connected, non-blocking client socket
    int Connect()
    {
        int c = socket(AF_INET, SOCK_STREAM, 0);
        BOOST_REQUIRE(c >= 0);
        sockaddr_in a{};
        a.sin_family = AF_INET;
        a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        a.sin_port = htons(port);
        BOOST_REQUIRE_EQUAL(connect(c, (sockaddr *)&a, sizeof(a)), 0);
        fcntl(c, F_SETFL, O_NONBLOCK);
        clients.push_back(c);
        return c;
    }

    void Forget(int c) // the test closed it itself
    {
        for (int &x : clients)
        {
            if (x == c)
            {
                x = -1;
            }
        }
    }

    // What cMotherHook::Input() does: accept, then write to the client
    // before anything has come back from it (there: "Requesting an ID")
    mplex::cConHook *Accept(const char *greeting = "Welcome<br/>")
    {
        auto *h = new mplex::cConHook();
        hooks.push_back(h);
        h->m_pFptr = RecordCommand; // not MenuSelect(): there is no mud to ask
        h->SendCon(greeting);
        return h;
    }

    // One pass of mplex's main loop: select() with a timeout, then Input()
    // and PushWrite() on whatever is ready. Returns what select() returned.
    int Loop(int ms = 10)
    {
        timeval tv{0, ms * 1000};
        return g_CaptainHook.Wait(&tv);
    }

    // Server passes and client steps, alternating, until step() is true
    template <class Step>
    bool Until(Step step, int passes = 200)
    {
        for (int i = 0; i < passes; i++)
        {
            Loop();
            if (step())
            {
                return true;
            }
        }
        return false;
    }

    // Handshake to completion and the server's queue flushed to the client
    void Connected(TlsClient &client, mplex::cConHook *h, std::string &got)
    {
        BOOST_REQUIRE(Until([&] { return client.Handshake(); }));
        BOOST_REQUIRE(Until([&] {
            client.Read(got);
            return got.find("Welcome") != std::string::npos;
        }));
        BOOST_REQUIRE(h->IsHooked());
    }
};
} // namespace

BOOST_AUTO_TEST_SUITE(ClientConnector_TLS_Suite)

BOOST_FIXTURE_TEST_CASE(a_missing_pem_file_is_a_failure_not_a_plain_port, TlsFixture)
{
    BOOST_TEST(!mplex::tls_init_telnet("/nonexistent/server.pem"));
    BOOST_TEST(mplex::g_tls_ctx == nullptr);
    BOOST_TEST(Init());
    BOOST_TEST(mplex::g_tls_ctx != nullptr);
}

BOOST_FIXTURE_TEST_CASE(a_silent_client_costs_nothing_while_its_handshake_is_pending, TlsFixture)
{
    BOOST_REQUIRE(Init());
    int c = Connect();
    auto *h = Accept(); // queues the telnet negotiation and the greeting

    // Nothing can happen until the client speaks: select() must sleep the
    // whole timeout, not return at once because the socket is writable
    auto t0 = std::chrono::steady_clock::now();
    int n = Loop(100);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0).count();
    BOOST_TEST(n == 0);
    BOOST_TEST(ms >= 90);
    BOOST_TEST(h->IsHooked());

    // and nothing went to the client in clear
    char buf[64];
    BOOST_TEST(recv(c, buf, sizeof(buf), MSG_DONTWAIT) == -1);
    BOOST_TEST(errno == EAGAIN);
}

BOOST_FIXTURE_TEST_CASE(a_plain_telnet_client_is_dropped_even_without_an_id, TlsFixture)
{
    BOOST_REQUIRE(Init());
    int c = Connect();
    auto *h = Accept();

    BOOST_REQUIRE_EQUAL(send(c, "hello\r\n", 7, 0), 7);
    BOOST_TEST(Until([&] { return !h->IsHooked(); }));

    // The client gets the alert and then EOF, and the socket is out of the
    // table: the loop has nothing left to do with it
    char buf[256];
    int r = -1;
    for (int i = 0; i < 50 && (r = recv(c, buf, sizeof(buf), MSG_DONTWAIT)) != 0; i++)
    {
        if (r < 0)
        {
            usleep(1000);
        }
    }
    BOOST_TEST(r == 0);
    BOOST_TEST(Loop(20) == 0);
}

BOOST_FIXTURE_TEST_CASE(negotiation_goes_first_then_what_was_queued_then_input_flows, TlsFixture)
{
    BOOST_REQUIRE(Init());
    int c = Connect();
    auto *h = Accept();
    TlsClient client(c);
    std::string got;
    Connected(client, h, got);

    BOOST_REQUIRE(got.size() >= 3);
    BOOST_TEST((unsigned char)got[0] == 255); // IAC
    BOOST_TEST((unsigned char)got[1] == 253); // DO
    BOOST_TEST((unsigned char)got[2] == 31);  // NAWS
    BOOST_TEST(got.find("Welcome") >= 3);

    client.Write("look\r\n");
    BOOST_TEST(Until([&] { return g_lastCommand == "look"; }));
    BOOST_TEST(h->IsHooked());
}

BOOST_FIXTURE_TEST_CASE(a_write_error_in_the_flush_after_the_handshake_is_not_a_crash, TlsFixture)
{
    BOOST_REQUIRE(Init());
    int c = Connect();
    auto *h = Accept();
    TlsClient client(c);

    // The client has sent its Finished; the server has not read it yet
    BOOST_REQUIRE(Until([&] { return client.Handshake(); }));

    // so the flush that follows the server's side of the handshake fails hard
    BOOST_REQUIRE_EQUAL(shutdown(h->tfd(), SHUT_WR), 0);
    Loop(20);

    BOOST_TEST(!h->IsHooked());
    BOOST_TEST(Loop(20) == 0);
}

BOOST_FIXTURE_TEST_CASE(a_client_that_resets_after_the_handshake_is_dropped, TlsFixture)
{
    BOOST_REQUIRE(Init());
    int c = Connect();
    auto *h = Accept();
    TlsClient client(c);
    std::string got;
    Connected(client, h, got);

    linger l{1, 0}; // RST: no FIN, no close_notify
    setsockopt(c, SOL_SOCKET, SO_LINGER, &l, sizeof(l));
    close(c);
    Forget(c);

    BOOST_TEST(Until([&] { return !h->IsHooked(); }));
    BOOST_TEST(Loop(20) == 0);
}

BOOST_FIXTURE_TEST_CASE(a_stale_entry_on_the_error_queue_does_not_cost_a_connection, TlsFixture)
{
    BOOST_REQUIRE(Init());
    int c = Connect();
    auto *h = Accept();
    TlsClient client(c);
    std::string got;
    Connected(client, h, got);

    LeaveAnErrorOnTheQueue();
    h->Input(SELECT_READ); // nothing to read: must come back as "nothing yet", not as a TLS error
    BOOST_TEST(h->IsHooked());

    LeaveAnErrorOnTheQueue();
    h->SendCon("still here<br/>");
    BOOST_TEST(h->IsHooked());
    BOOST_TEST(Until([&] {
        client.Read(got);
        return got.find("still here") != std::string::npos;
    }));
}

BOOST_FIXTURE_TEST_CASE(a_renewed_certificate_is_used_for_the_next_connection, TlsFixture)
{
    BOOST_REQUIRE(Init());
    int c1 = Connect();
    auto *h1 = Accept();
    TlsClient client1(c1);
    std::string got1;
    Connected(client1, h1, got1);
    BOOST_TEST(PeerSerial(client1.ssl) == 1);

    WritePem(SelfSigned(2), 2);

    int c2 = Connect();
    auto *h2 = Accept();
    TlsClient client2(c2);
    std::string got2;
    Connected(client2, h2, got2);
    BOOST_TEST(PeerSerial(client2.ssl) == 2);

    // and the first connection is unaffected
    h1->SendCon("still here<br/>");
    BOOST_TEST(Until([&] {
        client1.Read(got1);
        return got1.find("still here") != std::string::npos;
    }));
}

BOOST_FIXTURE_TEST_CASE(the_plain_port_behaves_as_before, TlsFixture)
{
    // No tls_init_telnet(): a plain hook, negotiation out at once, in clear
    int c = Connect();
    auto *h = Accept();
    Loop(20);

    char buf[256];
    int r = recv(c, buf, sizeof(buf), MSG_DONTWAIT);
    BOOST_REQUIRE(r >= 3);
    BOOST_TEST((unsigned char)buf[0] == 255);
    BOOST_TEST((unsigned char)buf[1] == 253);
    BOOST_TEST(std::string(buf, r).find("Welcome") != std::string::npos);
    BOOST_TEST(h->IsHooked());
}

BOOST_AUTO_TEST_SUITE_END()
