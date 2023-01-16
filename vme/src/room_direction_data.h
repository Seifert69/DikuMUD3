#pragma once

#include "namelist.h"

class unit_data;

class room_direction_data
{
public:
    room_direction_data();
    ~room_direction_data();

    const cNamelist &getOpenName() const;
    cNamelist &getOpenName();

    const char *getKey() const;
    char **getKeyPtr();
    void setKey(char *value);

    const unit_data *getToRoom() const;
    unit_data *getToRoom();
    void setToRoom(unit_data *value);

    ubit8 getSkillDifficulty() const;
    ubit8 *getSkillDifficultyPtr();
    void setSkillDifficulty(ubit8 value);

    ubit8 getDoorFlags() const;
    bool isDoorFlagSet(ubit8 value) const;
    void setDoorFlags(ubit8 value);
    ubit8 *getDoorFlagsPtr();

    void toJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const;

private:
    cNamelist open_name;         ///< For Open & Enter
    char *key{nullptr};          ///<
    unit_data *to_room{nullptr}; ///<
    ubit8 difficulty{0};         ///< Skill needed for swim, climb, search, pick-lock
    int weight{0};               ///< Used for shortest path algorithm
    ubit8 exit_info{0};          ///< Door info flags
};
