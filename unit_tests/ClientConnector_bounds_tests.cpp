#include "ClientConnector.h"
#include "MUDConnector.h"
#include "mplex.h"
#include "translate.h"

#include <cstring>
#include <string>
#include <utility>

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * ClientConnector_Bounds_Suite
 *
 * cConHook keeps StripHTML()'s telnet output inside its buffer, and counts
 * only telnet connections against g_nConnectionsLeft.
 */
namespace
{
struct WebsocketHook
{
    WebsocketHook()
    {
        mplex::translate_init(); // The colour codes, as mplex's main() sets them up
        hook.m_sSetup.emulation = TERM_ANSI;
    }
    ~WebsocketHook() { mplex::g_mplex_arg.bWebSockets = saveWebSockets; }

    // In websocket mode cConHook() needs no listening socket
    int saveWebSockets = std::exchange(mplex::g_mplex_arg.bWebSockets, TRUE); // Before hook
    mplex::cConHook hook;
};
} // namespace

BOOST_FIXTURE_TEST_SUITE(ClientConnector_Bounds_Suite, WebsocketHook)

// Before: each </div> (6 bytes) became about 22 bytes of colour codes, so a
// 32 KB message of them ran far past ParseOutput()'s 64 KB buffer
BOOST_AUTO_TEST_CASE(strip_html_stays_in_its_buffer)
{
    std::string html;
    while (html.size() < 3000)
    {
        html += "x</div>";
    }

    char out[2048];
    memset(out, 'Z', sizeof(out));
    hook.StripHTML(out, 1024, html.c_str());

    // Nothing written past the 1024 bytes it was given
    BOOST_TEST(std::string(out + 1024, sizeof(out) - 1024) == std::string(sizeof(out) - 1024, 'Z'));
}

BOOST_AUTO_TEST_CASE(strip_html_short_output_unchanged)
{
    char out[1024];
    hook.StripHTML(out, sizeof(out), "Hello<br/>there");
    BOOST_TEST(std::string(out) == "Hello\n\rthere");
}

// Before: websocket hooks never took a connection but gave one back, so the
// count only grew
BOOST_AUTO_TEST_CASE(websocket_hook_leaves_the_connection_count)
{
    int left = mplex::g_nConnectionsLeft;
    {
        mplex::cConHook other;
    }
    BOOST_TEST(mplex::g_nConnectionsLeft == left);
}

BOOST_AUTO_TEST_SUITE_END()
