#pragma once

#include "essential.h"

#include <array>

class file_index_type;
/* A linked list of commands to execute */
class zone_reset_cmd
{
public:
    zone_reset_cmd() = default;                                 // Default ctor
    ~zone_reset_cmd() = default;                                // Default dtor
    zone_reset_cmd(const zone_reset_cmd &) = delete;            // Delete copy ctor
    zone_reset_cmd &operator=(const zone_reset_cmd &) = delete; // Delete assignment
    zone_reset_cmd(zone_reset_cmd &&) = delete;                 // Delete move ctor
    zone_reset_cmd &operator=(zone_reset_cmd &&) = delete;      // Delete move assignment

    [[nodiscard]] ubit8 getCommandNum() const;
    [[nodiscard]] file_index_type *getFileIndexType(size_t index) const;
    [[nodiscard]] sbit16 getNum(size_t index) const;
    [[nodiscard]] ubit8 getCompleteFlag() const;
    [[nodiscard]] zone_reset_cmd *getNext() const;
    [[nodiscard]] zone_reset_cmd *getNested() const;

    void setCommandNum(ubit8 value);
    void setFileIndexType(size_t index, file_index_type *value);
    void setNum(size_t index, sbit16 value);
    void setCompleteFlag(ubit8 value);
    void setNextPtr(zone_reset_cmd *value);
    void setNestedPtr(zone_reset_cmd *value);

private:
    ubit8 cmd_no{};                        // Index to array of func() ptrs
    ubit8 cmpl{};                          // Complete flag
    std::array<file_index_type *, 2> fi{}; //
    std::array<sbit16, 3> num{};           //
    zone_reset_cmd *next{nullptr};         //
    zone_reset_cmd *nested{nullptr};       //
};
