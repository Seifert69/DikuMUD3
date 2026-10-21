#pragma once

#include <cstdint>
#include <string>
#include <vector>

/**
 * Building blocks for the "$combat" packet sent to web clients once per
 * combat round, e.g.
 *
 *   <data type='$combat'>{"round":17,"list":[{"id":"…","name":"the mangy mongrel",
 *   "life":4,"target":"you","me":false,"foe":true,"byYou":[3,6],"byOthers":[]}, …]}</data>
 *
 * Others' health is only "life", 0-5 boxes, about as much as the prompt's opponent
 * diagnosis tells; the exact "hp" percent is sent for the player alone. byYou and
 * byOthers are this round's blows on that being, each the 0-8 step that picks the
 * combat message's words (damage_index()).
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
    int hp{0};          ///< hit points in percent, 0-100; sent as is for the player only
    std::string target; ///< who this being melees, as the player sees it; "you" or ""
    bool me{false};     ///< the receiving player
    bool foe{false};    ///< hostile to the receiving player
    std::vector<int> byYou;    ///< this round's blows on it by the receiving player (0-8 each)
    std::vector<int> byOthers; ///< this round's blows on it by anyone else
};

/// hp percent as 0-5 boxes of life: 0 only at 0 hp, a sliver of health is 1
int lifeBoxes(int hpPercent);

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
