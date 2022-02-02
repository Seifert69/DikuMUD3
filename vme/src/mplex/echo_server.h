#pragma once

#include "ClientConnector.h"

namespace mplex
{

void runechoserver();
void remove_gmap(cConHook *con);
int ws_send_message(wsserver *s, websocketpp::connection_hdl hdl, const char *txt);

} // namespace mplex
