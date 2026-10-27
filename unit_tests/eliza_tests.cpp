#include "eliza.h"

#include <cstring>
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

namespace
{
/// The longest command argument made of unit repeated
std::string repeated(const std::string &unit)
{
    std::string line;
    while (line.size() + unit.size() < MAX_INPUT_LENGTH)
    {
        line += unit;
    }
    return line;
}

/// The most bytes preprocess_string() uses while it rewrites line. It works in place, so the bytes it
/// never touched are still '#' (which the lines don't contain).
size_t peak_use(const std::string &line)
{
    std::vector<char> buf(2 * ORACLE_BUF_SIZE, '#');
    memcpy(buf.data(), line.c_str(), line.size() + 1);

    preprocess_string(buf.data(), "eliza");

    size_t used = buf.size();
    while (used > 0 && buf[used - 1] == '#')
    {
        used--;
    }
    return used;
}
} // namespace

/**********************************************************************************
 * Eliza_Preprocess_Suite - the oracle rewrites any line a player can say in a buffer of
 * ORACLE_BUF_SIZE: padding punctuation and growing words must stay within it
 */
BOOST_AUTO_TEST_SUITE(Eliza_Preprocess_Suite)

BOOST_AUTO_TEST_CASE(any_command_argument_fits)
{
    for (const char *unit : {"!", "id!", "id ", "!id", "i'd!", "im!", "ive!", "cant!", "cannot!", "dont!", "youve!", "youre!", "a"})
    {
        BOOST_TEST_CONTEXT(unit)
        {
            BOOST_TEST(peak_use(repeated(unit)) < ORACLE_BUF_SIZE);
        }
    }
}

// Every unit of up to four tokens; the worst uses 10/3 times the line ("!id"), so 4 times leaves room
BOOST_AUTO_TEST_CASE(worst_growth_is_below_four_times)
{
    const std::vector<std::string> tokens{
        "!", "id", "im", "ive", "cant", "cannot", "dont", "youve", "youre", "but", "eliza", " ", "'", "i"};
    double worst = 0;
    std::string worst_unit;
    for (const auto &a : tokens)
    {
        for (const auto &b : tokens)
        {
            for (const auto &c : tokens)
            {
                for (const auto &d : tokens)
                {
                    const std::string unit = a + b + c + d;
                    const std::string line = repeated(unit);
                    const double growth = static_cast<double>(peak_use(line)) / line.size();
                    if (growth > worst)
                    {
                        worst = growth;
                        worst_unit = unit;
                    }
                }
            }
        }
    }
    BOOST_TEST_MESSAGE("worst growth " << worst << " for \"" << worst_unit << "\"");
    BOOST_TEST(worst < 4.0);
}

// A reply quotes the player's words, and each '@' in it starts a command for the oracle to run
BOOST_AUTO_TEST_CASE(no_commands_from_the_player)
{
    char line[ORACLE_BUF_SIZE] = "i @kill papi you@drop all";
    preprocess_string(line, "eliza");
    BOOST_TEST(std::string(line) == " i kill papi youdrop all ");
}

BOOST_AUTO_TEST_SUITE_END()
