#define BOOST_TEST_MODULE "cNamelists Unit Tests"
#include <boost/test/unit_test.hpp>
#include "namelist.h"
#include <vector>

BOOST_AUTO_TEST_SUITE(cNamelist_tests)

/**
 * This test is really really important
 * its being used to verify the other tests
 */
BOOST_AUTO_TEST_CASE(json_test)
{
    cNamelist list;
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.json();
    ////////////////////////// Test Subject //////////////////////////////
    {
        std::string expected = R"("namelist": [])";
        BOOST_REQUIRE(ret == expected);
    }
}

BOOST_AUTO_TEST_CASE(default_ctor_test)
{
    ////////////////////////// Test Subject //////////////////////////////
    cNamelist list;
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 0);
}

BOOST_AUTO_TEST_CASE(dbl_ptr_ctor_test)
{
    const char *data[] = {"Thor", "Loki", "Tyr", nullptr};
    ////////////////////////// Test Subject //////////////////////////////
    cNamelist list(data);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 3);

    {
        std::string expected = R"("namelist": ["Thor","Loki","Tyr"])";
        auto ret = list.json();
        BOOST_TEST(ret == expected);
    }
}

BOOST_AUTO_TEST_CASE(operator_eq1_test)
{
    cNamelist list;
    // This coredumps as doesn't check for null
    // TODO enable after refactor
    // cNamelist *ptr{};
    ////////////////////////// Test Subject //////////////////////////////
    // list = ptr;
    ////////////////////////// Test Subject //////////////////////////////
    // BOOST_TEST(true, "Cool - didn't crash");
}

BOOST_AUTO_TEST_CASE(operator_eq2_test)
{
    cNamelist list;
    const char **ptr{};
    ////////////////////////// Test Subject //////////////////////////////
    list = ptr;
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(true, "Cool - didn't crash");
}

BOOST_AUTO_TEST_CASE(operator_eq3_test)
{
    cNamelist list;
    const char *data[] = {"Thor", "Loki", "Tyr", nullptr};
    ////////////////////////// Test Subject //////////////////////////////
    list = data;
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 3);

    {
        std::string expected = R"("namelist": ["Thor","Loki","Tyr"])";
        auto ret = list.json();
        BOOST_TEST(ret == expected);
    }
}

BOOST_AUTO_TEST_CASE(operator_eq4_test)
{
    cNamelist list_a;
    cNamelist list_b;
    const char *data[] = {"Thor", "Loki", "Tyr", nullptr};
    list_a = data;
    ////////////////////////// Test Subject //////////////////////////////
    list_b = &list_a;
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list_b.Length() == 3);

    {
        std::string expected = R"("namelist": ["Thor","Loki","Tyr"])";
        auto ret = list_b.json();
        BOOST_TEST(ret == expected);
    }
}

BOOST_AUTO_TEST_CASE(Free_test)
{
    cNamelist list;
    list.AppendName("Thor");
    list.PrependName("Loki");
    list.InsertName("Tyr", 10);
    ////////////////////////// Test Subject //////////////////////////////
    list.Free();
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(true, "Cool - didn't crash");
    BOOST_TEST(list.Length() == 0);
}

BOOST_AUTO_TEST_CASE(CopyList1_test)
{
    cNamelist list;
    const char **ptr{};
    ////////////////////////// Test Subject //////////////////////////////
    list.CopyList(ptr);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(true, "Cool - didn't crash");
}

BOOST_AUTO_TEST_CASE(CopyList2_test)
{
    cNamelist list;
    // This coredumps as doesn't check for null
    // TODO enable after refactor
    // cNamelist *ptr{};
    ////////////////////////// Test Subject //////////////////////////////
    // list.CopyList(ptr);
    ////////////////////////// Test Subject //////////////////////////////
    // BOOST_TEST(true, "Cool - didn't crash");
}

BOOST_AUTO_TEST_CASE(CopyList3_test)
{
    cNamelist list;
    const char *data[] = {"Thor", "Loki", "Tyr", nullptr};
    ////////////////////////// Test Subject //////////////////////////////
    list.CopyList(data);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 3);

    {
        std::string expected = R"("namelist": ["Thor","Loki","Tyr"])";
        auto ret = list.json();
        BOOST_TEST(ret == expected);
    }
}

