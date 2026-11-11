#include "hook.h"

#include <fcntl.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <unistd.h>

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * cCaptainHook_Suite
 *
 * g_CaptainHook's table is indexed by fd. It holds every fd select() can
 * watch (below FD_SETSIZE), and refuses the others instead of writing past
 * its end: with 256 entries, mplex's 256th fd (about 250 telnet clients)
 * aborted it.
 */
namespace
{
// A hook that counts the times Wait() found it readable
struct CountingHook : public cHook
{
    ~CountingHook() override { Unhook(); }

    void Unhook() override
    {
        if (IsHooked())
        {
            g_CaptainHook.Unhook(this); // the table first: it goes by fd
        }
        cHook::Unhook();
    }

    void Input(int nFlags) override
    {
        if (nFlags & SELECT_READ)
        {
            reads++;
        }
    }

    int reads = 0;
};

// One end of a socket pair moved to fd `at`; the other end, to write to
struct SocketAt
{
    explicit SocketAt(int at)
    {
        BOOST_REQUIRE(fcntl(at, F_GETFD) == -1); // free: dup2() would close what's there
        int sv[2];
        BOOST_REQUIRE_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
        BOOST_REQUIRE_EQUAL(dup2(sv[0], at), at);
        close(sv[0]);
        fcntl(at, F_SETFL, O_NONBLOCK);
        fd = at;
        peer = sv[1];
    }
    ~SocketAt() { close(peer); } // fd is the hook's: Unhook() closes it

    int fd;
    int peer;
};

// So that fd FD_SETSIZE - 1 can be opened where the soft limit is lower
void AllowFdSetSize()
{
    rlimit r{};
    BOOST_REQUIRE_EQUAL(getrlimit(RLIMIT_NOFILE, &r), 0);
    if (r.rlim_cur < FD_SETSIZE && r.rlim_max >= FD_SETSIZE)
    {
        r.rlim_cur = FD_SETSIZE;
        BOOST_REQUIRE_EQUAL(setrlimit(RLIMIT_NOFILE, &r), 0);
    }
    BOOST_REQUIRE(getdtablesize() >= FD_SETSIZE);
}
} // namespace

BOOST_AUTO_TEST_SUITE(cCaptainHook_Suite)

BOOST_AUTO_TEST_CASE(fds_from_256_up_to_FD_SETSIZE_are_hooked_and_served)
{
    AllowFdSetSize();
    SocketAt low(256); // the first fd past the old table
    SocketAt high(FD_SETSIZE - 1);
    CountingHook a;
    CountingHook b;

    BOOST_REQUIRE(g_CaptainHook.Hook(low.fd, &a));
    BOOST_REQUIRE(g_CaptainHook.Hook(high.fd, &b));

    BOOST_REQUIRE_EQUAL(write(low.peer, "x", 1), 1);
    BOOST_REQUIRE_EQUAL(write(high.peer, "y", 1), 1);
    timeval tv{1, 0};
    BOOST_TEST(g_CaptainHook.Wait(&tv) == 2);
    BOOST_TEST(a.reads == 1);
    BOOST_TEST(b.reads == 1);

    // and out again, leaving the table as it was
    a.Unhook();
    b.Unhook();
    tv = {0, 0};
    BOOST_TEST(g_CaptainHook.Wait(&tv) == 0);
}

BOOST_AUTO_TEST_CASE(an_fd_select_cannot_watch_is_refused)
{
    CountingHook h;

    BOOST_REQUIRE(fcntl(FD_SETSIZE, F_GETFD) == -1); // Hook() closes an fd it refuses
    BOOST_TEST(!g_CaptainHook.Hook(FD_SETSIZE, &h));
    BOOST_TEST(!g_CaptainHook.Hook(-1, &h));
    BOOST_TEST(!h.IsHooked());
}

BOOST_AUTO_TEST_SUITE_END()
