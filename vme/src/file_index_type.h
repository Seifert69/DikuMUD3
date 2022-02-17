#pragma once
#include "essential.h"

#include <cstring>
#include <forward_list>
#include <string>

class unit_data;
class zone_type;

/* A linked sorted list of all units within a zone file */
class file_index_type
{
public:
    file_index_type() = default;                                  // Default ctor
    ~file_index_type() = default;                                 // Default dtor
    file_index_type(const file_index_type &) = delete;            // Delete copy ctor
    file_index_type &operator=(const file_index_type &) = delete; // Delete assignment
    file_index_type(file_index_type &&) = delete;                 // Delete move ctor
    file_index_type &operator=(file_index_type &&) = delete;      // Delete move assignment

    [[nodiscard]] unit_data *find_symbolic_instance();
    [[nodiscard]] unit_data *find_symbolic_instance_ref(unit_data *ref, ubit16 bitvector);

    [[nodiscard]] const char *getName() const;
    [[nodiscard]] zone_type *getZone() const;
    [[nodiscard]] long getFilepos() const;
    [[nodiscard]] ubit32 getLength() const;
    [[nodiscard]] ubit32 getCRC() const;
    [[nodiscard]] sbit16 getNumInZone() const;
    [[nodiscard]] ubit16 getNumInMem() const;
    [[nodiscard]] ubit16 getRoomNum() const;
    [[nodiscard]] ubit8 getType() const;

    [[nodiscard]] bool Empty() const;
    [[nodiscard]] unit_data *Front() const;
    [[nodiscard]] std::forward_list<unit_data *>::iterator Begin();
    [[nodiscard]] std::forward_list<unit_data *>::iterator End();
    void PushFront(unit_data *value);
    void Remove(unit_data *value);

    void IncrementNumInMemory();
    void DecrementNumInMemory();

    void IncrementNumInZone();

    /**
     * @param value Name to set
     * @param to_lower If true convert to lower case
     */
    void setName(const char *value, bool to_lower = false);
    void setZone(zone_type *value);
    void setCRC(ubit32 value);
    void setType(ubit8 value);
    void setNumInZone(sbit16 value);
    void setNumInMemory(sbit16 value);
    void setLength(ubit32 value);
    void setFilepos(long value);
    void setRoomNum(ubit16 value);

private:
    std::forward_list<unit_data *> fi_unit_list; // This list of units that match this file_index
    std::string name{};                          // Unique within this list
    zone_type *zone{nullptr};                    // Pointer to owner of structure
    long filepos{};                              // Byte offset into file
    ubit32 length{};                             // No of bytes to read
    ubit32 crc{};                                // CRC check for compressed items
    sbit16 no_in_zone{};                         // Updated in zone reset for reset
    ubit16 no_in_mem{};                          // Number of these in the game
    ubit16 room_no{};                            // The number of the room
    ubit8 type{};                                // Room/Obj/Char or other?
};
