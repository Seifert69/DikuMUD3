#include "sessionlog.h"

#include "descriptor_data.h"
#include "hookmud.h"
#include "main_functions.h"
#include "pc_data.h"
#include "slog.h"
#include "utils.h"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

std::string sessionlog_line(time_t when, const char *event, const sessionlog_fields &fields)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    writer.StartObject();
    writer.Key("t");
    writer.Int64(when);
    writer.Key("ev");
    writer.String(event);
    for (const auto &field : fields)
    {
        writer.Key(field.first);
        writer.Int64(field.second);
    }
    writer.EndObject();

    return buffer.GetString();
}

// LOG_OFF: the line goes to vme.log only, not to the wizards' in-game log
static void sessionlog_write(const char *event, const sessionlog_fields &fields)
{
    slog(LOG_OFF, 0, "SESSION %s", sessionlog_line(time(nullptr), event, fields));
}

static sessionlog_fields sessionlog_pc_fields(pc_data *pc)
{
    return {{"id", pc->getPlayerUID()}, {"lvl", CHAR_LEVEL(pc)}};
}

// 1 if the connection came through a websocket mplex (the web client), 0 for
// telnet or an mplex started with -y (ASCII), as the mplex reports it
static void sessionlog_add_client(descriptor_data *d, sessionlog_fields &fields)
{
    fields.emplace_back("web", d->getMultiHookPtr()->bWebsockets ? 1 : 0);
}

void sessionlog_boot()
{
    sessionlog_write("boot", {});
}

void sessionlog_shutdown()
{
    sessionlog_write("shutdown", {});
}

// The four below are called only by pc_data::connect_game() and
// disconnect_game(), which decide when each event happens.

// A connection has taken over a character that was not in the game, or one an
// admin loaded
void sessionlog_login(pc_data *pc, bool new_player)
{
    sessionlog_fields fields = sessionlog_pc_fields(pc);
    fields.emplace_back("new", new_player ? 1 : 0);
    sessionlog_add_client(CHAR_DESCRIPTOR(pc), fields);
    fields.emplace_back("played", pc->getPCTimeInformation().getTotalTimePlayedInSeconds());
    sessionlog_write("login", fields);
}

// A new connection has taken over a character already in the game
void sessionlog_reconnect(pc_data *pc)
{
    sessionlog_fields fields = sessionlog_pc_fields(pc);
    sessionlog_add_client(CHAR_DESCRIPTOR(pc), fields);
    sessionlog_write("reconnect", fields);
}

// The connection was lost while the character is in the game. Not logged while
// the server shuts down: that closes every connection, and the shutdown event
// ends those sessions.
void sessionlog_linkdead(pc_data *pc)
{
    if (g_mud_shutdown)
    {
        return;
    }

    sessionlog_write("linkdead", sessionlog_pc_fields(pc));
}

// The character leaves the game (quit, the immortal menu, removed while link
// dead, ...). No "played": for a character never saved (a new one) it is
// still 0 here; others are saved just after.
void sessionlog_logout(pc_data *pc)
{
    sessionlog_write("logout", sessionlog_pc_fields(pc));
}
