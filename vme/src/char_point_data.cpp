#include "char_point_data.h"

#include "json_helper.h"
#include "slog.h"
#include "utility.h"
#include "utils.h"

char_point_data::char_point_data()
    : flags{0}          ///< Char flags
    , exp{0}            ///< The experience of the player
    , race{0}           ///< PC/NPC race, Humanoid, Animal, etc.
    , mana{0}           ///< How many mana points are left?
    , endurance{0}      ///< How many endurance points are left?
    , offensive{0}      ///< The OB of a character.
    , defensive{0}      ///< The DB of a character.
    , speed{0}          ///< The default speed for natural combat
    , natural_armour{0} ///< The natural built-in armour (ARM_)
    , attack_type{0}    ///< PC/NPC Attack Type for bare hands (WPN_)
    , dex_reduction{0}  ///< For speed of armour calculations only
    , sex{0}            ///< PC / NPC s sex
    , level{0}          ///< PC / NPC s level
    , position{0}       ///< Standing, sitting, fighting...
{
    memset(abilities.data(), 0, abilities.size());
}

ubit32 char_point_data::getCharacterFlags() const
{
    return flags;
}

ubit32 *char_point_data::getCharacterFlagsPtr()
{
    return &flags;
}

void char_point_data::setAllCharacterFlags(ubit32 value)
{
    flags = value;
}

void char_point_data::setCharacterFlag(ubit32 value)
{
    flags |= value;
}

void char_point_data::removeCharacterFlag(ubit32 value)
{
    flags &= ~value;
}

void char_point_data::toggleCharacterFlag(ubit32 value)
{
    flags ^= value;
}

sbit32 char_point_data::getPlayerExperience() const
{
    return exp;
}

sbit32 *char_point_data::getPlayerExperiencePtr()
{
    return &exp;
}

void char_point_data::setPlayerExperience(sbit32 value)
{
    exp = value;
}

void char_point_data::increasePlayerExperienceBy(sbit32 value)
{
    exp += value;
}

ubit16 char_point_data::getRace() const
{
    return race;
}

ubit16 *char_point_data::getRacePtr()
{
    return &race;
}

void char_point_data::setRace(ubit16 value)
{
    race = value;
}

sbit16 char_point_data::getMana() const
{
    return mana;
}

sbit16 *char_point_data::getManaPtr()
{
    return &mana;
}

void char_point_data::setMana(sbit16 value)
{
    mana = value;
}

void char_point_data::decreaseManaBy(sbit16 value)
{
    mana -= value;
}

sbit16 char_point_data::getEndurance() const
{
    return endurance;
}

sbit16 *char_point_data::getEndurancePtr()
{
    return &endurance;
}

void char_point_data::setEndurance(sbit16 value)
{
    endurance = value;
}

void char_point_data::decreaseEnduranceBy(sbit16 value)
{
    endurance -= value;
}

sbit16 char_point_data::getOffensiveBonus() const
{
    return offensive;
}

void char_point_data::setOffensiveBonus(sbit16 value)
{
    offensive = value;
}

sbit16 *char_point_data::getOffensiveBonusPtr()
{
    return &offensive;
}

sbit16 char_point_data::getDefensiveBonus() const
{
    return defensive;
}

void char_point_data::setDefensiveBonus(sbit16 value)
{
    defensive = value;
}

sbit16 *char_point_data::getDefensiveBonusPtr()
{
    return &defensive;
}

sbit16 char_point_data::getSpeed() const
{
    return speed;
}

///        N1  N2
/// 100% = 1   2
/// 200% = 2   4
/// 300% = 3   6
int char_point_data::getNumberOfMeleeAttacks(int naturalAttacks, bool isPC)
{
    return MAX(1, (naturalAttacks * getSpeedPercentage(isPC)) / 100);
}

