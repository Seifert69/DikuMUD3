#include "ClientConnector.h"
#include "mplex.h"
#include "slog.h"
#include "textutil.h"

#include <websocketpp/config/asio.hpp>
#include <websocketpp/server.hpp>

#include <map>
#include <mutex>

typedef websocketpp::server<websocketpp::config::asio_tls> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef websocketpp::config::asio::message_type::ptr message_ptr;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;

namespace mplex
{

// Monitor owning the websocket connection map: the mutex is private, every
// method locks internally, and no method calls into cConHook code while
// holding the lock. That structure is what prevents the two deadlocks this
// file used to have - holding the map lock while entering hook code that
// re-locks it (self-deadlock), and holding it while acquiring
// cConHook::m_mtx (ABBA with cConHook::Close()).
//
// The monitor protects map MEMBERSHIP only; the cConHook objects themselves
// are guarded by their own m_mtx. NOTE on lifetime: hooks ARE deleted - the
// main thread's ClearUnhooked() sweep frees any hook that is no longer
// hooked (for websocket hooks: after Close() nulls m_pWebsServer). A raw
// pointer returned by take()/find_or_insert() therefore has a narrow window
// where a concurrent main-thread Close() + sweep could free it while the
// websocket thread is still calling into it. Closing that window for real
// needs shared ownership (shared_ptr in map + connection list) or handing
// message processing off to the main thread.
class cConnectionMap
{
public:
    // Look up hdl; when absent, run factory() to create the hook and insert
    // it atomically (a separate find + insert would race against removal
    // from the main thread).
    // NOTE: factory runs UNDER the internal lock. It must never call back
    // into this map nor lock any cConHook::m_mtx. Construction/setup code
    // only.
    template<typename Factory>
    cConHook *find_or_insert(websocketpp::connection_hdl hdl, Factory factory)
    {
        std::lock_guard<std::mutex> lock(m_mtx);

        auto it = m_map.find(hdl);
        if (it != m_map.end())
        {
            return it->second;
        }

        cConHook *con = factory();
        m_map[hdl] = con;
        slog(LOG_OFF, 0, "connection map: created new connection for hdl %p, map size: %zu", hdl.lock().get(), m_map.size());
        return con;
    }

    // Find and remove hdl in one atomic step, returning the hook (nullptr
    // if absent). The caller can then Close() it with no lock held.
    cConHook *take(websocketpp::connection_hdl hdl)
    {
        std::lock_guard<std::mutex> lock(m_mtx);

        auto it = m_map.find(hdl);
        if (it == m_map.end())
        {
            slog(LOG_OFF, 0, "connection map: no entry for hdl %p, map size: %zu", hdl.lock().get(), m_map.size());
            return nullptr;
        }

        cConHook *con = it->second;
        m_map.erase(it);
        return con;
    }

