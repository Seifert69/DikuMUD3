#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
#include "../vme/include/vme.h"
#include "FixtureBase.h"
#include "OutputCapture.h"
#include "account.h"
#include "config.h"
#include "db.h"
#include "formatter.h"
#include "structs.h"
#include "utils.h"

#include <cstdarg>
#include <fstream>

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

struct AccountCPPFixture : public unit_tests::FixtureBase
{
    AccountCPPFixture()
        : FixtureBase()
    {
        chdir("../vme/src/bin");

        // Boot some global vars needed by these tests
        std::string config("../etc/server.cfg");
        g_cServerConfig.Boot(config.data());
        g_cServerConfig.enableAccounting();
        g_cAccountConfig.Boot();

        god = std::unique_ptr<pc_data>(dynamic_cast<pc_data *>(new_unit_data(UNIT_ST_PC)));
        god->names.AppendName("Thor");

        whom = std::unique_ptr<pc_data>(dynamic_cast<pc_data *>(new_unit_data(UNIT_ST_PC)));
        whom->names.AppendName("Bilbo");

        npc = std::unique_ptr<npc_data>(dynamic_cast<npc_data *>(new_unit_data(UNIT_ST_NPC)));
        npc->names.AppendName("Red Shirt 1");

        // Booting the server config and account config create log messsage so start with clean slate
        unit_tests::OutputCapture::Instance()->Clear();
    }

    ~AccountCPPFixture() = default;

    std::unique_ptr<pc_data> god{};
    std::unique_ptr<pc_data> whom{};
    std::unique_ptr<npc_data> npc{};
};

BOOST_FIXTURE_TEST_SUITE(Account_Public_Funcs_Suite, AccountCPPFixture)

BOOST_AUTO_TEST_SUITE(Account_Flatrate_Change_Suite)

#if 0
BOOST_AUTO_TEST_CASE(all_nulls_test)
{
    BOOST_TEST(false);
    BOOST_TEST_MESSAGE("The null test below should ideally work");
    //    BOOST_CHECK_NO_THROW(account_flatrate_change(nullptr, nullptr, 0));
    (void)0;
}
#endif

/**
 * Days = 20
 * Flatrate = now() + 10000
 */
BOOST_AUTO_TEST_CASE(days_gt_zero_flatrate_gt_now)
{
    sbit32 days = 20;
    time_t now = time(nullptr);
    whom->account.flatrate = now + 10000;

    const sbit32 expected_flatrate = whom->account.flatrate + (days * SECS_PER_REAL_DAY);

    ////////////////////////// Test Subject //////////////////////////////
    account_flatrate_change(god.get(), whom.get(), days);
    ////////////////////////// Test Subject //////////////////////////////

    BOOST_TEST(whom->account.flatrate == expected_flatrate);

    const auto &slog = unit_tests::OutputCapture::Instance()->getSLogData();
    BOOST_TEST(slog.m_level == LOG_ALL);
    BOOST_TEST(slog.m_wiz_inv_level == 255);
    BOOST_TEST(slog.m_fmt == "%s change flatrate with %d on account %s.");
    BOOST_TEST(slog.m_message == "Thor change flatrate with 20 on account Bilbo.");

    const auto &account_log = unit_tests::OutputCapture::Instance()->getAccountLogData();
    BOOST_TEST(account_log.m_action == 'F');
    BOOST_TEST(account_log.m_amount == days);

    const auto &send_to_char = unit_tests::OutputCapture::Instance()->getSendToCharData();
    BOOST_TEST(send_to_char.m_message == "\n\rAdding 20 days to the flatrate.\n\r\n\r");
}

/**
 * Days = 20
 * Flatrate = now() - 10000
 */
BOOST_AUTO_TEST_CASE(days_gt_zero_flatrate_lt_now_plus_add)
{
    sbit32 days = 20;
    time_t now = time(nullptr);
    whom->account.flatrate = now - 10000;
    const sbit32 expected_flatrate = now + (days * SECS_PER_REAL_DAY);

    ////////////////////////// Test Subject //////////////////////////////
    account_flatrate_change(god.get(), whom.get(), days);
    ////////////////////////// Test Subject //////////////////////////////

    BOOST_TEST(whom->account.flatrate == expected_flatrate);

    const auto &slog = unit_tests::OutputCapture::Instance()->getSLogData();
    BOOST_TEST(slog.m_level == LOG_ALL);
    BOOST_TEST(slog.m_wiz_inv_level == 255);
    BOOST_TEST(slog.m_fmt == "%s change flatrate with %d on account %s.");
    BOOST_TEST(slog.m_message == "Thor change flatrate with 20 on account Bilbo.");

    const auto &account_log = unit_tests::OutputCapture::Instance()->getAccountLogData();
    BOOST_TEST(account_log.m_action == 'F');
    BOOST_TEST(account_log.m_amount == days);

    const auto &send_to_char = unit_tests::OutputCapture::Instance()->getSendToCharData();
    BOOST_TEST(send_to_char.m_message == "\n\rSetting flatrate to 20 days.\n\r\n\r");
}

/**
 * Days = 0
 * Flatrate = now() + 10000
 */