/// Calculate the speed percentage. 200% max for players,
/// 300% max for NPCs. 25% is the worst possible (4x slowed)
int char_point_data::getSpeedPercentage(bool isPC)
{
    sbit16 s = speed;

    if (s < SPEED_MIN)
        s = SPEED_MIN;

    if (s == SPEED_DEFAULT)
        return 100;

    if (isPC)
        return MAX(MIN(200, (100 * SPEED_DEFAULT) / s), 25);
    else
        return MAX(MIN(300, (100 * SPEED_DEFAULT) / s), 25);
}

/// max values are -8 to +8
/// +1.. increases the speed
/// -1.. decreases the speed
void char_point_data::modifySpeedBy(sbit16 delta)
{
    if ((delta > 8) or (delta < -8))
    {
        slog(LOG_ALL, 0, "Attempt to modify speed by an invalid amount %d", delta);
        return;
    }

    speed -= delta;
}

/// Should only be called from raw operations
void char_point_data::setSpeed(sbit16 value)
{
    speed = value;
}

ubit8 char_point_data::getNaturalArmor() const
{
    return natural_armour;
}

void char_point_data::setNaturalArmor(ubit8 value)
{
    natural_armour = value;
}

ubit8 *char_point_data::getNaturalArmorPtr()
{
    return &natural_armour;
}

ubit8 char_point_data::getAttackType() const
{
    return attack_type;
}

void char_point_data::setAttackType(ubit8 value)
{
    attack_type = value;
}

ubit8 *char_point_data::getAttackTypePtr()
{
    return &attack_type;
}

ubit8 char_point_data::getSex() const
{
    return sex;
}

void char_point_data::setSex(ubit8 value)
{
    sex = value;
}

ubit8 *char_point_data::getSexPtr()
{
    return &sex;
}

ubit8 char_point_data::getLevel() const
{
    return level;
}

void char_point_data::setLevel(ubit8 value)
{
    level = value;
}

ubit8 *char_point_data::getLevelPtr()
{
    return &level;
}

void char_point_data::incrementLevel()
{
    ++level;
}

ubit8 char_point_data::getPosition() const
{
    return position;
}

void char_point_data::setPosition(ubit8 value)
{
    position = value;
}

ubit8 *char_point_data::getPositionPtr()
{
    return &position;
}

sbit16 char_point_data::getAbilityAtIndex(size_t index) const
{
    return abilities[index];
}

sbit16 char_point_data::getSTR() const
{
    return abilities[ABIL_STR];
}

sbit16 char_point_data::getDEX() const
{
    return abilities[ABIL_DEX];
}

sbit16 char_point_data::getCON() const
{
    return abilities[ABIL_CON];
}

sbit16 char_point_data::getCHA() const
{
    return abilities[ABIL_CHA];
}

sbit16 char_point_data::getBRA() const
{
    return abilities[ABIL_BRA];
}

sbit16 char_point_data::getMAG() const
{
    return abilities[ABIL_MAG];
}

sbit16 char_point_data::getDIV() const
{
    return abilities[ABIL_DIV];
}

sbit16 char_point_data::getHPP() const
{
    return abilities[ABIL_HP];
}

sbit16 *char_point_data::getAbilityAtIndexPtr(size_t index)
{
    return &abilities[index];
}

void char_point_data::setAbilityAtIndexTo(size_t index, sbit16 value)
{
    abilities[index] = value;
}

void char_point_data::increaseAbilityAtIndexBy(size_t index, sbit16 value)
{
    abilities[index] += value;
}

void char_point_data::decreaseAbilityAtIndexBy(size_t index, sbit16 value)
{
    abilities[index] -= value;
}

std::array<sbit16, ABIL_TREE_MAX> &char_point_data::getAbilitiesArray()
{
    return abilities;
}

void char_point_data::setSTR(sbit16 value)
{
    abilities[ABIL_STR] = value;
}

void char_point_data::setDEX(sbit16 value)
{
    abilities[ABIL_DEX] = value;
}

void char_point_data::setCON(sbit16 value)
{
    abilities[ABIL_CON] = value;
}

void char_point_data::setCHA(sbit16 value)
{
    abilities[ABIL_CHA] = value;
}

void char_point_data::setBRA(sbit16 value)
{
    abilities[ABIL_BRA] = value;
}

