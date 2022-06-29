#pragma once

#include "char_follow_type.h"
#include "char_point_data.h"
#include "combat.h"
#include "descriptor_data.h"
#include "unit_data.h"

class char_data : public unit_data
{
public:
    static size_t g_world_nochars; // number of chars in the world

public:
    explicit char_data(ubit8 unit_type);
    virtual ~char_data();

    /**
     * @name Descriptor related code
     * @{
     */
    const descriptor_data *getDescriptor() const;
    descriptor_data *getDescriptor();
    void setDescriptor(descriptor_data *value);
    ///@}

    /**
     * @name Combat related code
     * @{
     */
    const cCombat *getCombat() const;
    cCombat *getCombat();
    void setCombat(cCombat *value);
    ///@}

    /**
     * @name Master related code
     * @{
     */
    const unit_data *getMaster() const;
    unit_data *getMaster();
    unit_data **getMasterPtr();
    void setMaster(unit_data *value);
    ///@}

    /**
     * @name Last Location related code
     * @{
     */
    const unit_data *getLastLocation() const;
    unit_data *getLastLocation();
    unit_data **getLastLocationPtr();
    void setLastLocation(unit_data *value);
    ///@}

    /**
     * @name Character points related functions
     * @todo This is basically char_point_data's public interface - so in future char_point_data can be just removed
     * @{
     */
    void readFrom(CByteBuffer &buf, ubit8 unit_version, unit_data *unit, int &error);

    ubit32 *getCharacterFlagsPtr();
    ubit32 getCharacterFlags() const;
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

    int getNumberOfMeleeAttacks(int naturalAttacks);

    sbit16 getSpeed() const;
    void setSpeed(sbit16 value);
    void modifySpeedBy(sbit16 delta);
    /**
     * Calculate the speed percentage. 200% max for players, 300% max for NPCs. 25% is the worst possible (4x slowed)
     * @return speed percentage
     */
    int getSpeedPercentage();

    ubit8 getNaturalArmor() const;
    void setNaturalArmor(ubit8 value);
    ubit8 *getNaturalArmorPtr();

    ubit8 getAttackType() const;
    void setAttackType(ubit8 value);
    ubit8 *getAttackTypePtr();

    virtual ubit8 getSex() const;
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
    /// @}

    /**
     * @name Follower related code
     * @{
     */
    const char_follow_type *getFollowers() const;
    char_follow_type *getFollowers();
    void setFollowers(char_follow_type *value);
    /// @}

    /**
     * @name
     * @{
     */
    const char *getLastAttacker() const;
    void freeLastAttacker();
    void setLastAttacker(char *value);
    ///@}

    /**
     * @name Money related functions
     * @{
     */
    const char *getMoney() const;
    char *getMoney();
    char **getMoneyPtr();
    void freeMoney();
    void setMoney(char *value);
    ///@}

    /**
     * @name Last Attacker type functions
     * @{
     */
    ubit8 getLastAttackerType() const;
    void setLastAttackerType(ubit8 value);
    ///@}
private:
    descriptor_data *m_descriptor{nullptr}; ///<
    cCombat *m_combat{nullptr};             ///<
    unit_data *m_master{nullptr};           ///< Must be a char
    unit_data *m_last_room{nullptr};        ///< Last location of character
    char_point_data m_points;               ///<
    char_follow_type *m_followers{nullptr}; ///<
    char *m_last_attacker{nullptr};         ///< Last attacker of character
    char *m_money{nullptr};                 ///<  Money transfer from db-files.
    ubit8 m_last_attacker_type{0};          ///< Last attacker type of character
};