BOOST_AUTO_TEST_CASE(CopyList4_test)
{
    cNamelist list_a;
    cNamelist list_b;

    const char *data[] = {"Thor", "Loki", "Tyr", nullptr};
    list_a.CopyList(data);
    ////////////////////////// Test Subject //////////////////////////////
    list_b.CopyList(&list_a);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list_a.Length() == 3);

    {
        std::string expected = R"("namelist": ["Thor","Loki","Tyr"])";
        auto ret = list_b.json();
        BOOST_TEST(ret == expected);
    }
}

BOOST_AUTO_TEST_CASE(AppendBuffer1_test)
{
    cNamelist list;
    CByteBuffer buf;
    ////////////////////////// Test Subject //////////////////////////////
    list.AppendBuffer(&buf);
    ////////////////////////// Test Subject //////////////////////////////
    {
        auto len = buf.GetLength();
        std::vector<ubit8> ret(buf.GetData(), buf.GetData() + len);
        BOOST_TEST(len == 4);
        ubit8 expected[4]{0, 0, 0, 0};
        BOOST_REQUIRE(len == sizeof(expected));
        BOOST_TEST(expected == ret);
    }
}

BOOST_AUTO_TEST_CASE(AppendBuffer2_test)
{
    cNamelist list;
    // TODO Enable after refactor - this coredumps now
    ////////////////////////// Test Subject //////////////////////////////
    //    list.AppendBuffer(nullptr);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(true, "Cool - didn't crash");
}

BOOST_AUTO_TEST_CASE(AppendBuffer3_test)
{
    cNamelist list;
    list.AppendName("Thor");
    list.AppendName("Tyr");
    list.AppendName("Heimdall");
    CByteBuffer buf;
    ////////////////////////// Test Subject //////////////////////////////
    list.AppendBuffer(&buf);
    ////////////////////////// Test Subject //////////////////////////////
    {
        auto len = buf.GetLength();
        std::vector<ubit8> ret(buf.GetData(), buf.GetData() + len);
        BOOST_TEST(len == 22);
        ubit8 expected[22] = {3, 0, 0, 0, 'T', 'h', 'o', 'r', 0, 'T', 'y', 'r', 0, 'H', 'e', 'i', 'm', 'd', 'a', 'l', 'l'};
        BOOST_REQUIRE(len == sizeof(expected));
        BOOST_TEST(expected == ret);
    }
}

BOOST_AUTO_TEST_CASE(ReadBuffer1_test)
{
    cNamelist list;
    CByteBuffer buf;
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.ReadBuffer(&buf, 4387);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(ret == 1);
    BOOST_TEST(list.Length() == 0);
}

BOOST_AUTO_TEST_CASE(ReadBuffer2_test)
{
    cNamelist list;
    CByteBuffer buf;
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.ReadBuffer(&buf, 4);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(ret == 1);
    BOOST_TEST(list.Length() == 0);
}

BOOST_AUTO_TEST_CASE(ReadBuffer3_test)
{
    cNamelist list;
    CByteBuffer buf;
    buf.Append32(3);
    buf.AppendString("Heimdall");
    buf.AppendString("Tyr");
    buf.AppendString("Loki");
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.ReadBuffer(&buf, 4387);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(ret == 0);
    BOOST_TEST(list.Length() == 3);

    {
        std::string expected = R"("namelist": ["Heimdall","Tyr","Loki"])";
        auto ret = list.json();
        BOOST_TEST(ret == expected);
    }
}

BOOST_AUTO_TEST_CASE(ReadBuffer4_test)
{
    cNamelist list;
    CByteBuffer buf;
    buf.AppendString("Heimdall");
    buf.AppendString("Tyr");
    buf.AppendString("Loki");
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.ReadBuffer(&buf, 50); // <65 tested in func
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(ret == 1);
    BOOST_TEST(list.Length() == 3);

    {
        std::string expected = R"("namelist": ["Heimdall","Tyr","Loki"])";
        auto ret = list.json();
        BOOST_TEST(ret == expected);
    }
}

BOOST_AUTO_TEST_CASE(ReadBuffer5_test)
{
    cNamelist list;
    CByteBuffer buf;
    buf.AppendString("");
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.ReadBuffer(&buf, 50); // <65 tested in func
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(ret == 0);
    BOOST_TEST(list.Length() == 0);
}

