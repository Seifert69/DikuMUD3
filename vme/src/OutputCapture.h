#pragma once

#include "structs.h"
#include "utility.h"

#include <memory>

namespace unit_tests
{
struct slog_data
{
    int m_level{};
    int m_wiz_inv_level{};
    std::string m_fmt{};
    std::string m_message{};
};

struct account_log_data
{
    char m_action{};
    int m_amount{};
};

struct send_to_char_data
{
    std::string m_message{};
};

struct page_string_data
{
    std::string m_message{};
};

class OutputCapture
{
public:
    OutputCapture(OutputCapture &) = delete;
    OutputCapture(OutputCapture &&) = delete;
    OutputCapture &operator=(OutputCapture &) = delete;
    OutputCapture &operator=(OutputCapture &&) = delete;
    ~OutputCapture() = default;

    static OutputCapture *Instance();
    static void EnableOutputCapture();

    static void slog(log_level level, ubit8 wizinv_level, const std::string &fmt, const std::string &msg);
    [[nodiscard]] const slog_data &getSLogData() const;
    [[nodiscard]] const std::vector<slog_data> &getSLogDataMulti() const;

    static void account_log(char action, unit_data *god, unit_data *pc, int amount);
    [[nodiscard]] const account_log_data &getAccountLogData() const;
    [[nodiscard]] const std::vector<account_log_data> &getAccountLogDataMulti() const;

    static void send_to_char(const std::string &messg, const unit_data *ch);
    static void send_to_char(const char *messg, const unit_data *ch);
    [[nodiscard]] const send_to_char_data &getSendToCharData() const;
    [[nodiscard]] const std::vector<send_to_char_data> &getSendToCharDataMulti() const;

    static void page_string(descriptor_data *d, const std::string &);
    static void page_string(descriptor_data *d, const char *);
    [[nodiscard]] const page_string_data &getPageStringData() const;
    [[nodiscard]] const std::vector<page_string_data> &getPageStringDataMulti() const;

    void Clear();

private:
    OutputCapture() = default;

    static std::unique_ptr<OutputCapture> m_instance; // Only created during unit testing
    std::vector<slog_data> m_slog;                    // Captured log messages to slog
    std::vector<account_log_data> m_account_log;      // Captured log messages to account_log
    std::vector<send_to_char_data> m_send_to_char;    // Captured messages to send_to_char
    std::vector<page_string_data> m_page_string;      // Captured message to page_string
};

} // namespace unit_tests
