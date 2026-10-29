#pragma once

#include <chrono>
#include <cstddef>
#include <deque>
#include <iterator>
#include <unordered_map>

/**
 * Limits the "when" lookups that miss its cache: one per wait for
 * each asker (by key), and per_second in any one second for all of them together.
 * The caller passes the time, from a monotonic clock.
 */
class cWhenLimit
{
public:
    using clock = std::chrono::steady_clock;

    cWhenLimit(clock::duration wait, int per_second)
        : m_wait(wait)
        , m_per_second(per_second)
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

        // The last second, not the current one: a fixed window allowed twice
        // per_second across its boundary
        while (!m_recent.empty() && now - m_recent.front() >= std::chrono::seconds(1))
        {
            m_recent.pop_front();
        }
        if (static_cast<int>(m_recent.size()) >= m_per_second)
        {
            return false;
        }
        m_recent.push_back(now);

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
    const int m_per_second;
    std::unordered_map<long, clock::time_point> m_last; ///< By key, when each last looked up
    std::deque<clock::time_point> m_recent;             ///< All lookups in the last second
};
