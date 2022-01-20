/*
 $Author: All $
 $RCSfile: color.cpp,v $
 $Date: 2003/10/09 01:12:32 $
 $Revision: 2.3 $
 */
#include "color.h"

#include "formatter.h"
#include "textutil.h"

#include <optional>
#include <sstream>

std::string color_type::insert(char *key, char *c)
{
    if (!key || !c)
    {
        return {};
    }
    return insert(std::string{key}, std::string{c});
}

std::string color_type::insert(std::string keyword, std::string color)
{
    if (keyword.empty() || color.empty())
    {
        return {};
    }
    auto it = m_map.insert_or_assign(std::move(keyword), std::move(color));
    return {it.first->second + it.first->first};
}

void color_type::change(const char *combo)
{
    if (!combo)
    {
        return;
    }
    change(std::string{combo});
}

void color_type::change(const std::string &combo)
{
    auto key{combo.substr(0, combo.find(':'))};
    auto tok{combo.substr(combo.find(':') + 1, std::string::npos)};

    (void)change(key, tok);
}

void color_type::insert(const char *combo)
{
    if (!combo)
    {
        return;
    }
    insert(std::string{combo});
}

void color_type::insert(const std::string &combo)
{
    auto key{combo.substr(0, combo.find(':'))};
    auto tok{combo.substr(combo.find(':') + 1, std::string::npos)};

    if (key.empty() || tok.empty())
    {
        return;
    }
    (void)insert(std::move(key), std::move(tok));
}

std::string color_type::change(char *key, char *c)
{
    if (!key || !c)
    {
        return {};
    }
    return change(std::string{key}, std::string{c});
}

std::string color_type::change(const std::string &keyword, std::string color)
{
    if (auto search = m_map.find(keyword); search != m_map.end())
    {
        search->second = std::move(color);

        std::ostringstream strm;
        strm << diku::format_to_str("<div class='%s'>%s = %s</div>", search->second, keyword, search->second);
        return strm.str();
    }
    return {};
}

std::string color_type::get(const char *key) const
{
    if (!key)
    {
        return {};
    }
    return get(std::string{key});
}

std::string color_type::get(const std::string &key) const
{
    if (auto it = m_map.lower_bound(key); it != m_map.end())
    {
        auto min = std::min(key.length(), it->first.length());
        // emulate old strncmp comparison of prefix match
        if (min > 0 && key.compare(0, min, it->first, 0, min) == 0)
        {
            return it->second;
        }
    }
    return {};
}

std::string color_type::get(const char *key, char *full_key) const
{
    if (!key)
    {
        return {};
    }

    std::string temp_full_key;
    auto ret = get(std::string{key}, temp_full_key);
    if (!ret.empty() && full_key)
    {
        memcpy(full_key, temp_full_key.data(), temp_full_key.length() + 1);
    }

    return ret;
}

// TODO write a test that covers the partial key match scenario
std::string color_type::get(const std::string &key, std::string &full_key) const
{
    if (auto it = m_map.lower_bound(key); it != m_map.end())
    {
        auto min = std::min(key.length(), it->first.length());
        // emulate old strncmp comparison of prefix match
        if (min > 0 && key.compare(0, min, it->first, 0, min) == 0)
        {
            full_key = it->first;
            return it->second;
        }
    }
    return {};
}

int color_type::remove(char *key)
{
    if (!key)
    {
        return 0;
    }
    return remove(std::string{key});
}

int color_type::remove(const std::string &key)
{
    return static_cast<int>(m_map.erase(key));
}

void color_type::remove_all()
{
    m_map.clear();
}

void color_type::create(const char *input_temp)
{
    if (!input_temp)
    {
        return;
    }
    create(std::string(input_temp));
}

void color_type::create(const std::string &input_string)
{
    bool done = false;
    std::string::size_type key_start = 0;
    std::string::size_type val_start = 0;
    while (!done)
    {
        auto key_end = input_string.find(':', key_start);
        if (key_end == std::string::npos)
        {
            done = true;
            continue;
        }
        auto key = input_string.substr(key_start, key_end - key_start);

        val_start = key_end + 1;
        auto val_end = input_string.find_first_of(':', val_start);
        auto token = input_string.substr(val_start, val_end - val_start);
        if (token.empty())
        {
            done = true;
            continue;
        }

        (void)insert(key, token);
        if (val_end == std::string::npos)
        {
            done = true;
        }
        key_start = val_end + 1;
    }
}

std::string color_type::key_string()
{
    if (m_map.empty())
    {
        return {};
    }

    std::ostringstream strm;
    for (const auto &[keyword, color] : m_map)
    {
        auto i = std::max(0, 25 - static_cast<int>(keyword.length()));
        strm << diku::format_to_str("<div class='%s'>%s%s= %s</div><br/>", color, keyword, spc(i), color);
    }
    strm << "<br/>";
    return strm.str();
}

std::string color_type::key_string(const color_type &dft) const
{
    auto t = save_string();
    auto d = dft.save_string(); // ptr to create string

    auto combined_string = d + t;

    color_type temp;
    temp.create(combined_string);

    return temp.key_string();
}

std::string color_type::save_string() const
{
    if (m_map.empty())
    {
        return {};
    }

    std::ostringstream strm;
    for (const auto &[keyword, color] : m_map)
    {
        strm << keyword << ":" << color << ":";
    }
    return strm.str();
}