BOOST_AUTO_TEST_CASE(bread1_test)
{
    cNamelist list;
    // This coredumps as doesn't check for null
    // TODO enable after refactor
    // ubit8 **b{};
    ////////////////////////// Test Subject //////////////////////////////
    // list.bread(b);
    ////////////////////////// Test Subject //////////////////////////////
    // BOOST_TEST(true, "Cool - didn't crash");
}

BOOST_AUTO_TEST_CASE(bread2_test)
{
    cNamelist list;
    ubit8 buffer[100]{};
    ubit8 *buf_ptr = buffer;
    ////////////////////////// Test Subject //////////////////////////////
    list.bread(&buf_ptr);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 0);
}

BOOST_AUTO_TEST_CASE(bread3_test)
{
    cNamelist list;
    ubit8 buffer[100]{3, 0, 0, 0, 'T', 'y', 'r', 0, 'L', 'o', 'k', 'i', 0, 'T', 'h', 'o', 'r'};
    ubit8 *buf_ptr = buffer;
    ////////////////////////// Test Subject //////////////////////////////
    list.bread(&buf_ptr);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 3);

    {
        std::string expected = R"("namelist": ["Tyr","Loki","Thor"])";
        auto ret = list.json();
        BOOST_TEST(ret == expected);
    }
}

BOOST_AUTO_TEST_CASE(bwrite1_test)
{
    cNamelist list;
    // This coredumps as doesn't check for null
    // TODO enable after refactor
    // ubit8 **b{};
    ////////////////////////// Test Subject //////////////////////////////
    // list.bwrite(b);
    ////////////////////////// Test Subject //////////////////////////////
    // BOOST_TEST(true, "Cool - didn't crash");
}

BOOST_AUTO_TEST_CASE(bwrite2_test)
{
    cNamelist list;
    ubit8 buffer[100]{};
    ubit8 *buf_ptr = buffer;
    ////////////////////////// Test Subject //////////////////////////////
    list.bwrite(&buf_ptr);
    ////////////////////////// Test Subject //////////////////////////////
    ubit8 expected[100]{};
    BOOST_TEST(buffer == expected);
}

BOOST_AUTO_TEST_CASE(bwrite3_test)
{
    cNamelist list;
    list.AppendName("Tyr");
    list.AppendName("Loki");
    list.AppendName("Thor");
    ubit8 buffer[100]{};
    ubit8 *buf_ptr = buffer;
    ////////////////////////// Test Subject //////////////////////////////
    list.bwrite(&buf_ptr);
    ////////////////////////// Test Subject //////////////////////////////
    ubit8 expected[100]{3, 0, 0, 0, 'T', 'y', 'r', 0, 'L', 'o', 'k', 'i', 0, 'T', 'h', 'o', 'r'};
    BOOST_TEST(buffer == expected);
}

BOOST_AUTO_TEST_CASE(catnames1_test)
{
    cNamelist list;
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.catnames();
    ////////////////////////// Test Subject //////////////////////////////
    std::string expected = R"({})";
    BOOST_REQUIRE_NE(ret, nullptr);
    BOOST_TEST(std::string(ret) == expected);
}

BOOST_AUTO_TEST_CASE(catnames2_test)
{
    cNamelist list;
    list.AppendName("Thor");
    list.AppendName("Loki");
    list.AppendName("Tyr");
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.catnames();
    ////////////////////////// Test Subject //////////////////////////////
    std::string expected = R"({"Thor","Loki","Tyr"})";
    BOOST_REQUIRE_NE(ret, nullptr);
    BOOST_TEST(std::string(ret) == expected);
}

BOOST_AUTO_TEST_CASE(Remove1_test)
{
    cNamelist list;
    ////////////////////////// Test Subject //////////////////////////////
    list.Remove(0);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(true, "Cool - didn't crash");
}

BOOST_AUTO_TEST_CASE(Remove2_test)
{
    cNamelist list;
    ////////////////////////// Test Subject //////////////////////////////
    list.Remove(45463);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(true, "Cool - didn't crash");
}

BOOST_AUTO_TEST_CASE(Remove3_test)
{
    cNamelist list;
    list.AppendName("Thor");
    ////////////////////////// Test Subject //////////////////////////////
    list.Remove(0);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 0);
}

