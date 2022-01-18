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
    // clang-format off
    [[deprecated("use std::string insert(std::string keyword, std::string color) instead"), nodiscard]]
    std::string insert(char *key, char *c);

    [[deprecated("use void insert(const std::string &combo) instead")]]
    void insert(const char *combo);

    [[deprecated("use std::string change(const std::string &keyword, std::string color) instead"), nodiscard]]
    std::string change(char *key, char *c);

    [[deprecated("use void change(const std::string &combo) instead")]]
    void change(const char *combo);

    [[deprecated("use std::string get(const std::string &key) const instead"), nodiscard]]
    std::string get(const char *key) const;

    [[deprecated("use std::string get(const std::string &key, std::string &full_key) const instead"), nodiscard]]
    std::string get(const char *key, char *full_key) const;

    [[deprecated("use int remove(const std::string &key) instead")]]
    int remove(char *key);

    [[deprecated("use void create(const std::string &input_string) instead")]]
    void create(const char *input_str);
    // clang-format on
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
