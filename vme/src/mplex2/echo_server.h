#pragma once

void runechoserver(void);
void remove_gmap(class cConHook *con);
int ws_send_message(wsserver *s, websocketpp::connection_hdl hdl, const char *txt);
