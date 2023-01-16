#pragma once

#include "bytestring.h"
#include "vme.h"

#include <rapidjson/prettywriter.h>

#include <array>

class unit_data;

class char_point_data
{
public:
    char_point_data();
    /*~char_point_data(void); not needed yet all base types they destroy themselves*/

    /**
     *
     * @param buf Buffer to read from
     * @param unit_version Version number of data object
     * @param unit We have to pass this in because there data in the middle that doesn't belong to char_point_data
     * @param error Error count
     */
    void readFrom(CByteBuffer &buf, ubit8 unit_version, unit_data *unit, int &error);

    ubit32 getCharacterFlags() const;
    ubit32 *getCharacterFlagsPtr();
    void setAllCharacterFlags(ubit32 value);
    void setCharacterFlag(ubit32 value);
    void removeCharacterFlag(ubit32 value);
    void toggleCharacterFlag(ubit32 value);

    sbit32 getPlayerExperience() const;
    sbit32 *getPlayerExperiencePtr();
    void setPlayerExperience(sbit32 value);
    void increasePlayerExperienceBy(sbit32 value);

    ubit16 getRace() const;
    ubit16 *getRacePtr();
    void setRace(ubit16 value);

    sbit16 getMana() const;
    sbit16 *getManaPtr();
    void setMana(sbit16 value);
    void decreaseManaBy(sbit16 value);

    sbit16 getEndurance() const;
    sbit16 *getEndurancePtr();
    void setEndurance(sbit16 value);
    void decreaseEnduranceBy(sbit16 value);

    sbit16 getOffensiveBonus() const;
    void setOffensiveBonus(sbit16 value);
    sbit16 *getOffensiveBonusPtr();

    sbit16 getDefensiveBonus() const;
    void setDefensiveBonus(sbit16 value);
    sbit16 *getDefensiveBonusPtr();

    sbit16 getSpeed() const;
    void setSpeed(sbit16 value);
    void modifySpeedBy(sbit16 delta);

private:
    /// @todo moving this up to char_data where it should be - will be deleted after
    int getNumberOfMeleeAttacks(int naturalAttacks, bool isPC);
    int getSpeedPercentage(bool isPC);

public:
    ubit8 getNaturalArmor() const;
    void setNaturalArmor(ubit8 value);
    ubit8 *getNaturalArmorPtr();

    ubit8 getAttackType() const;
    void setAttackType(ubit8 value);
    ubit8 *getAttackTypePtr();

    ubit8 getSex() const;
    void setSex(ubit8 value);
    ubit8 *getSexPtr();

    ubit8 getLevel() const;
    void setLevel(ubit8 value);
    ubit8 *getLevelPtr();
    void incrementLevel();

    ubit8 getPosition() const;
    void setPosition(ubit8 value);
    ubit8 *getPositionPtr();

    sbit16 getAbilityAtIndex(size_t index) const;
    sbit16 *getAbilityAtIndexPtr(size_t index);
    sbit16 getSTR() const;
    sbit16 getDEX() const;
    sbit16 getCON() const;
    sbit16 getCHA() const;
    sbit16 getBRA() const;
    sbit16 getMAG() const;
    sbit16 getDIV() const;
    sbit16 getHPP() const;
    void setSTR(sbit16 value);
    void setDEX(sbit16 value);
    void setCON(sbit16 value);
    void setCHA(sbit16 value);
    void setBRA(sbit16 value);
    void setMAG(sbit16 value);
    void setDIV(sbit16 value);
    void setHPP(sbit16 value);
    void setAbilityAtIndexTo(size_t index, sbit16 value);
    void increaseAbilityAtIndexBy(size_t index, sbit16 value);
    void decreaseAbilityAtIndexBy(size_t index, sbit16 value);
    std::array<sbit16, ABIL_TREE_MAX> &getAbilitiesArray();

    void toJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const;

private:
    ubit32 flags{0};                                ///< Char flags
    sbit32 exp{0};                                  ///< The experience of the player
    ubit16 race{0};                                 ///< PC/NPC race, Humanoid, Animal, etc.
    sbit16 mana{0};                                 ///< How many mana points are left?
    sbit16 endurance{0};                            ///< How many endurance points are left?
    sbit16 offensive{0};                            ///< The OB of a character.
    sbit16 defensive{0};                            ///< The DB of a character.
    sbit16 speed{0};                                ///< The speed for combat
    ubit8 natural_armour{0};                        ///< The natural built-in armour (ARM_)
    ubit8 attack_type{0};                           ///< PC/NPC Attack Type for bare hands (WPN_)
    ubit8 dex_reduction{0};                         ///< For speed of armour calculations only
    ubit8 sex{0};                                   ///< PC / NPC s sex
    ubit8 level{0};                                 ///< PC / NPC s level
    ubit8 position{0};                              ///< Standing, sitting, fighting...
    std::array<sbit16, ABIL_TREE_MAX> abilities{0}; ///< Str/dex etc.
};
