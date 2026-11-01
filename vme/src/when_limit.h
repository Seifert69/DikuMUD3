#pragma once

#include <chrono>
#include <cstddef>
#include <iterator>
#include <unordered_map>

/**
 * Limits the "when" lookups that miss its cache: one per wait for each asker (by key).
 * The caller passes the time, from a monotonic clock.
 */
class cWhenLimit
{
public:
    using clock = std::chrono::steady_clock;

    explicit cWhenLimit(clock::duration wait)
        : m_wait(wait)
    {
    }

    /// Whether key may look up now, and if so counts it
    bool Allow(long key, clock::time_point now)
    {
        auto it = m_last.find(key);
        if (it != m_last.end() && now - it->second < m_wait)
        {
            return false;
        }

        if (m_last.size() > 1000) // only recent ones matter
        {
            for (auto i = m_last.begin(); i != m_last.end();)
            {
                i = now - i->second >= m_wait ? m_last.erase(i) : std::next(i);
            }
        }
        m_last[key] = now;
        return true;
    }

    /// How many keys it remembers a lookup for
    std::size_t Size() const { return m_last.size(); }

private:
    const clock::duration m_wait;
    std::unordered_map<long, clock::time_point> m_last; ///< By key, when each last looked up
};
