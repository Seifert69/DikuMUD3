/*
 $Author: All $
 $RCSfile: color.h,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.4 $
 */
#pragma once

#include "essential.h"
#include "values.h"

#include <map>
#include <optional>
#include <string>

class color_type
{
private:
    std::map<std::string, std::string> m_map;

public:
    // These all use the pointer interface and should be removed later
    // once each use has been checked. std::string does not like nullptr being
    // passed, so I didn't want implicit conversions happening
    // There are std::string overloads for all of them which these
    // just call
    [[nodiscard]] std::string insert(char *key, char *c);
    void insert(const char *combo);
    [[nodiscard]] std::string change(char *key, char *c);
    void change(const char *combo);
    [[nodiscard]] std::string get(const char *key) const;
    [[nodiscard]] std::string get(const char *key, char *full_key) const;
    int remove(char *key);
    void create(const char *input_str);

public:
    color_type() = default;
    color_type(const color_type &) = delete;
    auto operator=(const color_type &) -> color_type & = delete;
    color_type(color_type &&) = delete;
    auto operator=(color_type &&) -> color_type & = delete;
    ~color_type() = default;

    [[nodiscard]] std::string insert(std::string keyword, std::string color);
    void insert(const std::string &combo);
    void change(const std::string &combo);
    [[nodiscard]] std::string change(const std::string &keyword, std::string color); // Nothing uses this except the tests
    [[nodiscard]] std::string get(const std::string &key) const;
    [[nodiscard]] std::string get(const std::string &key, std::string &full_key) const;
    int remove(const std::string &key);
    void remove_all();
    void create(const std::string &input_string);
    [[nodiscard]] std::string key_string();
    [[nodiscard]] std::string key_string(const color_type &dft) const;
    [[nodiscard]] std::string save_string() const;
};
