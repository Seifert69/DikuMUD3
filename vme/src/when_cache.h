#pragma once

#include "textutil.h"

#include <cstddef>
#include <ctime>
#include <string>
#include <unordered_map>

/// What "when" needs from a player file
struct cWhenEntry
{
    bool found{false};     ///< false: no one (the file was rejected or deleted, or has no names or no logon time)
    std::string name;      ///< The player's name as shown
    time_t last{0};        ///< When they last logged on
    int wizinv{0};         ///< Their level of wizard invisibility
    bool invisible{false}; ///< UNIT_FL_INVISIBLE
};

/**
 * What "when" read from player files since boot, by player name, so that each
 * file is read at most once per boot. Empty at boot. An entry is added only when
 * "when" reads a file that exists; a save or a deletion then updates an entry
 * that is there, and never adds one, so whether a name is here depends only on
 * the lookups made, not on what the player does.
 */
class cWhenCache
{
public:
    /// The entry for name, nullptr if there is none
    const cWhenEntry *Find(const std::string &name) const
    {
        auto it = m_entries.find(Key(name));
        return it == m_entries.end() ? nullptr : &it->second;
    }

    /// Adds or replaces the entry for name
    void Store(const std::string &name, const cWhenEntry &entry) { m_entries[Key(name)] = entry; }

    /// Replaces the entry for name if there is one: the player file was saved
    void Saved(const std::string &name, const cWhenEntry &entry)
    {
        auto it = m_entries.find(Key(name));
        if (it != m_entries.end())
        {
            it->second = entry;
        }
    }

    /// Makes the entry for name, if there is one, say no one: the player file was deleted
    void Deleted(const std::string &name)
    {
        auto it = m_entries.find(Key(name));
        if (it != m_entries.end())
        {
            it->second = cWhenEntry{};
        }
    }

    /// How many names it holds
    std::size_t Size() const { return m_entries.size(); }

private:
    /// Lower case, as PlayerFileName() makes the file names
    static std::string Key(std::string name)
    {
        str_lower(name);
        return name;
    }

    std::unordered_map<std::string, cWhenEntry> m_entries;
};

class unit_data;

/// What "when" read from player files since boot
cWhenCache &when_cache();

/// What "when" needs from pc. A player with no names (a DIL script can remove them all) or no logon time is no one.
cWhenEntry when_entry_of(unit_data *pc);

/// Keep what "when" remembers of a player file up to date: called by save_player_file() and delete_player()
void when_player_saved(unit_data *pc);
void when_player_deleted(const char *name);
