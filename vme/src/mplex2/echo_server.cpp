#include <websocketpp/config/asio_no_tls.hpp>

#include <websocketpp/server.hpp>

#include <iostream>

#include "mplex.h"
#include "ClientConnector.h"

//typedef websocketpp::server<websocketpp::config::asio> wsserver;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef wsserver::message_ptr message_ptr;

//std::map<std::owner_less<websocketpp::connection_hdl>, void *> g_cMapHandler;
std::map<websocketpp::connection_hdl, cConHook *, std::owner_less<websocketpp::connection_hdl> > g_cMapHandler;

void remove_gmap(class cConHook *con)
{
    std::map<websocketpp::connection_hdl, cConHook *, std::owner_less<websocketpp::connection_hdl> >::iterator it;

    for (it = g_cMapHandler.begin(); it != g_cMapHandler.end(); it++)
        if (it->second == con)
        {
            slog(LOG_OFF, 0, "remove_gmap located con class, removed.");
            g_cMapHandler.erase(it);
            return;
        }

}

void on_close(websocketpp::connection_hdl hdl)
{
    class cConHook *con = 0;
    std::map<websocketpp::connection_hdl, cConHook *, std::owner_less<websocketpp::connection_hdl> >::iterator it;

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


// This function removes any invalid characters not permitted on websockets
void cleanstring(std::string &data) {
    std::string buffer;
    buffer.reserve(data.size()*1.1);
    int m_nEscapeCode = 0;

    for(size_t pos = 0; pos < data.size(); ++pos)
    {
        if ((data[pos] & 0x80) == 0) // lead bit is zero, must be a single ascii
        {
            // I can soon remove this.
            if (data[pos] == '\x1B')
            {
                m_nEscapeCode = 1;
                continue;
            }

            if (m_nEscapeCode > 0)
            {
                if (m_nEscapeCode == 1)
                {
                    if (data[pos] == '[')
                    {
                        m_nEscapeCode++;
                        continue;
                    }
                }
                else if (m_nEscapeCode == 2)
                {
                    if (data[pos] == 'D')
                    {
                        m_nEscapeCode = 0;
                        continue;
                    }
                }
                else
                    m_nEscapeCode = 0;
            }
            if (data[pos] < 32)
            {
                // I should soon remove this too.
                if (data[pos] == '\n') // || (data[pos] == '\r'))
                    buffer.append(" ", 1);
                    //buffer.append(&data[pos], 1);
                continue;
            }
        }

        /*
        switch(data[pos]) {
            case '&':  buffer.append("&amp;");       break;
            case '\"': buffer.append("&quot;");      break;
            case '\'': buffer.append("&apos;");      break;
            case '<':  buffer.append("&lt;");        break;
            case '>':  buffer.append("&gt;");        break;
            default:   buffer.append(&data[pos], 1); break;
        }*/
        buffer.append(&data[pos], 1);
    }
    data.swap(buffer);
}


// send message back to websocket client: 1 is message sent, 0 if failure
int ws_send_message(wsserver *s, websocketpp::connection_hdl hdl, const char *txt)
{
    std::string mystr(txt);

    cleanstring(mystr);

    try {
        s->send(hdl, mystr.c_str(), mystr.length(), websocketpp::frame::opcode::text);
        //s->send(hdl, txt, strlen(txt), websocketpp::frame::opcode::text);
        return 1;
    } catch (websocketpp::exception const & e) {
        slog(LOG_OFF, 0, "Send failed: %s", e.what());
        return 0;
    }
}


// Define a callback to handle incoming messages
void on_message(wsserver* s, websocketpp::connection_hdl hdl, message_ptr msg)
{
    class cConHook *con = 0;

    if (g_cMapHandler.find(hdl) == g_cMapHandler.end()) 
    {
        con = new cConHook();
        con->SetWebsocket(s, hdl);
        g_cMapHandler[hdl] = con;
    }

    con = (cConHook *) g_cMapHandler[hdl];
    assert(con);

    // Log here to see all commands received (plus passwords :()
    //slog(LOG_OFF, 0, "on_message called with hdl %p and messsage %s.", 
    //        hdl.lock().get(), msg->get_payload().c_str());

    // check for a special command to instruct the server to stop listening so
    // it can be cleanly exited.
    if (msg->get_payload() == "stop-listening") {
        s->stop_listening();
        con->Close( TRUE );
        return;
    }

    con->m_pFptr (con, msg->get_payload().c_str());
}


void runechoserver(void)
{
    // Create a server endpoint
    wsserver echo_server;

    try {
        // Set logging settings
        //echo_server.set_access_channels(websocketpp::log::alevel::all);
        //echo_server.clear_access_channels(websocketpp::log::alevel::frame_payload);
        echo_server.set_access_channels(websocketpp::log::alevel::none);
        echo_server.clear_access_channels(websocketpp::log::alevel::none);

        // Initialize Asio
        echo_server.init_asio();

        // Register our message handler
        // Look in endpoint.hpp for various types of handlers you can bind, e.g. set_open_handler 

        //echo_server.set_open_handler(bind(&on_open, ::_1));
        echo_server.set_close_handler(bind(&on_close, ::_1));
        echo_server.set_message_handler(bind(&on_message, &echo_server, ::_1, ::_2));

        // Listen on port
        echo_server.listen(mplex_arg.nMotherPort);

        // Start the server accept loop
        echo_server.start_accept();

        // Start the ASIO io_service run loop
        echo_server.run();
    } catch (websocketpp::exception const & e) {
        slog(LOG_OFF, 0, "Exception: %s.", e.what());
        exit(42);
    } catch (...) {
        slog(LOG_OFF, 0, "Exception other");
        exit(42);
    }
}
