#pragma once

#include "char_data.h"
#include "color.h"
#include "dil.h"
#include "pc_account_data.h"
#include "pc_time_data.h"
#include "protocol.h"

class pc_data : public char_data
{
public:
    static constexpr auto PC_MAX_PASSWORD = 13; ///< Max length of any pc-password
    static constexpr auto PC_MAX_NAME = 15;     ///< 14 Characters + Null
    static size_t g_world_nopc;                 ///< number of chars in the world

public:
    pc_data();
    ~pc_data() override;

    void gstate_tomenu(dilprg *pdontstop);
    void gstate_togame(dilprg *pdontstart);

    void disconnect_game();
    void connect_game();
    void reconnect_game(descriptor_data *d);

    /**
     * @name Terminal related code
     * @{
     */
    terminal_setup_type &getTerminalSetupType();
    /// @}

    /**
     * @name PC time related code
     * @{
     */
    pc_time_data &getPCTimeInformation();
    /// @}

    /**
     * @name Accounting related code
     * @{
     */
    pc_account_data &getPCAccountData();
    /// @}

    /**
     * @name Guild related code
     * @{
     */
    const char *getGuild() const;
    char **getGuildPtr();
    void freeGuild();
    void setGuild(const char *value);
    /// @}

    /**
     * @name Bank related code
     * @{
     */
    const char *getBank() const;
    char **getBankPtr();
    void freeBank();
    void setBank(char *value);
    /// @}

    /**
     * @name Hometown related code
     * @{
     */
    const char *getHometown() const;
    char **getHometownPtr();
    void setHometown(char *value);
    /// @}

    /** @name Prompt String related code
     * @{
     */
    const char *getPromptString() const;
    char **getPromptStringPtr();
    void setPromptString(char *value);
    void freePromptString();
    /// @}

    /**
     * @name Admin related code
     * @{
     */
    extra_list &getAdministrationInformation();
    /// @}

    /**
     * @name Quest related code
     * @{
     */
    extra_list &getQuestInformation();
    /// @}

    /**
     * @name Profession related code
     * @{
     */
    sbit8 getProfession() const;
    sbit8 *getProfessionPtr();
    void setProfession(sbit8 value);
    /// @}

    /**
     * @name Virtual Player level code
     * @{
     */
    ubit16 getVirtualPlayerLevel() const;
    ubit16 *getVirtualPlayerLevelPtr();
    void setVirtualPlayerLevel(ubit16 value);
    void incrementVirtualPlayerLevel();
    /// @}

    /**
     * @name UID for players
     * @{
     */
    sbit32 getPlayerUID() const;
    void setPlayerUID(sbit32 value);
    /// @}

    /**
     * @name Skill points
     * @{
     */
    sbit32 getSkillPoints() const;
    sbit32 *getSkillPointsPtr();
    void setSkillPoints(sbit32 value);
    void increaseSkillPointsBy(sbit32 value);
    /// @}

    /**
     * @name Ability Points
     * @{
     */
    sbit32 getAbilityPoints() const;
    sbit32 *getAbilityPointsPtr();
    void setAbilityPoints(sbit32 value);
    void increaseAbilityPointsBy(sbit32 value);
    /// @}

    /**
     * @name Flags
     * @{
     */
    ubit16 getAllPCFlags() const;
    ubit16 *getAllPCFlagsPtr();
    void setPCFlag(ubit16 value);
    void setAllPCFlags(ubit16 value);
    void togglePCFlag(ubit16 value);
    void removePCFlag(ubit16 value);
    /// @}

    /**
     * @name Crimes Committed
     * @{
     */
    ubit16 getNumberOfCrimesCommitted() const;
    ubit16 *getNumberOfCrimesCommittedPtr();
    void setNumberOfCrimesCommitted(ubit16 value);
    /// @}

    /**
     * @name Crack Attempts
     * @{
     */
    ubit16 getNumberOfCrackAttempts() const;
    void setNumberOfCrackAttempts(ubit16 value);
    void incrementNumberOfCrackAttempts();
    /// @}

    /**
     * @name Lifespan
     * @{
     */
    ubit16 getLifespan() const;
    ubit16 *getLifespanPtr();
    void setLifespan(ubit16 value);
    /// @}

    /**
     * @name Spells skills Learned
     * @todo Add range checking to indexes
     * @{
     */
    sbit16 getSpellSkillAtIndex(size_t index) const;
    sbit16 *getSpellSkillArrayPtr();
    sbit16 *getSpellSkillAtIndexPtr(size_t index);
    void setSpellSKillAtIndexTo(size_t index, sbit16 value);
    void increaseSpellSkillAtIndexBy(size_t index, sbit16 value);
    void decreaseSpellSkillAtIndexBy(size_t index, sbit16 value);
    /// @}

    /**
     * @name Spell level
     * @todo Add range checking to indexes
     * @{
     */
    ubit8 getSpellLevelAtIndex(size_t index) const;
    ubit8 *getSpellLevelAtIndexPtr(size_t index);
    ubit8 *getSpellLevelArrayPtr();
    void setSpellLevelAtIndexTo(size_t index, ubit8 value);
    /// @}

    /**
     * @name Skills
     * @todo Add range checking to indexes
     * @{
     */
    sbit16 getSkillAtIndex(size_t index) const;
    sbit16 *getSkillAtIndexPtr(size_t index);
    sbit16 *getSkillArrayPtr();
    void setSkillAtIndexTo(size_t index, sbit16 value);
    void increaseSkillAtIndexBy(size_t index, sbit16 value);
    void decreaseSkillAtIndexBy(size_t index, sbit16 value);
    /// @}

