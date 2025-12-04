#include "ClientConnector.h"
#include "mplex.h"
#include "slog.h"
#include "textutil.h"

#include <websocketpp/config/asio.hpp>
#include <websocketpp/server.hpp>

#include <map>
#include <mutex>
#include <thread>

typedef websocketpp::server<websocketpp::config::asio_tls> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef websocketpp::config::asio::message_type::ptr message_ptr;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;

namespace mplex
{

// Global map for connection handlers
std::map<websocketpp::connection_hdl, cConHook *, std::owner_less<websocketpp::connection_hdl>> g_cMapHandler;

// Mutex to protect access to the global map
std::mutex g_cMapHandler_mutex;

// Global WebSocket thread for proper management
std::thread *g_websocket_thread = nullptr;
server *g_echo_server = nullptr;  // Global server instance for stopping

void remove_gmap(cConHook *con)
{
    std::lock_guard<std::mutex> lock(g_cMapHandler_mutex);
    std::map<websocketpp::connection_hdl, cConHook *, std::owner_less<websocketpp::connection_hdl>>::iterator it;

    for (it = g_cMapHandler.begin(); it != g_cMapHandler.end(); it++)
    {
        if (it->second == con)
        {
            slog(LOG_OFF, 0, "remove_gmap located con class, removed.");
            g_cMapHandler.erase(it);
            return;
        }
    }
}

void on_close(websocketpp::connection_hdl hdl)
{
    cConHook *con = nullptr;
    std::map<websocketpp::connection_hdl, cConHook *, std::owner_less<websocketpp::connection_hdl>>::iterator it;

    std::lock_guard<std::mutex> lock(g_cMapHandler_mutex);

    it = g_cMapHandler.find(hdl);

    if (it != g_cMapHandler.end())
    {
        con = it->second;
        g_cMapHandler.erase(it);
        con->Close(TRUE);
    }
    else
    {
        slog(LOG_OFF, 0, "on_close unable to locate class.");
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
        slog(LOG_OFF, 0, "Send failed: %s", e.what());
        return 0;
    }
}

// Define a callback to handle incoming messages
void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg)
{
    cConHook *con = nullptr;

    std::lock_guard<std::mutex> lock(g_cMapHandler_mutex);

    if (g_cMapHandler.find(hdl) == g_cMapHandler.end())
    {
        con = new cConHook();
        con->SetWebsocket(s, hdl);
        g_cMapHandler[hdl] = con;

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
    }
    else
    {
        con = g_cMapHandler[hdl];
    }

    assert(con);

    con->m_pFptr(con, msg->get_payload().c_str());
}

void runechoserver()
{
    // Create a server endpoint
    server echo_server;
    g_echo_server = &echo_server;  // Store global pointer for stopping

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
    }
    catch (...)
    {
        slog(LOG_OFF, 0, "TLS Exception other");
    }
    g_echo_server = nullptr;  // Clean up global pointer
}

void stop_websocket_server()
{
    if (g_echo_server)
    {
        slog(LOG_OFF, 0, "Stopping WebSocket server...");
        
        // Stop the server - this will interrupt echo_server.run()
        g_echo_server->stop();
        g_echo_server = nullptr;
        slog(LOG_OFF, 0, "Echo server stopped");
    }

    if (g_websocket_thread)
    {
        if (g_websocket_thread->joinable())
        {
            g_websocket_thread->join();
        }
        
        delete g_websocket_thread;
        g_websocket_thread = nullptr;
        slog(LOG_OFF, 0, "WebSocket server stopped");
    }
}

void start_websocket_server()
{
    if (g_websocket_thread)
    {
        slog(LOG_OFF, 0, "WebSocket server already running");
        return;
    }
    
    slog(LOG_OFF, 0, "Starting WebSocket server...");
    g_websocket_thread = new std::thread(runechoserver);
}

} // namespace mplex