BOOST_AUTO_TEST_CASE(Remove4_test)
{
    cNamelist list;
    list.AppendName("Thor");
    ////////////////////////// Test Subject //////////////////////////////
    list.Remove(1);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 1);

    {
        std::string expected = R"("namelist": ["Thor"])";
        auto ret = list.json();
        BOOST_TEST(ret == expected);
    }
}

BOOST_AUTO_TEST_CASE(Remove5_test)
{
    cNamelist list;
    list.AppendName("Thor");
    list.AppendName("Loki");
    list.AppendName("Tyr");
    ////////////////////////// Test Subject //////////////////////////////
    list.Remove(1);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 2);

    {
        std::string expected = R"("namelist": ["Thor","Tyr"])";
        auto ret = list.json();
        BOOST_TEST(ret == expected);
    }
}

BOOST_AUTO_TEST_CASE(RemoveName1_test)
{
    cNamelist list;
    ////////////////////////// Test Subject //////////////////////////////
    list.RemoveName("Thor");
    ////////////////////////// Test Subject //////////////////////////////
}

BOOST_AUTO_TEST_CASE(RemoveName2_test)
{
    cNamelist list;
    ////////////////////////// Test Subject //////////////////////////////
    list.RemoveName(nullptr);
    ////////////////////////// Test Subject //////////////////////////////
}

BOOST_AUTO_TEST_CASE(RemoveName3_test)
{
    cNamelist list;
    list.AppendName("Thor");
    ////////////////////////// Test Subject //////////////////////////////
    list.RemoveName("Thor");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 0);

    {
        std::string expected = R"("namelist": [])";
        auto ret = list.json();
        BOOST_TEST(ret == expected);
    }
}

BOOST_AUTO_TEST_CASE(RemoveName4_test)
{
    cNamelist list;
    list.AppendName("Thor");
    list.AppendName("Tyr");
    ////////////////////////// Test Subject //////////////////////////////
    list.RemoveName("Thor");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 1);

    {
        std::string expected = R"("namelist": ["Tyr"])";
        auto ret = list.json();
        BOOST_TEST(ret == expected);
    }
}

BOOST_AUTO_TEST_CASE(RemoveName5_test)
{
    cNamelist list;
    list.AppendName("Thor");
    list.AppendName("Loki");
    list.AppendName("Tyr");
    ////////////////////////// Test Subject //////////////////////////////
    list.RemoveName("Loki");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 2);

    {
        std::string expected = R"("namelist": ["Thor","Tyr"])";
        auto ret = list.json();
        BOOST_TEST(ret == expected);
    }
}

BOOST_AUTO_TEST_CASE(RemoveName6_test)
{
    cNamelist list;
    list.AppendName("Thor");
    list.AppendName("Thor");
    list.AppendName("Thor");
    ////////////////////////// Test Subject //////////////////////////////
    list.RemoveName("Thor");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 2);

    {
        std::string expected = R"("namelist": ["Thor","Thor"])";
        auto ret = list.json();
        BOOST_TEST(ret == expected);
    }
}

BOOST_AUTO_TEST_CASE(RemoveName7_test)
{
    cNamelist list;
    list.AppendName("Thor");
    list.AppendName("LOKI");
    list.AppendName("Heimdall");
    ////////////////////////// Test Subject //////////////////////////////
    list.RemoveName("lOkI");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 2);

    {
        std::string expected = R"("namelist": ["Thor","Heimdall"])";
        auto ret = list.json();
        BOOST_TEST(ret == expected);
    }
}

BOOST_AUTO_TEST_CASE(Substitute_test)
{
    cNamelist list;
    ////////////////////////// Test Subject //////////////////////////////
    list.Substitute(0, "Thor");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(true, "Cool - didn't crash");
    BOOST_TEST(list.Length() == 0);
}

BOOST_AUTO_TEST_CASE(Substitue2_test)
{
    cNamelist list;
    ////////////////////////// Test Subject //////////////////////////////
    list.Substitute(355, "Thor");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(true, "Cool - didn't crash");
    BOOST_TEST(list.Length() == 0);
}

BOOST_AUTO_TEST_CASE(Substitue3_test)
{
    cNamelist list;
    list.AppendName("Thor");
    list.AppendName("Loki");
    list.AppendName("Tyr");
    ////////////////////////// Test Subject //////////////////////////////
    list.Substitute(1, "Homer");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 3);

    {
        std::string expected = R"("namelist": ["Thor","Homer","Tyr"])";
        auto ret = list.json();
        BOOST_TEST(ret == expected);
    }
}