    /**
     * @name Skill Level
     * @todo Add range checking to indexes
     * @{
     */
    ubit8 getSkillLevelAtIndex(size_t index) const;
    ubit8 *getSkillLevelAtIndexPtr(size_t index);
    ubit8 *getSkillLevelArrayPtr();
    void setSkillLevelAtIndexTo(size_t index, ubit8 value);
    /// @}

    /**
     * @name Weapon skill
     * @todo Add range checking to indexes
     * @{
     */
    sbit16 getWeaponSkillAtIndex(size_t index) const;
    sbit16 *getWeaponSkillArrayPtr();
    sbit16 *getWeaponSkillAtIndexPtr(size_t index);
    void setWeaponSkillAtIndexTo(size_t index, sbit16 value);
    void increaseWeaponSkillAtIndexBy(size_t index, sbit16 value);
    void decreaseWeaponSkillAtIndexBy(size_t index, sbit16 value);
    /// @}

    /**
     * @name Weapon Skill Level
     * @todo Add range checking to indexes
     * @{
     */
    ubit8 getWeaponSkillLevelAtIndex(size_t index) const;
    ubit8 *getWeaponSkillLevelAtIndexPtr(size_t index);
    ubit8 *getWeaponSkillLevelArrayPtr();
    void setWeaponSkillLevelAtIndexTo(size_t index, ubit8 value);
    /// @}

    /**
     * @name Ability Level
     * @todo Add range checking to indexes
     * @{
     */
    ubit8 getAbilityLevelAtIndex(size_t index) const;
    ubit8 *getAbilityLevelAtIndexPtr(size_t index);
    ubit8 *getAbilityLevelArrayPtr();
    void setAbilityLevelAtIndexTo(size_t index, ubit8 value);
    /// @}

    /**
     * @name Conditions
     * @todo Add range checking to indexes
     * @{
     */
    sbit8 getConditionAtIndex(size_t index) const;
    sbit8 *getConditionAtIndexPtr(size_t index);
    void setConditionAtIndexTo(size_t index, sbit8 value);
    void increaseConditionAtIndexBy(size_t index, sbit8 value);
    void decreaseConditionAtIndexBy(size_t index, sbit8 value);
    /// @}

    /**
     * @name Access Level
     * @{
     */
    ubit8 getAccessLevel() const;
    void setAccessLevel(ubit8 value);
    /// @}

    /**
     * @name Passwords
     * @{
     */
    const char *getPassword() const;
    void setPassword(const char *value);
    int readPasswordFrom(CByteBuffer &buf);
    void truncatePassword();
    /// @}

    /**
     * @name Filename
     * @{
     */
    const char *getFilename() const;
    int readFilenameFrom(CByteBuffer &buf);
    void setFilename(const char *value);
    /// @}

    /**
     * @name Last hosts
     * @todo Add range checking to indexes
     * @{
     */
    ubit32 getLastHostAtIndex(size_t index) const;
    void setLastHostAtIndexTo(size_t index, ubit32 value);
    void pushBackLastHost(ubit32 value);
    /// @}

    /**
     * @name Color
     * @{
     */
    const color_type &getColor() const;
    color_type &getColor();
    /// @}
private:
    terminal_setup_type m_setup{0, 0, 0, 0, 0, 0, 0, 0}; ///<
    pc_time_data m_time{};                               ///< PCs time info
    pc_account_data m_account{};                         ///< Accounting
    char *m_guild{nullptr};                              ///< Player's current default guild (guilds in .info)
    char *m_bank{nullptr};                               ///< How much money in bank?
    char *m_hometown{nullptr};                           ///< PCs Hometown (symbolic reference)
    char *m_promptstr{nullptr};                          ///< A PC's Prompt
    extra_list m_info;                                   ///< For saving Admin information
    extra_list m_quest;                                  ///< For saving QUEST information
    sbit8 m_profession{-1};                              ///< The player's chosen profession, -1 means unknown
    ubit16 m_vlvl{0};                                    ///< Virtual Level for player
    sbit32 m_id{-1};                                     ///< Unique identifier for each player (-1 guest)
    sbit32 m_skill_points{0};                            ///< No of practice points left
    sbit32 m_ability_points{0};                          ///< No of practice points left
    ubit16 m_flags{0};                                   ///< flags for PC setup (brief, noshout...)
    ubit16 m_nr_of_crimes{0};                            ///< Number of crimes committed
    ubit16 m_crack_attempts{0};                          ///< Number of wrong passwords entered
    ubit16 m_lifespan{0};                                ///< How many year to live....
    sbit16 m_spells[SPL_TREE_MAX]{0};                    ///< The spells learned
    ubit8 m_spell_lvl[SPL_TREE_MAX]{0};                  ///< Practiced within that level
    sbit16 m_skills[SKI_TREE_MAX]{0};                    ///< The skills learned
    ubit8 m_skill_lvl[SKI_TREE_MAX]{0};                  ///< The skills practiced within level
    sbit16 m_weapons[WPN_TREE_MAX]{0};                   ///< The weapons learned
    ubit8 m_weapon_lvl[WPN_TREE_MAX]{0};                 ///< The weapons learned
    ubit8 m_ability_lvl[ABIL_TREE_MAX]{0};               ///< The abilities learned
    sbit8 m_conditions[3]{0};                            ///< Drunk full etc.
    ubit8 m_nAccessLevel{0};                             ///< Access Level for BBS use
    char m_pwd[PC_MAX_PASSWORD]{0};                      ///< Needed when loaded w/o descriptor
    char m_filename[PC_MAX_NAME]{0};                     ///< The name on disk...
    ubit32 m_lasthosts[5]{0};                            ///< last 5 different IPs
    color_type m_color;                                  ///< Players default colors
};
