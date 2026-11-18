#include "ClientConnector.h"
#include "MUDConnector.h"
#include "hook.h"
#include "mplex.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <utility>
#include <vector>

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * MUDConnector_Suite
 *
 * mplex keeps one connection to the server (g_MudHook), which Control() makes on
 * the main thread. After a reboot the websocket mplex connected twice: Control()
 * and the websocket thread (a player typing while the server was down) both called
 * test_mud_up(). The second connection was hooked over the first, whose fd stayed
 * in select() with nobody reading it: mplex spun at 100% CPU, and the two threads
 * working on g_MudHook's queues at once ended in cQueue::Cut()'s assertion,
 * dropping every web player (code 1006 in the browser).
 */
namespace
{
// A listening socket standing in for the server, on a free loopback port
struct FakeMud
{
    FakeMud()
    {
        listener = socket(AF_INET, SOCK_STREAM, 0);
        BOOST_REQUIRE(listener != -1);
        sockaddr_in a{};
        a.sin_family = AF_INET;
        a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        socklen_t n = sizeof(a);
        if (bind(listener, (sockaddr *)&a, sizeof(a)) != 0 || listen(listener, 8) != 0 || getsockname(listener, (sockaddr *)&a, &n) != 0)
        {
            close(listener);
            BOOST_FAIL("no loopback listener");
        }
        fcntl(listener, F_SETFL, O_NONBLOCK);
        // Only now, so a failed setup leaves the globals alone
        port = std::exchange(mplex::g_mplex_arg.nMudPort, ntohs(a.sin_port));
        address = std::exchange(mplex::g_mplex_arg.pAddress, const_cast<char *>("127.0.0.1"));
        webSockets = std::exchange(mplex::g_mplex_arg.bWebSockets, TRUE); // cConHook() needs no socket
    }

    ~FakeMud()
    {
        mplex::g_MudHook.Unhook();
        for (int fd : accepted)
        {
            close(fd);
        }
        close(listener);
        mplex::g_mplex_arg.nMudPort = port;
        mplex::g_mplex_arg.pAddress = address;
        mplex::g_mplex_arg.bWebSockets = webSockets;
    }

    // Accepts what came in; returns how many connections there have been
    int Connections()
    {
        int fd;
        while ((fd = accept(listener, nullptr, nullptr)) != -1)
        {
            accepted.push_back(fd);
        }
        return accepted.size();
    }

    int listener = -1;
    int port = 0;
    char *address = nullptr;
    int webSockets = 0;
    std::vector<int> accepted;
};
} // namespace

BOOST_FIXTURE_TEST_SUITE(MUDConnector_Suite, FakeMud)

BOOST_AUTO_TEST_CASE(a_second_test_mud_up_keeps_the_one_connection)
{
    mplex::test_mud_up();
    BOOST_REQUIRE(mplex::g_MudHook.IsHooked());
    int fd = mplex::g_MudHook.tfd();

    mplex::test_mud_up();
    BOOST_TEST(Connections() == 1);
    BOOST_TEST(mplex::g_MudHook.tfd() == fd);
}

BOOST_AUTO_TEST_CASE(input_while_the_server_is_down_does_not_connect)
{
    // On the websocket thread this ran beside Control(): only Control() connects
    mplex::cConHook web;
    web.MenuSelect("");
    web.MudDown("look");
    BOOST_TEST(!mplex::g_MudHook.IsHooked());
    BOOST_TEST(Connections() == 0);
}

BOOST_AUTO_TEST_SUITE_END()
