#include "OutputCapture.h"

namespace unit_tests
{
std::unique_ptr<OutputCapture> OutputCapture::m_instance;

OutputCapture *OutputCapture::Instance()
{
    return m_instance.get();
}

void OutputCapture::EnableOutputCapture()
{
    m_instance = std::unique_ptr<OutputCapture>(new OutputCapture);
}

void OutputCapture::slog(log_level level, ubit8 wizinv_level, const std::string &fmt, const std::string &msg)
{
    if (m_instance)
    {
        m_instance->m_slog.emplace_back(slog_data{level, wizinv_level, fmt, msg});
    }
}

const slog_data &OutputCapture::getSLogData() const
{
    assert(m_slog.size() == 1);
    return m_slog[0];
}

const std::vector<slog_data> &OutputCapture::getSLogDataMulti() const
{
    return m_slog;
}

void OutputCapture::account_log(char action, unit_data *, unit_data *, int amount)
{
    if (m_instance)
    {
        m_instance->m_account_log.emplace_back(account_log_data{action, amount});
    }
}

const account_log_data &OutputCapture::getAccountLogData() const
{
    assert(m_account_log.size() == 1);
    return m_account_log[0];
}

const std::vector<account_log_data> &OutputCapture::getAccountLogDataMulti() const
{
    return m_account_log;
}

void OutputCapture::send_to_char(const char *messg, const unit_data *)
{
    if (m_instance)
    {
        assert(messg != nullptr);
        m_instance->m_send_to_char.emplace_back(send_to_char_data{messg});
    }
}

void OutputCapture::send_to_char(const std::string &messg, const unit_data *)
{
    if (m_instance)
    {
        m_instance->m_send_to_char.emplace_back(send_to_char_data{messg});
    }
}

const send_to_char_data &OutputCapture::getSendToCharData() const
{
    assert(m_send_to_char.size() == 1);
    return m_send_to_char[0];
}

const std::vector<send_to_char_data> &OutputCapture::getSendToCharDataMulti() const
{
    return m_send_to_char;
}

void OutputCapture::page_string(descriptor_data *, const std::string &msg)
{
    if (m_instance)
    {
        m_instance->m_page_string.emplace_back(page_string_data{msg});
    }
}

void OutputCapture::page_string(descriptor_data *, const char *msg)
{
    if (m_instance && msg != nullptr)
    {
        assert(msg != nullptr);
        m_instance->m_page_string.emplace_back(page_string_data{msg});
    }
}

const page_string_data &OutputCapture::getPageStringData() const
{
    assert(m_page_string.size() == 1);
    return m_page_string[0];
}

const std::vector<page_string_data> &OutputCapture::getPageStringDataMulti() const
{
    return m_page_string;
}

void OutputCapture::Clear()
{
    m_slog.clear();
    m_account_log.clear();
    m_send_to_char.clear();
    m_page_string.clear();
}

} // namespace unit_tests