BOOST_AUTO_TEST_CASE(Substitue4_test)
{
    cNamelist list;
    list.AppendName("Thor");
    list.AppendName("Loki");
    list.AppendName("Tyr");
    ////////////////////////// Test Subject //////////////////////////////
    list.Substitute(1, nullptr);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 3);

    {
        std::string expected = R"("namelist": ["Thor","","Tyr"])";
        auto ret = list.json();
        BOOST_TEST(ret == expected);
    }
}

BOOST_AUTO_TEST_CASE(Name1_test)
{
    cNamelist list;
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.Name();
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(ret == nullptr);
}

BOOST_AUTO_TEST_CASE(Name2_test)
{
    cNamelist list;
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.Name(546);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(ret == nullptr);
}

BOOST_AUTO_TEST_CASE(Name3_test)
{
    cNamelist list;
    list.AppendName("Loki");
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.Name();
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_REQUIRE_NE(ret, nullptr);
    BOOST_TEST(std::string(ret) == "Loki");
}

BOOST_AUTO_TEST_CASE(Name4_test)
{
    cNamelist list;
    list.AppendName("Loki");
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.Name(2);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(ret == nullptr);
}

BOOST_AUTO_TEST_CASE(Name5_test)
{
    cNamelist list;
    list.AppendName("Loki");
    list.AppendName("Thor");
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.Name(1);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_REQUIRE_NE(ret, nullptr);
    BOOST_TEST(std::string(ret) == "Thor");
}

BOOST_AUTO_TEST_CASE(InstanceName_test)
{
    cNamelist list;
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.InstanceName();
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(ret == nullptr);
}

BOOST_AUTO_TEST_CASE(InstanceName2_test)
{
    cNamelist list;
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.InstanceName(546);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(ret == nullptr);
}

BOOST_AUTO_TEST_CASE(InstanceName3_test)
{
    cNamelist list;
    list.AppendName("Thor");
    list.AppendName("Loki");
    list.AppendName("Heimdall");
    BOOST_TEST(list.Length() == 3);
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.InstanceName(1);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(*ret == "Loki");
}

BOOST_AUTO_TEST_CASE(AppendName_tests)
{
    cNamelist list;

    BOOST_TEST(list.Length() == 0);
    ////////////////////////// Test Subject //////////////////////////////
    list.AppendName("Thor");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 1);
    {
        auto rc = list.json();
        std::string expected = R"("namelist": ["Thor"])";
        BOOST_TEST(rc == expected);
    }

    ////////////////////////// Test Subject //////////////////////////////
    list.AppendName("Loki");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 2);
    {
        auto rc = list.json();
        std::string expected = R"("namelist": ["Thor","Loki"])";
        BOOST_TEST(rc == expected);
    }

    ////////////////////////// Test Subject //////////////////////////////
    list.AppendName("Heimdall");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 3);
    {
        auto rc = list.json();
        std::string expected = R"("namelist": ["Thor","Loki","Heimdall"])";
        BOOST_TEST(rc == expected);
    }

    ////////////////////////// Test Subject //////////////////////////////
    list.AppendName(nullptr);
    ////////////////////////// Test Subject //////////////////////////////
    {
        auto rc = list.json();
        std::string expected = R"("namelist": ["Thor","Loki","Heimdall",""])";
        BOOST_TEST(rc == expected);
        BOOST_TEST(true, "Cool, didn't crash");
    }

    {
        char buffer[10]{};
        ////////////////////////// Test Subject //////////////////////////////
        list.AppendName(buffer);
        ////////////////////////// Test Subject //////////////////////////////
        auto rc = list.json();
        std::string expected = R"("namelist": ["Thor","Loki","Heimdall","",""])";
        BOOST_TEST(rc == expected);
        BOOST_TEST(true, "Cool, didn't crash");
    }
}

