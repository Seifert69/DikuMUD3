#pragma once

#include <ctime>
#include <string>
#include <utility>
#include <vector>

class pc_data;

/*
 * Session log: one JSON object per event, written to vme.log (file only, not
 * the in-game log) after a fixed code, used to measure player sessions and
 * retention. Players are identified by their player id only - no names and no
 * host addresses are written.
 *
 *   Thu Oct 22 20:26:37 2026 :: SESSION {"t":1792700797,"ev":"login","id":42,"lvl":1,"new":0,"web":0,"played":174}
 *
 * "t" (Unix time) is the time of the event; use it, not the prefix: that is
 * slog()'s own reading of the clock (it can be a second later) in the
 * server's local time.
 *
 * To collect them, oldest first, from the rotated logs (vme.log.N.gz, N=1 the
 * newest) and the current one:
 *   zgrep -a -h -E '^.{24} :: SESSION [{]' $(ls -v vme.log.*.gz | tac) vme.log |
 *     sed 's/^.\{24\} :: SESSION //' > sessions.jsonl
 * The pattern is anchored to the 24-character timestamp slog() writes, so other
 * log lines that contain the code (e.g. player text) don't match; -a keeps grep
 * reading text after a stray NUL byte, and zgrep reads each file on its own, so
 * a last line cut off by a crash or power loss doesn't swallow the first line
 * of the next file (that cut line is not valid JSON: skip lines that don't
 * parse). runvme.sh keeps only 10 rotated logs (one per restart, crash
 * restarts too), and vme started without it truncates vme.log, so a longer
 * history has to be archived before it rotates away.
 *
 * Events: boot, shutdown, login, reconnect, linkdead, logout.
 * "played" (login only) is the time played saved in the player file before
 * this session; the length of a session is the logout "t" minus the login "t"
 * (time in the game, link-dead, idle or at the immortal menu included).
 * A session starts with login (a connection takes over the character) and ends
 * with logout (the character leaves the game for good). Quitting to the
 * immortal menu does not end it. Players still in the game when the server
 * stops get no logout: their sessions end at the shutdown event, or after a
 * crash at the next boot event (the last event before it is the best guess of
 * when the server went down). Shutdown saves every connected player in the
 * game (new characters too) and then closes every connection; an immortal at
 * the menu is removed then, so it does log logout.
 */

using sessionlog_fields = std::vector<std::pair<const char *, long long>>;

// Builds one log line (without the trailing newline). Exposed for unit tests.
std::string sessionlog_line(time_t when, const char *event, const sessionlog_fields &fields);

void sessionlog_boot();
void sessionlog_shutdown();
void sessionlog_login(pc_data *pc, bool new_player);
void sessionlog_reconnect(pc_data *pc);
void sessionlog_linkdead(pc_data *pc);
void sessionlog_logout(pc_data *pc);
