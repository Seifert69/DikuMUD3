#include "FixtureBase.h"
#include "files.h"
#include "vme.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * files_tests - the string files of DIL's loadstr, savestr and delstr: load_string(),
 * save_string() and store_name_test().
 */
namespace
{
// A FixtureBase, as load_string() logs: a log file another fixture closed is no place for it
struct TmpDirFixture : public unit_tests::FixtureBase
{
    TmpDirFixture()
    {
        char tmpl[] = "/tmp/files_tests.XXXXXX";
        BOOST_REQUIRE(mkdtemp(tmpl));
        dir = std::string(tmpl) + "/";
    }

    ~TmpDirFixture() override { std::filesystem::remove_all(dir); }

    std::string read(const std::string &name) const
    {
        std::ifstream in(dir + name, std::ios::binary);
        return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
    }

    std::string dir;
};

int save(const std::string &filename, const char *text, const char *mode)
{
    std::string buf{text};
    char *p = buf.data();
    std::string opp{mode};
    return save_string(filename.c_str(), &p, opp.data());
}
} // namespace

BOOST_AUTO_TEST_SUITE(files_tests)

// "w" left the end of a longer text that was there (boards.zon rewrites its .idx files so)
BOOST_FIXTURE_TEST_CASE(write_replaces_a_longer_text, TmpDirFixture)
{
    BOOST_TEST(save(dir + "t", "a longer text", "w") == FILE_SAVED);
    BOOST_TEST(save(dir + "t", "short", "w") == FILE_SAVED);
    BOOST_TEST(read("t") == "short");

    BOOST_TEST(save(dir + "t", "+more", "a") == FILE_SAVED);
    BOOST_TEST(read("t") == "short+more");
}

// open() gives -1, not 0, when it fails: the write went to fd -1 and it reported FILE_SAVED
BOOST_FIXTURE_TEST_CASE(directory_is_not_written, TmpDirFixture)
{
    BOOST_TEST(save(dir, "x", "w") == FILE_NOT_CREATED);
    BOOST_TEST(save(dir + "missing/t", "x", "a") == FILE_NOT_CREATED);
}

// read() on a directory gives -1, which moved the write pointer far past the buffer
BOOST_FIXTURE_TEST_CASE(directory_is_not_loaded, TmpDirFixture)
{
    char *text = nullptr;
    BOOST_TEST(load_string(dir.c_str(), &text) == FILE_NOT_FOUND);
    BOOST_TEST(text == nullptr);
}

BOOST_FIXTURE_TEST_CASE(file_is_loaded, TmpDirFixture)
{
    std::ofstream(dir + "t") << "text";
    char *text = nullptr;
    BOOST_TEST(load_string((dir + "t").c_str(), &text) == FILE_LOADED);
    BOOST_TEST(std::string(text) == "text");
    free(text);
}

BOOST_AUTO_TEST_CASE(dot_names_are_not_files)
{
    for (const char *name : {".", "..", "", "a/b", "a b"})
    {
        std::string buf{name};
        BOOST_TEST(!store_name_test(buf.data()));
    }
    for (const char *name : {"a", "board.idx", "x..y", "...", "clan_1.rank"})
    {
        std::string buf{name};
        BOOST_TEST(store_name_test(buf.data()));
    }
}

BOOST_AUTO_TEST_SUITE_END()
