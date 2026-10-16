#include "bytestring.h"
#include "char_point_data.h"
#include "db.h"
#include "pc_data.h"

#include <vme.h>

#include <memory>

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * char_point_data_Suite
 *
 * readFrom() reads a character's points from a unit file. The number of
 * abilities comes from the file: every entry must be read, so the rest of the
 * unit stays in step, but only ABIL_TREE_MAX of them fit in the arrays.
 */
BOOST_AUTO_TEST_SUITE(char_point_data_Suite)

static constexpr ubit8 VERSION = 77;

// The fields before the ability list, as written for unit version 77
static void append_points_header(CByteBuffer &buf)
{
    buf.Append32(1234); // exp
    buf.Append32(0);    // flags
    buf.Append16(100);  // mana
    buf.Append16(200);  // endurance
    buf.Append8(0);     // natural armour
    buf.Append16(12);   // speed
    buf.Append16(0);    // attack type
    buf.Append16(1);    // race
    buf.Append16(0);    // offensive
    buf.Append16(0);    // defensive
    buf.Append8(1);     // sex
    buf.Append8(5);     // level
    buf.Append8(8);     // position
}

static void append_abilities(CByteBuffer &buf, int count)
{
    buf.Append8(static_cast<ubit8>(count));
    for (int i = 0; i < count; i++)
    {
        buf.Append16(static_cast<ubit16>(10 + i)); // value
        buf.Append8(static_cast<ubit8>(20 + i));   // level it was trained at (players only)
    }
}

static std::unique_ptr<pc_data> new_pc()
{
    return std::unique_ptr<pc_data>(dynamic_cast<pc_data *>(new_unit_data(UNIT_ST_PC, nullptr)));
}

BOOST_AUTO_TEST_CASE(reads_all_abilities_that_fit)
{
    CByteBuffer buf;
    append_points_header(buf);
    append_abilities(buf, ABIL_TREE_MAX);

    auto pc = new_pc();
    char_point_data points;
    int error = 0;
    points.readFrom(buf, VERSION, pc.get(), error);

    BOOST_TEST(error == 0);
    BOOST_TEST(buf.GetReadPosition() == buf.GetLength());
    for (int i = 0; i < ABIL_TREE_MAX; i++)
    {
        BOOST_TEST(points.getAbilitiesArray()[i] == 10 + i);
        BOOST_TEST(pc->getAbilityLevelAtIndex(i) == 20 + i);
    }
}

BOOST_AUTO_TEST_CASE(extra_abilities_are_read_but_not_kept)
{
    // 255 is the largest count a corrupt file can give; it used to write past both arrays
    CByteBuffer buf;
    append_points_header(buf);
    append_abilities(buf, 255);
    buf.Append8(0x5a); // the next field in the unit

    auto pc = new_pc();
    char_point_data points;
    int error = 0;
    points.readFrom(buf, VERSION, pc.get(), error);

    BOOST_TEST(error == 0);
    BOOST_TEST(buf.ReadU8() == 0x5a); // still in step with the data that follows
    for (int i = 0; i < ABIL_TREE_MAX; i++)
    {
        BOOST_TEST(points.getAbilitiesArray()[i] == 10 + i);
        BOOST_TEST(pc->getAbilityLevelAtIndex(i) == 20 + i);
    }
}

BOOST_AUTO_TEST_CASE(count_past_the_end_of_the_data_is_an_error)
{
    CByteBuffer buf;
    append_points_header(buf);
    buf.Append8(ABIL_TREE_MAX); // count, but no abilities follow

    auto pc = new_pc();
    char_point_data points;
    int error = 0;
    points.readFrom(buf, VERSION, pc.get(), error);

    BOOST_TEST(error > 0);
    BOOST_TEST(buf.IsCorrupt());
}

// Up to version 74 the speed is one byte, and a player's below SPEED_MIN becomes
// SPEED_DEFAULT (the branch for versions below 39 is gone: read_unit_string()
// refuses versions below 48)
BOOST_AUTO_TEST_CASE(version_74_speed_is_one_byte)
{
    for (const ubit8 stored : {ubit8(9), ubit8(SPEED_MIN - 1)})
    {
        CByteBuffer buf;
        buf.Append32(1234); // exp
        buf.Append32(0);    // flags
        buf.Append16(100);  // mana
        buf.Append16(200);  // endurance
        buf.Append8(0);     // natural armour
        buf.Append8(stored); // speed, one byte
        buf.Append16(0);    // attack type
        buf.Append16(1);    // race
        buf.Append16(0);    // offensive
        buf.Append16(0);    // defensive
        buf.Append8(1);     // sex
        buf.Append8(5);     // level
        buf.Append8(8);     // position
        append_abilities(buf, 2);

        auto pc = new_pc();
        char_point_data points;
        int error = 0;
        points.readFrom(buf, 74, pc.get(), error);

        BOOST_TEST(error == 0);
        BOOST_TEST(buf.GetReadPosition() == buf.GetLength());
        BOOST_TEST(points.getSpeed() == (stored < SPEED_MIN ? SPEED_DEFAULT : stored));
        BOOST_TEST(points.getAbilitiesArray()[1] == 11);
    }
}

BOOST_AUTO_TEST_SUITE_END()
