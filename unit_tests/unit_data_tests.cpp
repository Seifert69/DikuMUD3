#include "FixtureBase.h"
#include "npc_data.h"
#include "unit_data.h"

#include <cstdint>
#include <limits>
#include <memory>

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * UnitData_Alignment_Suite - setAlignment() and changeAlignmentBy() keep -1000..+1000
 */
BOOST_AUTO_TEST_SUITE(UnitData_Alignment_Suite)

struct AlignmentFixture : public unit_tests::FixtureBase
{
    std::unique_ptr<unit_data> npc{new_unit_data(UNIT_ST_NPC, nullptr)};
};

BOOST_FIXTURE_TEST_CASE(keeps_a_value_in_range, AlignmentFixture)
{
    for (int64_t value : {0, 350, -350, 1000, -1000})
    {
        BOOST_TEST(npc->setAlignment(value));
        BOOST_TEST(npc->getAlignment() == value);
    }
}

BOOST_FIXTURE_TEST_CASE(clamps_a_value_out_of_range, AlignmentFixture)
{
    BOOST_TEST(!npc->setAlignment(1001));
    BOOST_TEST(npc->getAlignment() == 1000);
    BOOST_TEST(!npc->setAlignment(-1001));
    BOOST_TEST(npc->getAlignment() == -1000);
}

// Beyond 16 bits: these used to wrap first (65000 became -536, 40000 became -1000)
BOOST_FIXTURE_TEST_CASE(clamps_before_narrowing, AlignmentFixture)
{
    BOOST_TEST(!npc->setAlignment(65000));
    BOOST_TEST(npc->getAlignment() == 1000);
    BOOST_TEST(!npc->setAlignment(40000));
    BOOST_TEST(npc->getAlignment() == 1000);
    BOOST_TEST(!npc->setAlignment(-65000));
    BOOST_TEST(npc->getAlignment() == -1000);
    BOOST_TEST(!npc->setAlignment(std::numeric_limits<int64_t>::max()));
    BOOST_TEST(npc->getAlignment() == 1000);
    BOOST_TEST(!npc->setAlignment(std::numeric_limits<int64_t>::min()));
    BOOST_TEST(npc->getAlignment() == -1000);
}

BOOST_FIXTURE_TEST_CASE(change_stays_in_range, AlignmentFixture)
{
    npc->setAlignment(900);
    npc->changeAlignmentBy(300);
    BOOST_TEST(npc->getAlignment() == 1000);
    npc->changeAlignmentBy(-70000);
    BOOST_TEST(npc->getAlignment() == -1000);
}

BOOST_AUTO_TEST_SUITE_END()