BOOST_AUTO_TEST_CASE(days_eq_zero_flatrate_gt_now)
{
    sbit32 days = 0;
    time_t now = time(nullptr);
    whom->account.flatrate = now + 10000;
    const sbit32 expected_flatrate = whom->account.flatrate + (days * SECS_PER_REAL_DAY);

    ////////////////////////// Test Subject //////////////////////////////
    account_flatrate_change(god.get(), whom.get(), days);
    ////////////////////////// Test Subject //////////////////////////////

    BOOST_TEST(whom->account.flatrate == expected_flatrate);

    const auto &slog = unit_tests::OutputCapture::Instance()->getSLogData();
    BOOST_TEST(slog.m_level == LOG_ALL);
    BOOST_TEST(slog.m_wiz_inv_level == 255);
    BOOST_TEST(slog.m_fmt == "%s change flatrate with %d on account %s.");
    BOOST_TEST(slog.m_message == "Thor change flatrate with 0 on account Bilbo.");

    const auto &account_log = unit_tests::OutputCapture::Instance()->getAccountLogData();
    BOOST_TEST(account_log.m_action == 'F');
    BOOST_TEST(account_log.m_amount == days);

    const auto &send_to_char = unit_tests::OutputCapture::Instance()->getSendToCharData();
    BOOST_TEST(send_to_char.m_message == "\n\rSubtracting 0 days from the flatrate.\n\r\n\r");
}

/**
 * Days = 0
 * Flatrate = now() - 10000
 */
BOOST_AUTO_TEST_CASE(days_eq_zero_flatrate_lt_now)
{
    sbit32 days = 0;
    time_t now = time(nullptr);
    whom->account.flatrate = now - 10000;
    const sbit32 expected_flatrate = 0;

    ////////////////////////// Test Subject //////////////////////////////
    account_flatrate_change(god.get(), whom.get(), days);
    ////////////////////////// Test Subject //////////////////////////////

    BOOST_TEST(whom->account.flatrate == expected_flatrate);

    const auto &slog = unit_tests::OutputCapture::Instance()->getSLogData();
    BOOST_TEST(slog.m_level == LOG_ALL);
    BOOST_TEST(slog.m_wiz_inv_level == 255);
    BOOST_TEST(slog.m_fmt == "%s change flatrate with %d on account %s.");
    BOOST_TEST(slog.m_message == "Thor change flatrate with 0 on account Bilbo.");

    const auto &account_log = unit_tests::OutputCapture::Instance()->getAccountLogData();
    BOOST_TEST(account_log.m_action == 'F');
    BOOST_TEST(account_log.m_amount == days);

    const auto &send_to_char = unit_tests::OutputCapture::Instance()->getSendToCharData();
    BOOST_TEST(send_to_char.m_message == "\n\rDisabling flatrate, enabling measure rate.\n\r\n\r");
}

/**
 * Days = -7
 * Flatrate = now() + 10000;
 */
BOOST_AUTO_TEST_CASE(days_lt_zero_flatrate_gt_now)
{
    sbit32 days = -7;
    time_t now = time(nullptr);
    whom->account.flatrate = now + 10000;
    const sbit32 expected_flatrate = 0;

    ////////////////////////// Test Subject //////////////////////////////
    account_flatrate_change(god.get(), whom.get(), days);
    ////////////////////////// Test Subject //////////////////////////////

    BOOST_TEST(whom->account.flatrate == expected_flatrate);
    const auto &slog = unit_tests::OutputCapture::Instance()->getSLogData();
    BOOST_TEST(slog.m_level == LOG_ALL);
    BOOST_TEST(slog.m_wiz_inv_level == 255);
    BOOST_TEST(slog.m_fmt == "%s change flatrate with %d on account %s.");
    BOOST_TEST(slog.m_message == "Thor change flatrate with -7 on account Bilbo.");

    const auto &account_log = unit_tests::OutputCapture::Instance()->getAccountLogData();
    BOOST_TEST(account_log.m_action == 'F');
    BOOST_TEST(account_log.m_amount == days);

    const auto &send_to_char = unit_tests::OutputCapture::Instance()->getSendToCharData();
    BOOST_TEST(send_to_char.m_message == "\n\rDisabling flatrate, enabling measure rate.\n\r\n\r");
}

/**
 * Days = -7
 * Flatrate = now() + ((days * -1) + 1) * SECS_PER_REAL_DAY
 */
BOOST_AUTO_TEST_CASE(days_lt_zero_flatrate_lt_now)
{
    sbit32 days = -7;
    time_t now = time(nullptr);
    whom->account.flatrate = now + ((days * -1) + 1) * SECS_PER_REAL_DAY;
    const sbit32 expected_flatrate = whom->account.flatrate + (days * SECS_PER_REAL_DAY);

    ////////////////////////// Test Subject //////////////////////////////
    account_flatrate_change(god.get(), whom.get(), days);
    ////////////////////////// Test Subject //////////////////////////////

    BOOST_TEST(whom->account.flatrate == expected_flatrate);

    const auto &slog = unit_tests::OutputCapture::Instance()->getSLogData();
    BOOST_TEST(slog.m_level == LOG_ALL);
    BOOST_TEST(slog.m_wiz_inv_level == 255);
    BOOST_TEST(slog.m_fmt == "%s change flatrate with %d on account %s.");
    BOOST_TEST(slog.m_message == "Thor change flatrate with -7 on account Bilbo.");

    const auto &account_log = unit_tests::OutputCapture::Instance()->getAccountLogData();
    BOOST_TEST(account_log.m_action == 'F');
    BOOST_TEST(account_log.m_amount == days);

    const auto &send_to_char = unit_tests::OutputCapture::Instance()->getSendToCharData();
    BOOST_TEST(send_to_char.m_message == "\n\rSubtracting -7 days from the flatrate.\n\r\n\r");
}

