#pragma once

#include <cstdint>
#include <string>
#include <vector>

/**
 * Building blocks for the "$combat" packet sent to web clients once per
 * combat round, e.g.
 *
 *   <data type='$combat'>{"round":17,"list":[{"id":"…","name":"the mangy mongrel",
 *   "hp":62,"target":"you","me":false,"foe":true}, …]}</data>
 *
 * Everything here works on plain values so it can be unit tested without
 * a running world. The unit_data side lives in combat.cpp.
 */
namespace combat_packet
{
/// Max entries in one packet's list (the player included), keeps packets small
constexpr int MAX_ENTRIES = 12;

struct Entry
{
    std::string id;     ///< opaque id, see opaqueId()
    std::string name;   ///< name as the receiving player sees it
    int hp{0};          ///< hit points in percent, 0-100
    std::string target; ///< who this being melees, as the player sees it; "you" or ""
    bool me{false};     ///< the receiving player
    bool foe{false};    ///< hostile to the receiving player
};

/// {"round":N,"list":[…]}, with "end":true added when end is set
std::string listJSON(int round, const std::vector<Entry> &list, bool end = false);

/// {"round":N,"list":[],"end":true}
std::string endJSON(int round);

/// {"event":"death","id":"…"}
std::string deathJSON(const std::string &id);

/// Wraps json in <data type='$combat'>…</data>
std::string wrap(const std::string &json);

/// hp as an integer percent of max, clamped to 0-100 (0 if max <= 0)
int hpPercent(long hp, long max);

/// Opaque, stable id for a unit address: a keyed hash, never the raw pointer
std::string opaqueId(const void *p, uint64_t salt);

/// opaqueId() with a per-process random salt
std::string opaqueId(const void *p);
} // namespace combat_packet