    // Remove any entry pointing at con (reached via remove_gmap() from
    // cConHook::Close(), which must not hold m_mtx of the hook - see the
    // lock-order comment there).
    void erase_value(cConHook *con)
    {
        std::lock_guard<std::mutex> lock(m_mtx);

        for (auto it = m_map.begin(); it != m_map.end(); it++)
        {
            if (it->second == con)
            {
                slog(LOG_OFF, 0, "connection map: located con class, removed.");
                m_map.erase(it);
                return;
            }
        }
    }

private:
    std::mutex m_mtx; // Only this class can ever hold it
    std::map<websocketpp::connection_hdl, cConHook *, std::owner_less<websocketpp::connection_hdl>> m_map;
};

// File-local by design: the only entry point for code outside this file is
// remove_gmap() below.
static cConnectionMap g_connections;

void remove_gmap(cConHook *con)
{
    g_connections.erase_value(con);
}

void on_close(websocketpp::connection_hdl hdl)
{
    slog(LOG_OFF, 0, "on_close called for hdl %p", hdl.lock().get());

    // take() removes the entry and releases the map lock before we Close()
    cConHook *con = g_connections.take(hdl);

    if (con)
    {
        con->Close(TRUE);
    }
}

std::string get_password() {
    return "test";
}

context_ptr on_tls_init(websocketpp::connection_hdl hdl) {
    namespace asio = websocketpp::lib::asio;

    slog(LOG_OFF, 0, "on_tls_init called with hdl: %p", hdl.lock().get());

    context_ptr ctx = websocketpp::lib::make_shared<asio::ssl::context>(asio::ssl::context::sslv23);

    try {
        ctx->set_options(asio::ssl::context::default_workarounds |
                         asio::ssl::context::no_sslv2 |
                         asio::ssl::context::no_sslv3 |
                         asio::ssl::context::single_dh_use);
        ctx->set_password_callback(bind(&get_password));
        ctx->use_certificate_chain_file("server.pem");
        ctx->use_private_key_file("server.pem", asio::ssl::context::pem);
        ctx->use_tmp_dh_file("dh.pem");
        
        slog(LOG_OFF, 0, "TLS context initialized successfully");
    } catch (std::exception& e) {
        slog(LOG_OFF, 0, "TLS init exception: %s", e.what());
        slog(LOG_OFF, 0, "TLS init failed - connection may be unstable");
    } catch (...) {
        slog(LOG_OFF, 0, "TLS init unknown exception");
        slog(LOG_OFF, 0, "TLS init failed - connection may be unstable");
    }
    return ctx;
}

// send message back to websocket client: 1 is message sent, 0 if failure
int ws_send_message(wsserver_tls *s, websocketpp::connection_hdl hdl, const char *txt)
{
    std::string mystr(txt);

    str_correct_utf8(mystr);

    try
    {
        s->send(hdl, mystr.c_str(), mystr.length(), websocketpp::frame::opcode::text);
        return 1;
    }
    catch (websocketpp::exception const &e)
    {
        slog(LOG_OFF, 0, "Send failed for hdl %p: %s", hdl.lock().get(), e.what());
        slog(LOG_OFF, 0, "Send failed message: %s", txt);
        return 0;
    }
    catch (...)
    {
        slog(LOG_OFF, 0, "Send failed with unknown exception for hdl %p", hdl.lock().get());
        slog(LOG_OFF, 0, "Send failed message: %s", txt);
        return 0;
    }
}

// Define a callback to handle incoming messages
void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg)
{
    // The monitor holds its lock only for the lookup/insert (plus the
    // factory below for new connections) and releases it before we call
    // into the hook: the input processing can reach cConHook::Close()
    // (e.g. on a failed write), which re-enters the map via remove_gmap().
    cConHook *con = g_connections.find_or_insert(hdl,
                                                 [s, hdl]()
                                                 {
                                                     cConHook *con = new cConHook();
                                                     con->SetWebsocket(s, hdl);

                                                     // it's a new connection - Get the IP address
                                                     const auto theip = s->get_con_from_hdl(hdl);
                                                     boost::asio::ip::address theadr = theip->get_raw_socket().remote_endpoint().address();
                                                     std::string ip_as_string{theadr.to_string()};
                                                     if (theadr.is_v6())
                                                     {
                                                         auto v6 = boost::asio::ip::make_address_v6(theadr.to_string());
                                                         // Lets hope it is a ipv4 mapped to ipv6 address space
                                                         if (v6.is_v4_mapped())
                                                         {
                                                             auto v4 = boost::asio::ip::make_address_v4(boost::asio::ip::v4_mapped_t::v4_mapped, v6);
                                                             ip_as_string = v4.to_string();
                                                         }
                                                         else
                                                         {
                                                             ip_as_string = boost::asio::ip::address_v4::any().to_string();
                                                         }
                                                     }
                                                     strncpy(con->m_aHost, ip_as_string.c_str(), sizeof(con->m_aHost) - 1);
                                                     *(con->m_aHost + sizeof(con->m_aHost) - 1) = '\0';
                                                     slog(LOG_OFF, 0, "IP connection from: %s", con->m_aHost);
                                                     return con;
                                                 });

    assert(con);

    con->m_pFptr(con, msg->get_payload().c_str());
}

void runechoserver()
{
    // Create a server endpoint
    server echo_server;

    try
    {
        // Set logging settings
        echo_server.set_access_channels(websocketpp::log::alevel::none);
        echo_server.clear_access_channels(websocketpp::log::alevel::none);

        // Initialize Asio
        echo_server.init_asio();

        // Register our message handler
        echo_server.set_close_handler(bind(&on_close, _1));
        echo_server.set_message_handler(bind(&on_message, &echo_server, _1, _2));
        echo_server.set_tls_init_handler(bind(&on_tls_init, _1));

        // Listen on port
        echo_server.set_reuse_addr(true);
        echo_server.listen(g_mplex_arg.nMotherPort);

        // Start the server accept loop
        echo_server.start_accept();

        // Start the ASIO io_service run loop
        echo_server.run();
    }
    catch (websocketpp::exception const &e)
    {
        slog(LOG_OFF, 0, "TLS Exception: %s.", e.what());
        exit(42);
    }
    catch (...)
    {
        slog(LOG_OFF, 0, "TLS Exception other");
        exit(42);
    }
}

} // namespace mplex