BOOST_AUTO_TEST_SUITE_END() // Account_Flatrate_Change_Suite
//===========================================================================================

BOOST_FIXTURE_TEST_SUITE(Account_Cclog_Suite, AccountCPPFixture)

/**
 * Happy path call
 */
BOOST_AUTO_TEST_CASE(account_cclog_test)
{
    const std::string coin_name{"dollar"};
    std::string logfile = g_cServerConfig.getFileInLogDir(CREDITFILE_LOG);
    unlink(logfile.c_str());

    int amount = 123456789;

    ////////////////////////// Test Subject //////////////////////////////
    account_cclog(whom.get(), amount);
    ////////////////////////// Test Subject //////////////////////////////

    char buf[1024]{};
    snprintf(buf, sizeof(buf), "%-16s %6.2f %s\n", UNIT_NAME(whom), ((float)amount) / 100.0, g_cAccountConfig.m_pCoinName);

    std::ifstream in(logfile.c_str(), std::ios::binary);
    BOOST_TEST(in.good());
    std::string file((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    BOOST_TEST(file == std::string(buf));
    unlink(logfile.c_str());
}

/**
 * Use name longer than 16 chars
 */
BOOST_AUTO_TEST_CASE(account_cclog_long_name_test)
{
    const std::string coin_name{"dollar"};
    std::string logfile = g_cServerConfig.getFileInLogDir(CREDITFILE_LOG);
    unlink(logfile.c_str());

    int amount = 123456789;
    whom->names.InsertName("ThisIsAReallyLongNameMoreThanItShouldBe", 0);

    ////////////////////////// Test Subject //////////////////////////////
    account_cclog(whom.get(), amount);
    ////////////////////////// Test Subject //////////////////////////////

    char buf[1024]{};
    snprintf(buf, sizeof(buf), "%-16s %6.2f %s\n", UNIT_NAME(whom), ((float)amount) / 100.0, g_cAccountConfig.m_pCoinName);

    std::ifstream in(logfile.c_str(), std::ios::binary);
    BOOST_TEST(in.good());
    std::string file((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    BOOST_TEST(file == std::string(buf));
    unlink(logfile.c_str());
}

/**
 * Large amount value
 */
BOOST_AUTO_TEST_CASE(account_cclog_long_amount_test)
{
    const std::string coin_name{"dollar"};
    std::string logfile = g_cServerConfig.getFileInLogDir(CREDITFILE_LOG);
    unlink(logfile.c_str());

    int amount = INT_MAX;

    ////////////////////////// Test Subject //////////////////////////////
    account_cclog(whom.get(), amount);
    ////////////////////////// Test Subject //////////////////////////////

    char buf[1024]{};
    snprintf(buf, sizeof(buf), "%-16s %6.2f %s\n", UNIT_NAME(whom), ((float)amount) / 100.0, g_cAccountConfig.m_pCoinName);

    std::ifstream in(logfile.c_str(), std::ios::binary);
    BOOST_TEST(in.good());
    std::string file((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    BOOST_TEST(file == std::string(buf));
    unlink(logfile.c_str());
}

/**
 * Name > 16 chars and large amount value
 */
BOOST_AUTO_TEST_CASE(account_cclog_long_name_and_amount_test)
{
    const std::string coin_name{"dollar"};
    std::string logfile = g_cServerConfig.getFileInLogDir(CREDITFILE_LOG);
    unlink(logfile.c_str());

    int amount = INT_MAX;
    whom->names.InsertName("ThisIsAReallyLongNameMoreThanItShouldBe", 0);

    ////////////////////////// Test Subject //////////////////////////////
    account_cclog(whom.get(), amount);
    ////////////////////////// Test Subject //////////////////////////////

    char buf[1024]{};
    snprintf(buf, sizeof(buf), "%-16s %6.2f %s\n", UNIT_NAME(whom), ((float)amount) / 100.0, g_cAccountConfig.m_pCoinName);

    std::ifstream in(logfile.c_str(), std::ios::binary);
    BOOST_TEST(in.good());
    std::string file((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    BOOST_TEST(file == std::string(buf));
    unlink(logfile.c_str());
}

/**
 * Large negative amount
 */
BOOST_AUTO_TEST_CASE(account_cclog_neg_amount_test)
{
    const std::string coin_name{"dollar"};
    std::string logfile = g_cServerConfig.getFileInLogDir(CREDITFILE_LOG);
    unlink(logfile.c_str());

    int amount = INT_MIN;

    ////////////////////////// Test Subject //////////////////////////////
    account_cclog(whom.get(), amount);
    ////////////////////////// Test Subject //////////////////////////////

    char buf[1024]{};
    snprintf(buf, sizeof(buf), "%-16s %6.2f %s\n", UNIT_NAME(whom), ((float)amount) / 100.0, g_cAccountConfig.m_pCoinName);

    std::ifstream in(logfile.c_str(), std::ios::binary);
    BOOST_TEST(in.good());
    std::string file((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    BOOST_TEST(file == std::string(buf));
    unlink(logfile.c_str());
}

BOOST_AUTO_TEST_CASE(account_cclog_no_name_test)
{
    const std::string coin_name{"dollar"};
    std::string logfile = g_cServerConfig.getFileInLogDir(CREDITFILE_LOG);
    unlink(logfile.c_str());

    int amount = INT_MIN;
    whom->names.Free();

    ////////////////////////// Test Subject //////////////////////////////
    account_cclog(whom.get(), amount);
    ////////////////////////// Test Subject //////////////////////////////

    char buf[1024]{};
    snprintf(buf, sizeof(buf), "%-16s %6.2f %s\n", UNIT_NAME(whom), ((float)amount) / 100.0, g_cAccountConfig.m_pCoinName);

    std::ifstream in(logfile.c_str(), std::ios::binary);
    BOOST_TEST(in.good());
    std::string file((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    BOOST_TEST(file == std::string(buf));
    unlink(logfile.c_str());
}

BOOST_AUTO_TEST_SUITE_END() // Account_Cclog_Suite

//===========================================================================================

BOOST_FIXTURE_TEST_SUITE(Account_Insert_Suite, AccountCPPFixture)
BOOST_AUTO_TEST_CASE(account_insert_test)
{
    float credit = 3.14f;
    ubit32 total_credit = 4876;

    int amount = 5546;

    whom->account.credit = credit;
    whom->account.total_credit = total_credit;

    ////////////////////////// Test Subject //////////////////////////////
    account_insert(god.get(), whom.get(), amount);
    ////////////////////////// Test Subject //////////////////////////////

    BOOST_TEST(whom->account.credit == credit + amount);
    BOOST_TEST(whom->account.total_credit == total_credit + amount);

    const auto &slog = unit_tests::OutputCapture::Instance()->getSLogData();
    BOOST_TEST(slog.m_level == LOG_ALL);
    BOOST_TEST(slog.m_wiz_inv_level == 255);
    BOOST_TEST(slog.m_fmt == "%s inserted %d on account %s.");
    BOOST_TEST(slog.m_message == "Thor inserted 5546 on account Bilbo.");

    const auto &account_log = unit_tests::OutputCapture::Instance()->getAccountLogData();
    BOOST_TEST(account_log.m_action == 'I');
    BOOST_TEST(account_log.m_amount == amount);
}
BOOST_AUTO_TEST_SUITE_END()
//===========================================================================================

BOOST_FIXTURE_TEST_SUITE(Account_Withdraw_Suite, AccountCPPFixture)

std::vector<ubit32> withdrawal_amounts{274, 2744, 27447};
BOOST_DATA_TEST_CASE(account_withdraw_test, withdrawal_amounts, amount)
{
    float credit = 375.14f;
    ubit32 total_credit = 4876;

    whom->account.credit = credit;
    whom->account.total_credit = total_credit;

    ////////////////////////// Test Subject //////////////////////////////
    account_withdraw(god.get(), whom.get(), amount);
    ////////////////////////// Test Subject //////////////////////////////

    BOOST_TEST(whom->account.credit == credit - amount);
    ubit32 expected_total_credit = total_credit - amount;
    if (amount > total_credit)
    {
        expected_total_credit = 0;
    }
    BOOST_TEST(whom->account.total_credit == expected_total_credit);

    auto expected = diku::format_to_str("%s withdrew %d from account %s.", god->names.Name(), amount, whom->names.Name());

    const auto &slog = unit_tests::OutputCapture::Instance()->getSLogData();
    BOOST_TEST(slog.m_level == LOG_ALL);
    BOOST_TEST(slog.m_wiz_inv_level == 255);
    BOOST_TEST(slog.m_fmt == "%s withdrew %d from account %s.");
    BOOST_TEST(slog.m_message == expected);

    const auto &account_log = unit_tests::OutputCapture::Instance()->getAccountLogData();
    BOOST_TEST(account_log.m_action == 'W');
    BOOST_TEST(account_log.m_amount == amount);
}
BOOST_AUTO_TEST_SUITE_END() // Account_Withdraw_Suite
//===========================================================================================

BOOST_FIXTURE_TEST_SUITE(Account_Global_Stat_Suite, AccountCPPFixture)

BOOST_AUTO_TEST_CASE(account_global_stat_disabled_test)
{
    g_cServerConfig.disableAccounting();

    ////////////////////////// Test Subject //////////////////////////////
    account_global_stat(whom.get());
    ////////////////////////// Test Subject //////////////////////////////

    const auto &send_to_char = unit_tests::OutputCapture::Instance()->getSendToCharDataMulti();
    BOOST_TEST(send_to_char.empty());

    const auto &page_string = unit_tests::OutputCapture::Instance()->getPageStringDataMulti();
    BOOST_TEST(page_string.empty());
}

BOOST_AUTO_TEST_CASE(account_global_stat_test)
{
    ////////////////////////// Test Subject //////////////////////////////
    account_global_stat(whom.get());
    ////////////////////////// Test Subject //////////////////////////////

    std::string expected("\n\rAccounting mode:\n\r"
                         "  Free from level : 200\n\r"
                         "  Currency Name   : dollar\n\r"
                         "  Default limit   : 0.00\n\r"
                         "  Default start   : 0.00\n\r\n\r");

    const auto &send_to_char = unit_tests::OutputCapture::Instance()->getSendToCharData();
    BOOST_TEST(send_to_char.m_message == expected);

    std::string expected_ps("    Time    Sun  Mon  Tir  Wed  Thu  Fri  Sat\n\r"
                            "   0-  14    0    0    0    0    0    0    0\n\r"
                            "  15-  29    0    0    0    0    0    0    0\n\r"
                            "  30-  44    0    0    0    0    0    0    0\n\r"
                            "  45-  59    0    0    0    0    0    0    0\n\r"
                            " 100- 114    0    0    0    0    0    0    0\n\r"
                            " 115- 129    0    0    0    0    0    0    0\n\r"
                            " 130- 144    0    0    0    0    0    0    0\n\r"
                            " 145- 159    0    0    0    0    0    0    0\n\r"
                            " 200- 214    0    0    0    0    0    0    0\n\r"
                            " 215- 229    0    0    0    0    0    0    0\n\r"
                            " 230- 244    0    0    0    0    0    0    0\n\r"
                            " 245- 259    0    0    0    0    0    0    0\n\r"
                            " 300- 314    0    0    0    0    0    0    0\n\r"
                            " 315- 329    0    0    0    0    0    0    0\n\r"
                            " 330- 344    0    0    0    0    0    0    0\n\r"
                            " 345- 359    0    0    0    0    0    0    0\n\r"
                            " 400- 414    0    0    0    0    0    0    0\n\r"
                            " 415- 429    0    0    0    0    0    0    0\n\r"
                            " 430- 444    0    0    0    0    0    0    0\n\r"
                            " 445- 459    0    0    0    0    0    0    0\n\r"
                            " 500- 514    0    0    0    0    0    0    0\n\r"
                            " 515- 529    0    0    0    0    0    0    0\n\r"
                            " 530- 544    0    0    0    0    0    0    0\n\r"
                            " 545- 559    0    0    0    0    0    0    0\n\r"
                            " 600- 614    0    0    0    0    0    0    0\n\r"
                            " 615- 629    0    0    0    0    0    0    0\n\r"
                            " 630- 644    0    0    0    0    0    0    0\n\r"
                            " 645- 659    0    0    0    0    0    0    0\n\r"
                            " 700- 714    0    0    0    0    0    0    0\n\r"
                            " 715- 729    0    0    0    0    0    0    0\n\r"
                            " 730- 744    0    0    0    0    0    0    0\n\r"
                            " 745- 759    0    0    0    0    0    0    0\n\r"
                            " 800- 814    0    0    0    0    0    0    0\n\r"
                            " 815- 829    0    0    0    0    0    0    0\n\r"
                            " 830- 844    0    0    0    0    0    0    0\n\r"
                            " 845- 859    0    0    0    0    0    0    0\n\r"
                            " 900- 914    0    0    0    0    0    0    0\n\r"
                            " 915- 929    0    0    0    0    0    0    0\n\r"
                            " 930- 944    0    0    0    0    0    0    0\n\r"
                            " 945- 959    0    0    0    0    0    0    0\n\r"
                            "1000-1014    0    0    0    0    0    0    0\n\r"
                            "1015-1029    0    0    0    0    0    0    0\n\r"
                            "1030-1044    0    0    0    0    0    0    0\n\r"
                            "1045-1059    0    0    0    0    0    0    0\n\r"
                            "1100-1114    0    0    0    0    0    0    0\n\r"
                            "1115-1129    0    0    0    0    0    0    0\n\r"
                            "1130-1144    0    0    0    0    0    0    0\n\r"
                            "1145-1159    0    0    0    0    0    0    0\n\r"
                            "1200-1214    0    0    0    0    0    0    0\n\r"
                            "1215-1229    0    0    0    0    0    0    0\n\r"
                            "1230-1244    0    0    0    0    0    0    0\n\r"
                            "1245-1259    0    0    0    0    0    0    0\n\r"
                            "1300-1314    0    0    0    0    0    0    0\n\r"
                            "1315-1329    0    0    0    0    0    0    0\n\r"
                            "1330-1344    0    0    0    0    0    0    0\n\r"
                            "1345-1359    0    0    0    0    0    0    0\n\r"
                            "1400-1414    0    0    0    0    0    0    0\n\r"
                            "1415-1429    0    0    0    0    0    0    0\n\r"
                            "1430-1444    0    0    0    0    0    0    0\n\r"
                            "1445-1459    0    0    0    0    0    0    0\n\r"
                            "1500-1514    0    0    0    0    0    0    0\n\r"
                            "1515-1529    0    0    0    0    0    0    0\n\r"
                            "1530-1544    0    0    0    0    0    0    0\n\r"
                            "1545-1559    0    0    0    0    0    0    0\n\r"
                            "1600-1614    0    0    0    0    0    0    0\n\r"
                            "1615-1629    0    0    0    0    0    0    0\n\r"
                            "1630-1644    0    0    0    0    0    0    0\n\r"
                            "1645-1659    0    0    0    0    0    0    0\n\r"
                            "1700-1714    0    0    0    0    0    0    0\n\r"
                            "1715-1729    0    0    0    0    0    0    0\n\r"
                            "1730-1744    0    0    0    0    0    0    0\n\r"
                            "1745-1759    0    0    0    0    0    0    0\n\r"
                            "1800-1814    0    0    0    0    0    0    0\n\r"
                            "1815-1829    0    0    0    0    0    0    0\n\r"
                            "1830-1844    0    0    0    0    0    0    0\n\r"
                            "1845-1859    0    0    0    0    0    0    0\n\r"
                            "1900-1914    0    0    0    0    0    0    0\n\r"
                            "1915-1929    0    0    0    0    0    0    0\n\r"
                            "1930-1944    0    0    0    0    0    0    0\n\r"
                            "1945-1959    0    0    0    0    0    0    0\n\r"
                            "2000-2014    0    0    0    0    0    0    0\n\r"
                            "2015-2029    0    0    0    0    0    0    0\n\r"
                            "2030-2044    0    0    0    0    0    0    0\n\r"
                            "2045-2059    0    0    0    0    0    0    0\n\r"
                            "2100-2114    0    0    0    0    0    0    0\n\r"
                            "2115-2129    0    0    0    0    0    0    0\n\r"
                            "2130-2144    0    0    0    0    0    0    0\n\r"
                            "2145-2159    0    0    0    0    0    0    0\n\r"
                            "2200-2214    0    0    0    0    0    0    0\n\r"
                            "2215-2229    0    0    0    0    0    0    0\n\r"
                            "2230-2244    0    0    0    0    0    0    0\n\r"
                            "2245-2259    0    0    0    0    0    0    0\n\r"
                            "2300-2314    0    0    0    0    0    0    0\n\r"
                            "2315-2329    0    0    0    0    0    0    0\n\r"
                            "2330-2344    0    0    0    0    0    0    0\n\r"
                            "2345-2359    0    0    0    0    0    0    0\n\r");
    const auto &page_string = unit_tests::OutputCapture::Instance()->getPageStringData();
    BOOST_TEST(page_string.m_message == expected_ps);
}

BOOST_AUTO_TEST_SUITE_END() // Account_Global_Stat_Suite
//===========================================================================================

BOOST_FIXTURE_TEST_SUITE(Account_Local_Stat_Suite, AccountCPPFixture)
BOOST_AUTO_TEST_CASE(account_local_stat_disabled_test)
{
    g_cServerConfig.disableAccounting();

    ////////////////////////// Test Subject //////////////////////////////
    account_local_stat(god.get(), whom.get());
    ////////////////////////// Test Subject //////////////////////////////

    const auto &send_to_char = unit_tests::OutputCapture::Instance()->getSendToCharData();
    BOOST_TEST(send_to_char.m_message == "Game is not in accounting mode.\n\r");
}

BOOST_AUTO_TEST_CASE(account_local_stat_npc_test)
{
    ////////////////////////// Test Subject //////////////////////////////
    account_local_stat(god.get(), npc.get());
    ////////////////////////// Test Subject //////////////////////////////

    const auto &send_to_char = unit_tests::OutputCapture::Instance()->getSendToCharData();
    BOOST_TEST(send_to_char.m_message == "Only players have accounts.\n\r");
}

BOOST_AUTO_TEST_CASE(account_local_stat_broke_test)
{
    ////////////////////////// Test Subject //////////////////////////////
    account_local_stat(god.get(), whom.get());
    ////////////////////////// Test Subject //////////////////////////////

    const auto &send_to_char = unit_tests::OutputCapture::Instance()->getSendToCharData();
    BOOST_TEST(send_to_char.m_message == "Has NOT yet paid for playing.\n\r");
}

BOOST_AUTO_TEST_CASE(account_local_stat_funds_test)
{
    whom->account.total_credit = 5000;

    ////////////////////////// Test Subject //////////////////////////////
    account_local_stat(god.get(), whom.get());
    ////////////////////////// Test Subject //////////////////////////////

    const auto &send_to_char = unit_tests::OutputCapture::Instance()->getSendToCharData();
    BOOST_TEST(send_to_char.m_message == "Has paid for playing.\n\r");
}

BOOST_AUTO_TEST_CASE(account_local_stat_admin_test)
{
    god->points.level = ADMINISTRATOR_LEVEL;
    whom->account.total_credit = 5000;

    ////////////////////////// Test Subject //////////////////////////////
    account_local_stat(god.get(), whom.get());
    ////////////////////////// Test Subject //////////////////////////////

    std::string expected("Credit         :  0.00\n\r"
                         "Credit Limit   :  0.00\n\r"
                         "Credit to date : 50.00\n\r"
                         "Credit Card    : REGISTERED\n\r"
                         "Discount       :   0%\n\r"
                         "Flat Rate      : Expired (none)\n\r"
                         "Crack counter  :   0\n\r");
    const auto &send_to_char = unit_tests::OutputCapture::Instance()->getSendToCharData();
    BOOST_TEST(send_to_char.m_message == expected);
}
BOOST_AUTO_TEST_SUITE_END() // Account_Local_Stat_Suite
//===========================================================================================

BOOST_FIXTURE_TEST_SUITE(Account_Defaults_Suite, AccountCPPFixture)
BOOST_AUTO_TEST_CASE(account_defaults_test)
{
    whom->account.credit = 333.0;
    whom->account.credit_limit = 333;
    whom->account.total_credit = 333;
    whom->account.last4 = 333;
    whom->account.discount = 222;
    whom->account.cracks = 222;
    whom->account.flatrate = 333;

    ////////////////////////// Test Subject //////////////////////////////
    account_defaults(whom.get());
    ////////////////////////// Test Subject //////////////////////////////

    BOOST_TEST(whom->account.credit == 0.0);
    BOOST_TEST(whom->account.credit_limit == 0);
    BOOST_TEST(whom->account.total_credit == 0);
    BOOST_TEST(whom->account.last4 == -1);
    BOOST_TEST(whom->account.discount == 0);
    BOOST_TEST(whom->account.cracks == 0);
    BOOST_TEST(whom->account.flatrate == 0);
}
BOOST_AUTO_TEST_SUITE_END() // Account_Defaults_Suite
//===========================================================================================

BOOST_FIXTURE_TEST_SUITE(Account_Subtract_Suite, AccountCPPFixture)
BOOST_AUTO_TEST_CASE(account_subtract_npc_test)
{
    // Don't know how to test this
    ////////////////////////// Test Subject //////////////////////////////
    // account_subtract(npc.get(), {}, {});
    ////////////////////////// Test Subject //////////////////////////////
}
BOOST_AUTO_TEST_SUITE_END() // Account_Subtract_Suite
//===========================================================================================

BOOST_FIXTURE_TEST_SUITE(Account_Is_Overdue_Suite, AccountCPPFixture)

BOOST_AUTO_TEST_CASE(account_is_overdue_test_false_false)
{
    g_cServerConfig.disableAccounting();
    g_cAccountConfig.m_nFreeFromLevel = 1;
    whom->points.level = 10;
    ////////////////////////// Test Subject //////////////////////////////
    auto rc = account_is_overdue(whom.get());
    ////////////////////////// Test Subject //////////////////////////////

    BOOST_TEST(rc == FALSE);
}

BOOST_AUTO_TEST_CASE(account_is_overdue_test_false_true)
{
    g_cServerConfig.disableAccounting();
    g_cAccountConfig.m_nFreeFromLevel = 10;
    whom->points.level = 1;
    ////////////////////////// Test Subject //////////////////////////////
    auto rc = account_is_overdue(whom.get());
    ////////////////////////// Test Subject //////////////////////////////

    BOOST_TEST(rc == FALSE);
}

BOOST_AUTO_TEST_CASE(account_is_overdue_test_true_false)
{
    g_cServerConfig.enableAccounting();
    g_cAccountConfig.m_nFreeFromLevel = 1;
    whom->points.level = 10;
    ////////////////////////// Test Subject //////////////////////////////
    auto rc = account_is_overdue(whom.get());
    ////////////////////////// Test Subject //////////////////////////////

    BOOST_TEST(rc == FALSE);
}

BOOST_AUTO_TEST_CASE(account_is_overdue_test_1)
{
    g_cServerConfig.enableAccounting();
    g_cAccountConfig.m_nFreeFromLevel = 10;
    whom->points.level = 1;
    whom->account.flatrate = time(nullptr) + 10000;
    whom->account.credit = -1000.00;
    ////////////////////////// Test Subject //////////////////////////////
    auto rc = account_is_overdue(whom.get());
    ////////////////////////// Test Subject //////////////////////////////

    BOOST_TEST(rc == FALSE);
}

BOOST_AUTO_TEST_CASE(account_is_overdue_test_2)
{
    g_cServerConfig.enableAccounting();
    g_cAccountConfig.m_nFreeFromLevel = 10;
    whom->points.level = 1;
    whom->account.flatrate = time(nullptr) - 10000;
    whom->account.credit = -1000.00;
    ////////////////////////// Test Subject //////////////////////////////
    auto rc = account_is_overdue(whom.get());
    ////////////////////////// Test Subject //////////////////////////////

    BOOST_TEST(rc == TRUE);
}

BOOST_AUTO_TEST_CASE(account_is_overdue_test_3)
{
    g_cServerConfig.enableAccounting();
    g_cAccountConfig.m_nFreeFromLevel = 10;
    whom->points.level = 1;
    whom->account.flatrate = time(nullptr) + 10000;
    whom->account.credit = 1000.00;
    ////////////////////////// Test Subject //////////////////////////////
    auto rc = account_is_overdue(whom.get());
    ////////////////////////// Test Subject //////////////////////////////

    BOOST_TEST(rc == FALSE);
}

BOOST_AUTO_TEST_CASE(account_is_overdue_test_4)
{
    g_cServerConfig.enableAccounting();
    g_cAccountConfig.m_nFreeFromLevel = 10;
    whom->points.level = 1;
    whom->account.flatrate = time(nullptr) - 10000;
    whom->account.credit = -1000.00;
    ////////////////////////// Test Subject //////////////////////////////
    auto rc = account_is_overdue(whom.get());
    ////////////////////////// Test Subject //////////////////////////////

    BOOST_TEST(rc == TRUE);
}
BOOST_AUTO_TEST_SUITE_END() // Account_Is_Overdue_Suite
//===========================================================================================

BOOST_FIXTURE_TEST_SUITE(Account_Overdue_Suite, AccountCPPFixture)

BOOST_AUTO_TEST_CASE(account_overdue_test)
{
    g_cAccountConfig.m_nHourlyRate = 5;
    whom->account.credit_limit = 4895;
    whom->account.credit = 456;
    ////////////////////////// Test Subject //////////////////////////////
    account_overdue(whom.get());
    ////////////////////////// Test Subject //////////////////////////////

    const auto &msgs = unit_tests::OutputCapture::Instance()->getSendToCharDataMulti();

    BOOST_TEST(msgs.size() == 2);
    BOOST_TEST(msgs[0].m_message == "Your account is overdue by -4.56 dollar with a limit of 48.95 dollar.\n\r"
                                    "The account will expire in 1070 hours and 12 minutes.\n\r\n\r");
    BOOST_TEST(msgs[1].m_message == "&c+yYour account is overdue.&nPlease report this message #1 to Papi.&cw&n");
}

BOOST_AUTO_TEST_CASE(account_overdue_test_2)
{
    whom->account.credit_limit = 4895;
    whom->account.credit = 456;
    whom->account.discount = 100;
    ////////////////////////// Test Subject //////////////////////////////
    account_overdue(whom.get());
    ////////////////////////// Test Subject //////////////////////////////

    const auto &msgs = unit_tests::OutputCapture::Instance()->getSendToCharDataMulti();

    BOOST_TEST(msgs.size() == 2);
    BOOST_TEST(msgs[0].m_message == "Your account is overdue by -4.56 dollar with a limit of 48.95 dollar.\n\r"
                                    "The account will expire in 0 hours and 0 minutes.\n\r\n\r");
    BOOST_TEST(msgs[1].m_message == "&c+yYour account is overdue.&nPlease report this message #1 to Papi.&cw&n");
}

BOOST_AUTO_TEST_SUITE_END() // Account_Overdue_Suite
//===========================================================================================

BOOST_FIXTURE_TEST_SUITE(Account_Paypoint_Suite, AccountCPPFixture)
BOOST_AUTO_TEST_CASE(account_paypoint_test)
{
    ////////////////////////// Test Subject //////////////////////////////
    account_paypoint(whom.get());
    ////////////////////////// Test Subject //////////////////////////////
    const auto &send_to_char = unit_tests::OutputCapture::Instance()->getSendToCharData();
    BOOST_TEST(send_to_char.m_message == "&c+gYou can not enter that part of the game unless you give a donation.&cw&n");
}
BOOST_AUTO_TEST_SUITE_END() // Account_Paypoint_Suite
//===========================================================================================

BOOST_FIXTURE_TEST_SUITE(Account_Closed_Suite, AccountCPPFixture)

BOOST_AUTO_TEST_CASE(account_closed_test_disabled)
{
    g_cServerConfig.disableAccounting();
    ////////////////////////// Test Subject //////////////////////////////
    account_closed(whom.get());
    ////////////////////////// Test Subject //////////////////////////////
    const auto &send_to_char = unit_tests::OutputCapture::Instance()->getSendToCharDataMulti();
    BOOST_TEST(send_to_char.empty());
}

BOOST_AUTO_TEST_CASE(account_closed_test_enabled)
{
    g_cServerConfig.enableAccounting();
    ////////////////////////// Test Subject //////////////////////////////
    account_closed(whom.get());
    ////////////////////////// Test Subject //////////////////////////////
    const auto &send_to_char = unit_tests::OutputCapture::Instance()->getSendToCharData();
    BOOST_TEST(send_to_char.m_message == "&c+rYour account is closed.&nPlease report this message #2 to Papi.&cw&n");
}

BOOST_AUTO_TEST_SUITE_END() // Account_Closed_Suite
//===========================================================================================

BOOST_FIXTURE_TEST_SUITE(Account_Is_Closed_Suite, AccountCPPFixture)
BOOST_AUTO_TEST_CASE(account_is_closed_test_1)
{
    g_cServerConfig.disableAccounting();
    g_cAccountConfig.m_nFreeFromLevel = 10;
    whom->points.level = 1;
    ////////////////////////// Test Subject //////////////////////////////
    auto rc = account_is_closed(whom.get());
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(rc == FALSE);
}
BOOST_AUTO_TEST_CASE(account_is_closed_test_2)
{
    g_cServerConfig.disableAccounting();
    g_cAccountConfig.m_nFreeFromLevel = 1;
    whom->points.level = 10;
    ////////////////////////// Test Subject //////////////////////////////
    auto rc = account_is_closed(whom.get());
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(rc == FALSE);
}

BOOST_AUTO_TEST_CASE(account_is_closed_test_3)
{
    g_cServerConfig.enableAccounting();
    g_cAccountConfig.m_nFreeFromLevel = 1;
    whom->points.level = 10;
    ////////////////////////// Test Subject //////////////////////////////
    auto rc = account_is_closed(whom.get());
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(rc == FALSE);
}

BOOST_AUTO_TEST_CASE(account_is_closed_test_4)
{
    g_cServerConfig.enableAccounting();
    g_cAccountConfig.m_nFreeFromLevel = 10;
    whom->points.level = 1;
    whom->account.flatrate = time(nullptr) + 1000;
    ////////////////////////// Test Subject //////////////////////////////
    auto rc = account_is_closed(whom.get());
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(rc == FALSE);
}

BOOST_AUTO_TEST_CASE(account_is_closed_test_5)
{
    g_cServerConfig.enableAccounting();
    g_cAccountConfig.m_nFreeFromLevel = 10;
    whom->points.level = 1;
    whom->account.flatrate = time(nullptr) - 1000;
    whom->account.credit = -50;
    whom->account.credit_limit = 100;
    ////////////////////////// Test Subject //////////////////////////////
    auto rc = account_is_closed(whom.get());
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(rc == FALSE);
}

BOOST_AUTO_TEST_CASE(account_is_closed_test_6)
{
    g_cServerConfig.enableAccounting();
    g_cAccountConfig.m_nFreeFromLevel = 10;
    whom->points.level = 1;
    whom->account.flatrate = time(nullptr) - 1000;
    whom->account.credit = -500;
    whom->account.credit_limit = 100;
    ////////////////////////// Test Subject //////////////////////////////
    auto rc = account_is_closed(whom.get());
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(rc == TRUE);
}
BOOST_AUTO_TEST_SUITE_END() // Account_Is_Closed_Suite
//===========================================================================================

BOOST_AUTO_TEST_SUITE_END() // Account_Public_Funcs_Suite

// BOOST_AUTO_TEST_SUITE(CAccountConfig_Suite)
// BOOST_AUTO_TEST_SUITE_END()

#pragma clang diagnostic pop
