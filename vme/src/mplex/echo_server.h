#pragma once

#include "ClientConnector.h"

namespace mplex
{

void runechoserver();
void start_websocket_server();
void stop_websocket_server();
void remove_gmap(cConHook *con);
int ws_send_message(wsserver_tls *s, websocketpp::connection_hdl hdl, const char *txt);

} // namespace mplex