BOOST_AUTO_TEST_CASE(dAppendName_tests)
{
    cNamelist list;

    BOOST_TEST(list.Length() == 0);
    ////////////////////////// Test Subject //////////////////////////////
    list.dAppendName("Thor");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 1);
    {
        auto rc = list.json();
        std::string expected = R"("namelist": ["Thor"])";
        BOOST_TEST(rc == expected);
    }

    ////////////////////////// Test Subject //////////////////////////////
    list.dAppendName("Loki");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 2);
    {
        auto rc = list.json();
        std::string expected = R"("namelist": ["Thor","Loki"])";
        BOOST_TEST(rc == expected);
    }

    ////////////////////////// Test Subject //////////////////////////////
    list.dAppendName("Heimdall");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 3);
    {
        auto rc = list.json();
        std::string expected = R"("namelist": ["Thor","Loki","Heimdall"])";
        BOOST_TEST(rc == expected);
    }

    ////////////////////////// Test Subject //////////////////////////////
    list.dAppendName(nullptr);
    ////////////////////////// Test Subject //////////////////////////////
    {
        auto rc = list.json();
        std::string expected = R"("namelist": ["Thor","Loki","Heimdall",""])";
        BOOST_TEST(rc == expected);
        BOOST_TEST(true, "Cool, didn't crash");
    }

    {
        char buffer[10]{};
        ////////////////////////// Test Subject //////////////////////////////
        list.dAppendName(buffer);
        ////////////////////////// Test Subject //////////////////////////////
        auto rc = list.json();
        std::string expected = R"("namelist": ["Thor","Loki","Heimdall","",""])";
        BOOST_TEST(rc == expected);
        BOOST_TEST(true, "Cool, didn't crash");
    }
}

BOOST_AUTO_TEST_CASE(PrependName1_tests)
{
    cNamelist list;

    BOOST_TEST(list.Length() == 0);
    ////////////////////////// Test Subject //////////////////////////////
    list.PrependName("Thor");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 1);
    {
        auto rc = list.json();
        std::string expected = R"("namelist": ["Thor"])";
        BOOST_TEST(rc == expected);
    }

    ////////////////////////// Test Subject //////////////////////////////
    list.PrependName("Loki");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 2);
    {
        auto rc = list.json();
        std::string expected = R"("namelist": ["Loki","Thor"])";
        BOOST_TEST(rc == expected);
    }

    ////////////////////////// Test Subject //////////////////////////////
    list.PrependName("Heimdall");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 3);
    {
        auto rc = list.json();
        std::string expected = R"("namelist": ["Heimdall","Loki","Thor"])";
        BOOST_TEST(rc == expected);
    }

    ////////////////////////// Test Subject //////////////////////////////
    list.PrependName(nullptr);
    ////////////////////////// Test Subject //////////////////////////////
    {
        auto rc = list.json();
        std::string expected = R"("namelist": ["","Heimdall","Loki","Thor"])";
        BOOST_TEST(rc == expected);
        BOOST_TEST(true, "Cool, didn't crash");
    }

    {
        char buffer[10]{};
        ////////////////////////// Test Subject //////////////////////////////
        list.PrependName(buffer);
        ////////////////////////// Test Subject //////////////////////////////
        auto rc = list.json();
        std::string expected = R"("namelist": ["","","Heimdall","Loki","Thor"])";
        BOOST_TEST(rc == expected);
        BOOST_TEST(true, "Cool, didn't crash");
    }
}

BOOST_AUTO_TEST_CASE(PrependName2_tests)
{
    cNamelist list;

    list.AppendName("Thor");
    list.AppendName("Loki");
    list.AppendName("Heimdall");
    BOOST_TEST(list.Length() == 3);
    {
        std::string expected = R"("namelist": ["Thor","Loki","Heimdall"])";
        auto rc = list.json();
        BOOST_TEST(rc == expected);
    }
    ////////////////////////// Test Subject //////////////////////////////
    list.PrependName("Tyr");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 4);
    {
        std::string expected = R"("namelist": ["Tyr","Thor","Loki","Heimdall"])";
        auto rc = list.json();
        BOOST_TEST(rc == expected);
    }
}

BOOST_AUTO_TEST_CASE(InsertName1_tests)
{
    cNamelist list;

    ////////////////////////// Test Subject //////////////////////////////
    list.InsertName("Loki", 0);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 1);
    {
        std::string expected = R"("namelist": ["Loki"])";
        auto rc = list.json();
        BOOST_TEST(rc == expected);
    }
}

BOOST_AUTO_TEST_CASE(InsertName2_tests)
{
    cNamelist list;

    ////////////////////////// Test Subject //////////////////////////////
    list.InsertName("Loki", 10);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 11);
    std::string expected = R"("namelist": ["","","","","","","","","","","Loki"])";
    auto rc = list.json();
    BOOST_TEST(rc == expected);
}