void char_point_data::setMAG(sbit16 value)
{
    abilities[ABIL_MAG] = value;
}

void char_point_data::setDIV(sbit16 value)
{
    abilities[ABIL_DIV] = value;
}

void char_point_data::setHPP(sbit16 value)
{
    abilities[ABIL_HP] = value;
}

/// @todo unit should not need to be passed, but it looks like there are parent fields in middle of char_point_data in the binary file
void char_point_data::readFrom(CByteBuffer &buf, ubit8 unit_version, unit_data *unit, int &error)
{
    exp = buf.ReadS32(&error);
    flags = buf.ReadU32(&error);

    mana = buf.ReadS16(&error);
    endurance = buf.ReadS16(&error);

    natural_armour = buf.ReadU8(&error);

    if (unit_version <= 74)
    {
        if (unit_version >= 39)
        {
            ubit8 spd = buf.ReadU8(&error); // Ensure correct conversion
            speed = spd;
            if (unit->isPC())
            {
                if (speed < SPEED_MIN)
                {
                    speed = SPEED_DEFAULT;
                }
            }
        }
        else
        {
            speed = SPEED_DEFAULT;
        }
    }
    else // Speed for version 75 and up
    {
        // Speed is fluid, minimum is applied in function, not in storage
        speed = buf.ReadS16(&error);
    }

    attack_type = buf.ReadU16(&error); /// @todo Why is attack_type 8 bit it really should be 16 it looks like

    if (unit_version <= 52)
    {
        unit->setSize(buf.ReadU16(&error));
    }
    race = buf.ReadU16(&error);

    offensive = buf.ReadS16(&error);
    defensive = buf.ReadS16(&error);

    sex = buf.ReadU8(&error);
    level = buf.ReadU8(&error);
    position = buf.ReadU8(&error);

    int j = buf.ReadU8(&error);

    for (int i = 0; i < j; i++)
    {
        if (unit_version < 69)
        {
            abilities[i] = buf.ReadU8(&error);
        }
        else
        {
            abilities[i] = buf.ReadS16(&error);
        }

        if (unit->isPC())
        {
            UPC(unit)->setAbilityLevelAtIndexTo(i, buf.ReadU8(&error));
            if (unit_version < 72)
            {
                error += buf.Skip8();
            }
        }
    }
}

void char_point_data::toJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const
{
    writer.StartObject();
    {
        std::string bits;
        json::write_kvp("flags", sprintbit(bits, flags, g_char_flags), writer);
        json::write_kvp("exp", exp, writer);
        //        auto race_text=sprinttype(nullptr, race, g_pc_races) : itoa(CHAR_RACE(u))
        json::write_kvp("race", race, writer);

        json::write_kvp("mana", mana, writer);
        json::write_kvp("endurance", endurance, writer);
        json::write_kvp("offensive", offensive, writer);
        json::write_kvp("defensive", defensive, writer);
        json::write_kvp("speed", speed, writer);
        json::write_kvp("natural_armour", natural_armour, writer);
        json::write_kvp("attack_type", attack_type, writer);
        json::write_kvp("dex_reduction", dex_reduction, writer);
        json::write_kvp("sex", sprinttype(nullptr, sex, g_char_sex), writer);
        json::write_kvp("level", level, writer);
        json::write_kvp("position", sprinttype(nullptr, position, g_char_pos), writer);

        json::write_kvp("MAG", abilities[ABIL_MAG], writer);
        json::write_kvp("DIV", abilities[ABIL_DIV], writer);
        json::write_kvp("STR", abilities[ABIL_STR], writer);
        json::write_kvp("DEX", abilities[ABIL_DEX], writer);
        json::write_kvp("CON", abilities[ABIL_CON], writer);
        json::write_kvp("CHA", abilities[ABIL_CHA], writer);
        json::write_kvp("BRA", abilities[ABIL_BRA], writer);
        json::write_kvp("HP", abilities[ABIL_HP], writer);
    }
    writer.EndObject();
}
