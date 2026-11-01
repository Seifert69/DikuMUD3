#include "when_cache.h"

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * when_cache_tests
 *
 * cWhenCache: what "when" read from player files since boot. Lookups add entries;
 * saves and deletions only change entries that are there.
 */
BOOST_AUTO_TEST_SUITE(when_cache_tests)

static cWhenEntry player(const char *name, time_t last)
{
    cWhenEntry entry;
    entry.found = true;
    entry.name = name;
    entry.last = last;
    return entry;
}

BOOST_AUTO_TEST_CASE(empty_at_first)
{
    cWhenCache cache;
    BOOST_TEST(cache.Find("bob") == nullptr);
    BOOST_TEST(cache.Size() == 0u);
}

BOOST_AUTO_TEST_CASE(store_then_find_any_case)
{
    cWhenCache cache;
    cache.Store("Bob", player("Bob", 100));

    for (const char *name : {"bob", "BOB", "Bob"})
    {
        const cWhenEntry *entry = cache.Find(name);
        BOOST_TEST_REQUIRE(entry != nullptr);
        BOOST_TEST(entry->found);
        BOOST_TEST(entry->name == "Bob");
        BOOST_TEST(entry->last == 100);
    }
    BOOST_TEST(cache.Find("bo") == nullptr);
    BOOST_TEST(cache.Size() == 1u);
}

BOOST_AUTO_TEST_CASE(store_no_one)
{
    cWhenCache cache;
    cache.Store("corrupt", cWhenEntry{});

    const cWhenEntry *entry = cache.Find("corrupt");
    BOOST_TEST_REQUIRE(entry != nullptr);
    BOOST_TEST(!entry->found);
}

// A save changes an entry that is there, and never adds one: only lookups add
// names, so what a player does can't show in which names are known
BOOST_AUTO_TEST_CASE(saved_updates_only_a_known_name)
{
    cWhenCache cache;
    cache.Saved("bob", player("Bob", 100));
    BOOST_TEST(cache.Find("bob") == nullptr);
    BOOST_TEST(cache.Size() == 0u);

    cache.Store("bob", player("Bob", 100));
    cWhenEntry hidden = player("Bob", 200);
    hidden.wizinv = 220;
    hidden.invisible = true;
    cache.Saved("BOB", hidden);

    const cWhenEntry *entry = cache.Find("bob");
    BOOST_TEST_REQUIRE(entry != nullptr);
    BOOST_TEST(entry->last == 200);
    BOOST_TEST(entry->wizinv == 220);
    BOOST_TEST(entry->invisible);
    BOOST_TEST(cache.Size() == 1u);
}

// A saved character that was no one (deleted, then created again) is found
BOOST_AUTO_TEST_CASE(saved_after_no_one)
{
    cWhenCache cache;
    cache.Store("bob", cWhenEntry{});
    cache.Saved("bob", player("Bob", 300));

    const cWhenEntry *entry = cache.Find("bob");
    BOOST_TEST_REQUIRE(entry != nullptr);
    BOOST_TEST(entry->found);
    BOOST_TEST(entry->last == 300);
}

BOOST_AUTO_TEST_CASE(deleted_makes_a_known_name_no_one)
{
    cWhenCache cache;
    cache.Deleted("bob");
    BOOST_TEST(cache.Find("bob") == nullptr);

    cache.Store("bob", player("Bob", 100));
    cache.Store("ann", player("Ann", 100));
    cache.Deleted("Bob");

    const cWhenEntry *entry = cache.Find("bob");
    BOOST_TEST_REQUIRE(entry != nullptr);
    BOOST_TEST(!entry->found);
    BOOST_TEST(entry->name.empty());
    const cWhenEntry *other = cache.Find("ann");
    BOOST_TEST_REQUIRE(other != nullptr);
    BOOST_TEST(other->found);
    BOOST_TEST(cache.Size() == 2u);
}

BOOST_AUTO_TEST_SUITE_END()
