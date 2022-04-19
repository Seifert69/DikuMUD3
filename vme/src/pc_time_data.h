#pragma once

#include "bytestring.h"

class pc_time_data
{
public:
    pc_time_data() = default;
    ~pc_time_data() = default;
    pc_time_data(const pc_time_data &) = default;
    pc_time_data &operator=(const pc_time_data &) = default;
    pc_time_data(pc_time_data &&) = default;
    pc_time_data &operator=(pc_time_data &&) = default;

    void readFrom(CByteBuffer &buf, int &errors);

    void writeTo(CByteBuffer &buf) const;

    time_t getPlayerCharacterCreationTime() const;
    void setPlayerCharacterCreationTime(time_t value);

    time_t getPlayerLastConnectTime() const;
    void setPlayerLastConnectTime(time_t value);

    time_t getPlayerBirthday() const;
    time_t *getPlayerBirthdayPtr();
    void setPlayerBirthday(time_t value);

    ubit32 getTotalTimePlayedInSeconds() const;
    void setTotalTimePlayedInSeconds(ubit32 value);
    void incTotalTimePlayedInSecondsBy(ubit32 seconds);

private:
    time_t creation{0}; ///< This represents time when the pc was created.
    time_t connect{0};  ///< This is the last time that the pc connected.
    time_t birth{0};    ///< This represents the characters age
    ubit32 played{0};   ///< This is the total accumulated time played in secs
};