BOOST_AUTO_TEST_CASE(InsertName3_tests)
{
    cNamelist list;
    list.AppendName("Thor");
    ////////////////////////// Test Subject //////////////////////////////
    list.InsertName("Loki", 10);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 11);
    std::string expected = R"("namelist": ["Thor","","","","","","","","","","Loki"])";
    auto rc = list.json();
    BOOST_TEST(rc == expected);
}

BOOST_AUTO_TEST_CASE(InsertName4_tests)
{
    cNamelist list;
    list.AppendName("Thor");
    list.AppendName("Heimdall");
    list.AppendName("Tyr");
    BOOST_TEST(list.Length() == 3);
    {
        std::string expected = R"("namelist": ["Thor","Heimdall","Tyr"])";
        auto rc = list.json();
        BOOST_TEST(rc == expected);
    }
    ////////////////////////// Test Subject //////////////////////////////
    list.InsertName("Loki", 2);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 4);
    {
        std::string expected = R"("namelist": ["Thor","Heimdall","Loki","Tyr"])";
        auto rc = list.json();
        BOOST_TEST(rc == expected);
    }
}

BOOST_AUTO_TEST_CASE(Duplicate1_test)
{
    cNamelist list;
    BOOST_TEST(list.Length() == 0);
    ////////////////////////// Test Subject //////////////////////////////
    std::unique_ptr<cNamelist> duplicate{list.Duplicate()};
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(duplicate.get());
    BOOST_TEST(duplicate->Length() == 0);
}

BOOST_AUTO_TEST_CASE(Duplicate2_test)
{
    cNamelist list;
    list.AppendName("Thor");
    list.AppendName("Loki");
    list.AppendName("Tyr");
    BOOST_TEST(list.Length() == 3);
    ////////////////////////// Test Subject //////////////////////////////
    std::unique_ptr<cNamelist> duplicate{list.Duplicate()};
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(duplicate.get());
    BOOST_TEST(duplicate->Length() == 3);

    {
        std::string expected = R"("namelist": ["Thor","Loki","Tyr"])";
        auto rc = duplicate->json();
        BOOST_TEST(rc == expected);
    }
}

BOOST_AUTO_TEST_CASE(IsNameIdx_test)
{
    cNamelist list;
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.IsNameIdx("Thor") == -1);
    ////////////////////////// Test Subject //////////////////////////////
}

BOOST_AUTO_TEST_CASE(IsNameIdx2_test)
{
    cNamelist list;
    list.AppendName("Thor");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.IsNameIdx("Thor") == 0);
    ////////////////////////// Test Subject //////////////////////////////

    list.AppendName("Loki");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.IsNameIdx("Loki") == 1);
    ////////////////////////// Test Subject //////////////////////////////
}

BOOST_AUTO_TEST_CASE(IsNameIdx3_test)
{
    cNamelist list;
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.IsNameIdx(nullptr) == -1);
    ////////////////////////// Test Subject //////////////////////////////
}

BOOST_AUTO_TEST_CASE(IsNameRawIdx1_test)
{
    cNamelist list;
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.IsNameRawIdx("Thor");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(ret == -1);
}

BOOST_AUTO_TEST_CASE(IsNameRawIdx2_test)
{
    cNamelist list;
    list.AppendName("Thor");
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.IsNameRawIdx("Thor");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(ret == 0);
}

BOOST_AUTO_TEST_CASE(IsNameRawIdx3_test)
{
    cNamelist list;
    list.AppendName("THOR");
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.IsNameRawIdx("thor");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(ret == 0);
}

BOOST_AUTO_TEST_CASE(IsNameRawIdx4_test)
{
    cNamelist list;
    list.AppendName("Thor");
    list.AppendName("Loki");
    list.AppendName("Heimdall");
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.IsNameRawIdx("Loki");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(ret == 1);
}

BOOST_AUTO_TEST_CASE(IsNameRaw1_test)
{
    cNamelist list;
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.IsNameRaw("Loki");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(ret == nullptr);
}

BOOST_AUTO_TEST_CASE(IsNameRaw2_test)
{
    cNamelist list;
    list.AppendName("Thor");
    list.AppendName("Loki");
    list.AppendName("Heimdall");
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.IsNameRaw("Loki        ");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_REQUIRE_NE(ret, nullptr);
    BOOST_TEST(std::string(ret) == "        ");
}

