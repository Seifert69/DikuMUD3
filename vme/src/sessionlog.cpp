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

// A connection has taken over the character. Not logged for a character
// without a connection (e.g. an admin 'load'), one already extracted while
// entering the game, or a session already logged (e.g. an immortal entering
// the game again from the menu).
void sessionlog_login(pc_data *pc, bool new_player)
{
    descriptor_data *d = CHAR_DESCRIPTOR(pc);
    if (d == nullptr || pc->is_destructed() || pc->isSessionLogged())
    {
        return;
    }

    sessionlog_fields fields = sessionlog_pc_fields(pc);
    fields.emplace_back("new", new_player ? 1 : 0);
    sessionlog_add_client(d, fields);
    fields.emplace_back("played", pc->getPCTimeInformation().getTotalTimePlayedInSeconds());
    sessionlog_write("login", fields);
    pc->setSessionLogged(true);
}

// A new connection has taken over a character already in the game. That is a
// login if no session was logged for it (e.g. it was loaded by an admin).
void sessionlog_reconnect(pc_data *pc)
{
    if (!pc->isSessionLogged())
    {
        sessionlog_login(pc, PC_IS_UNSAVED(pc));
        return;
    }

    sessionlog_fields fields = sessionlog_pc_fields(pc);
    sessionlog_add_client(CHAR_DESCRIPTOR(pc), fields);
    sessionlog_write("reconnect", fields);
}

// The connection was lost while the character is in the game. Not logged while
// the server shuts down: that closes every connection, and the shutdown event
// ends those sessions.
void sessionlog_linkdead(pc_data *pc)
{
    if (!pc->isSessionLogged() || g_mud_shutdown)
    {
        return;
    }

    sessionlog_write("linkdead", sessionlog_pc_fields(pc));
}

// The character leaves the game for good (quit, removed while link dead, ...).
// No "played": a character never saved (a new one) still has the time from its
// player file here; others are saved just before.
void sessionlog_logout(pc_data *pc)
{
    if (!pc->isSessionLogged())
    {
        return;
    }

    sessionlog_write("logout", sessionlog_pc_fields(pc));
    pc->setSessionLogged(false);
}