BOOST_AUTO_TEST_CASE(IsNameRaw3_test)
{
    cNamelist list;
    list.AppendName("Thor");
    list.AppendName("LOKI");
    list.AppendName("Heimdall");
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.IsNameRaw("loki     ");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_REQUIRE_NE(ret, nullptr);
    BOOST_TEST(std::string(ret) == "     ");
}

BOOST_AUTO_TEST_CASE(IsName1_test)
{
    cNamelist list;
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.IsName("Loki");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(ret == nullptr);
}

BOOST_AUTO_TEST_CASE(IsName2_test)
{
    cNamelist list;
    list.AppendName("Thor");
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.IsName("Loki");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(ret == nullptr);
}

BOOST_AUTO_TEST_CASE(IsName3_test)
{
    cNamelist list;
    list.AppendName("Thor");
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.IsName(nullptr);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(ret == nullptr);
}

BOOST_AUTO_TEST_CASE(IsName4_test)
{
    cNamelist list;
    list.AppendName("Loki");
    list.AppendName("Thor");

    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.IsName("    Loki        ");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_REQUIRE_NE(ret, nullptr);
    BOOST_TEST(std::string(ret) == " ");
}

BOOST_AUTO_TEST_CASE(Length_test)
{
    cNamelist list;
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(list.Length() == 0);
    ////////////////////////// Test Subject //////////////////////////////
}

BOOST_AUTO_TEST_CASE(StrStrRaw1_test)
{
    cNamelist list;
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.StrStrRaw(nullptr);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(ret == nullptr);
}

BOOST_AUTO_TEST_CASE(StrStrRaw2_test)
{
    cNamelist list;
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.StrStrRaw("Homer");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(ret == nullptr);
}

BOOST_AUTO_TEST_CASE(StrStrRaw3_test)
{
    cNamelist list;
    list.AppendName("Thor");
    list.AppendName("Loki");
    list.AppendName("Tyr");
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.StrStrRaw("Homer");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(ret == nullptr);
}

BOOST_AUTO_TEST_CASE(StrStrRaw4_test)
{
    cNamelist list;
    list.AppendName("Thor");
    list.AppendName("Loki");
    list.AppendName("Homer");
    list.AppendName("Tyr");
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.StrStrRaw("Homer");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_REQUIRE_NE(ret, nullptr);
    BOOST_TEST(std::string(ret) == "Homer");
}

BOOST_AUTO_TEST_CASE(StrStrRaw5_test)
{
    cNamelist list;
    list.AppendName("Thor");
    list.AppendName("Loki");
    list.AppendName("Homer");
    list.AppendName("Tyr");
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.StrStrRaw("HOMER");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_REQUIRE_NE(ret, nullptr);
    BOOST_TEST(std::string(ret) == "HOMER");
}

BOOST_AUTO_TEST_CASE(StrStr1_test)
{
    cNamelist list;
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.StrStr(nullptr);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(ret == nullptr);
}

BOOST_AUTO_TEST_CASE(StrStr2_test)
{
    cNamelist list;
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.StrStr("Homer");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(ret == nullptr);
}

BOOST_AUTO_TEST_CASE(StrStr3_test)
{
    cNamelist list;
    list.AppendName("Thor");
    list.AppendName("Loki");
    list.AppendName("Tyr");
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.StrStr("Homer");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(ret == nullptr);
}

BOOST_AUTO_TEST_CASE(StrStr4_test)
{
    cNamelist list;
    list.AppendName("Thor");
    list.AppendName("Loki");
    list.AppendName("Homer");
    list.AppendName("Tyr");
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.StrStr("Homer");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_REQUIRE_NE(ret, nullptr);
    BOOST_TEST(std::string(ret) == "Homer");
}

BOOST_AUTO_TEST_CASE(StrStr5_test)
{
    cNamelist list;
    list.AppendName("Thor");
    list.AppendName("Loki");
    list.AppendName("Homer");
    list.AppendName("Tyr");
    ////////////////////////// Test Subject //////////////////////////////
    auto ret = list.StrStr("HOMER");
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_REQUIRE_NE(ret, nullptr);
    BOOST_TEST(std::string(ret) == "HOMER");
}

BOOST_AUTO_TEST_SUITE_END()
