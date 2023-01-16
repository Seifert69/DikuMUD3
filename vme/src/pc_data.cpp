#include "pc_data.h"

#include "act_other.h"
#include "affect.h"
#include "comm.h"
#include "config.h"
#include "dilinst.h"
#include "dilrun.h"
#include "files.h"
#include "formatter.h"
#include "handler.h"
#include "json_helper.h"
#include "main_functions.h"
#include "nanny.h"
#include "pcsave.h"
#include "reception.h"
#include "slog.h"
#include "utils.h"

#include <arpa/inet.h>

size_t pc_data::g_world_nopc = 0; // number of chars in the world

pc_data::pc_data(file_index_type *fi)
    : char_data(UNIT_ST_PC, fi)
    , m_setup{0, 0, 0, 0, 0, 0, 0, 0}
    , m_guild{nullptr}
    , m_bank{nullptr}
    , m_hometown{nullptr}
    , m_promptstr{nullptr}
    , m_profession{-1}
    , m_vlvl{0}
    , m_id{-1}
    , m_skill_points{0}
    , m_ability_points{0}
    , m_flags{0}
    , m_nr_of_crimes{0}
    , m_crack_attempts{0}
    , m_lifespan{0}
    , m_spells{0}
    , m_spell_lvl{0}
    , m_skills{0}
    , m_skill_lvl{0}
    , m_weapons{0}
    , m_weapon_lvl{0}
    , m_ability_lvl{0}
    , m_conditions{0}
    , m_nAccessLevel{0}
    , m_pwd{0}
    , m_filename{0}
    , m_lasthosts{0}
{
    g_world_nopc++;
}

pc_data::~pc_data()
{
    g_world_nopc--;

    if (m_guild)
        FREE(m_guild);

    if (m_hometown)
        FREE(m_hometown);

    if (m_bank)
        FREE(m_bank);

    if (m_promptstr)
        FREE(m_promptstr);
}

// Take a player which is in the game and move to the menu
// Don't stop the DIL program executing as pdontstop
//
void pc_data::gstate_tomenu(dilprg *pdontstop)
{
    if (is_destructed())
    {
        return;
    }

    if (!char_is_playing(this))
    {
        return;
    }

    // slog(LOG_ALL, 0, "DEBUG: To menu %s", UNIT_NAME(this));

    if (!PC_IS_UNSAVED(this))
    {
        save_player(this);
        save_player_contents(this, TRUE);
    }

    setLastLocation(unit_room(this));

    stop_fightfollow(this);
    stop_snoopwrite(this);

    descriptor_data *tmp_descr = CHAR_DESCRIPTOR(this);
    UCHAR(this)->setDescriptor(nullptr);

    while (getUnitContains())
    {
        extract_unit(getUnitContains());
    }

    UCHAR(this)->setDescriptor(tmp_descr);

    unit_from_unit(this);
    remove_from_unit_list(this);
    dil_stop_special(this, pdontstop);
}

#define DILWAY 1

// Take a player which is in the menu and move to the game
// Don't start the DIL program executing as pdontstart
//
void pc_data::gstate_togame(dilprg *pdontstop)
{
    descriptor_data *i = nullptr;
    const time_t last_connect = m_time.getPlayerLastConnectTime();

    if (is_destructed())
    {
        return;
    }

    if (char_is_playing(this))
    { // Are we in the menu?
        return;
    }

    // slog(LOG_ALL, 0, "DEBUG: To game %s", UNIT_NAME(this));

    if (CHAR_DESCRIPTOR(this))
    {
        update_lasthost(this, inet_addr(CHAR_DESCRIPTOR(this)->getHostname()));

        CHAR_DESCRIPTOR(this)->setMinutesPlayerIdle(0);
        CHAR_DESCRIPTOR(this)->setPromptMode(PROMPT_EXPECT);
        CHAR_DESCRIPTOR(this)->setLastLogonTime(::time(nullptr));
        m_time.setPlayerLastConnectTime(time(nullptr));
        set_descriptor_fptr(CHAR_DESCRIPTOR(this), descriptor_interpreter, FALSE);
        dil_destroy("link_dead@basis", this);

        connect_game();
    }

    unit_data *load_room = nullptr;
    if (getLastLocation())
    {
        load_room = getLastLocation();
        setLastLocation(nullptr);
    }
    else
    {
        load_room = hometown_unit(m_hometown);
    }

    reset_char(this);

    insert_in_unit_list(this);
    unit_to_unit(this, load_room);

    if (CHAR_DESCRIPTOR(this) && !DILWAY) /* Only do these things if player is connected */
    {
        auto msg = diku::format_to_str("%s has entered the world.<br/>", getNames().Name());

        for (i = g_descriptor_list; i; i = i->getNext())
        {
            if (descriptor_is_playing(i) && i->cgetCharacter() != this && CHAR_CAN_SEE(CHAR_ORIGINAL(i->getCharacter()), this) &&
                CHAR_ORIGINAL(i->getCharacter())->isPC() && IS_SET(PC_FLAGS(CHAR_ORIGINAL(i->getCharacter())), PC_INFORM) &&
                !same_surroundings(this, i->cgetCharacter()))
            {
                send_to_descriptor(msg, i);
            }
        }

        act("$1n has arrived.", eA_HIDEINV, cActParameter(this), cActParameter(), cActParameter(), eTO_ROOM);
    }

    /* New player stats. Level can be zero after reroll while ID is not. */
    if ((CHAR_LEVEL(this) == 0) && PC_IS_UNSAVED(this))
    {
        slog(LOG_BRIEF, 0, "%s[%s] (GUEST) has entered the game.", PC_FILENAME(this), CHAR_DESCRIPTOR(this)->getHostname());

        m_id = new_player_id();

        start_player(this);
    }
    else
    {
        if (!DILWAY)
        {
            command_interpreter(this, "look");
        }
    }
    if (file_exists(ContentsFileName(PC_FILENAME(this))))
    {
        load_contents(PC_FILENAME(this), this);

        if (!DILWAY)
        {
            rent_calc(this, last_connect);
        }
    }

    /*		if (!dilway)*/
    if (strcmp(g_cServerConfig.getImmortalName().c_str(), getNames().Name()) == 0)
    {
        setLevel(ULTIMATE_LEVEL);
    }

    if (IS_ULTIMATE(this) && PC_IS_UNSAVED(this))
    {
        save_player(this);
    }

    start_affect(this); /* Activate affect ticks */
    // start_all_special(this); /* Activate fptr ticks   */
    if (!DILWAY)
    {
        send_done(this, nullptr, load_room, 0, &g_cmd_auto_play, "");
        send_done(this, nullptr, load_room, 0, &g_cmd_auto_play, "", nullptr, "void@basis");
    }

    ActivateDil(this);
    dil_start_special(this, pdontstop);
}

void pc_data::connect_game()
{
    //  assert (CHAR_DESCRIPTOR (pc));

    if (this->is_destructed() || !CHAR_DESCRIPTOR(this))
    {
        return;
    }

    m_time.setPlayerLastConnectTime(time(nullptr));
    CHAR_DESCRIPTOR(this)->setLastLogonTime(time(nullptr));

    CHAR_DESCRIPTOR(this)->CreateBBS();

    set_descriptor_fptr(CHAR_DESCRIPTOR(this), descriptor_interpreter, FALSE);

    g_no_players++;
    if (g_no_players > g_max_no_players)
    {
        g_max_no_players = g_no_players;
    }
}

void pc_data::disconnect_game()
{
    if (CHAR_DESCRIPTOR(this))
    {
        CHAR_DESCRIPTOR(this)->RemoveBBS();
    }

    g_no_players--;
}

void pc_data::reconnect_game(descriptor_data *d)
{
    // char *color;
    // char tbuf[MAX_STRING_LENGTH * 2];

    if (is_destructed() || !d)
    {
        return;
    }

    UCHAR(d->cgetCharacter())->setDescriptor(nullptr);
    extract_unit(d->getCharacter()); // Toss out the temporary unit and take over the new one
    d->setCharacter(this);
    UCHAR(this)->setDescriptor(d);

    dil_destroy("link_dead@basis", this);

    ActivateDil(this); // ch could potentially get zapped here.

    if (is_destructed() || !d)
    {
        return;
    }

    connect_game();
    /* MS2020
     color = UPC (ch)->color.save_string ();
     s printf (tbuf, "%s%s%s", CONTROL_COLOR_CREATE, color, CONTROL_COLOR_END);
     send_to_char (tbuf, ch);
     delete color;
 */
    send_to_char("Reconnecting.<br/>", this);

    if (CHAR_LAST_ROOM(this) && (CHAR_LAST_ROOM(this) != getUnitIn()))
    {
        act("$1n has reconnected, and is moved to another location.",
            eA_HIDEINV,
            cActParameter(this),
            cActParameter(),
            cActParameter(),
            eTO_ROOM);
        unit_from_unit(this);
        unit_to_unit(this, CHAR_LAST_ROOM(this));
        setLastLocation(nullptr);
    }
    act("$1n has reconnected.", eA_HIDEINV, cActParameter(this), cActParameter(), cActParameter(), eTO_ROOM);
    slog(LOG_BRIEF, getLevelOfWizardInvisibility(), "%s[%s] has reconnected.", PC_FILENAME(this), CHAR_DESCRIPTOR(this)->getHostname());
    CHAR_DESCRIPTOR(this)->setLastLogonTime(::time(nullptr));
    m_time.setPlayerLastConnectTime(time(nullptr));
    //      stop_affect(ch);
    //      stop_all_special(ch);
    //      start_affect(ch);               /* Activate affect ticks */
    //   start_all_special(ch);     /* Activate fptr ticks   */

    set_descriptor_fptr(d, descriptor_interpreter, FALSE);
}

terminal_setup_type &pc_data::getTerminalSetupType()
{
    return m_setup;
}

pc_time_data &pc_data::getPCTimeInformation()
{
    return m_time;
}

pc_account_data &pc_data::getPCAccountData()
{
    return m_account;
}

const char *pc_data::getGuild() const
{
    return m_guild;
}

char **pc_data::getGuildPtr()
{
    return &m_guild;
}

void pc_data::freeGuild()
{
    if (m_guild)
    {
        FREE(m_guild);
        m_guild = nullptr;
    }
}

void pc_data::setGuild(const char *value)
{
    if (m_guild == nullptr)
    {
        CREATE(m_guild, char, strlen(value) + 1);
    }
    else
    {
        RECREATE(m_guild, char, strlen(value) + 1);
    }
    strcpy(m_guild, value);
}

const char *pc_data::getBank() const
{
    return m_bank;
}

char **pc_data::getBankPtr()
{
    return &m_bank;
}

void pc_data::freeBank()
{
    if (m_bank)
    {
        FREE(m_bank);
    }
}

void pc_data::setBank(char *value)
{
    m_bank = value;
}

const char *pc_data::getHometown() const
{
    return m_hometown;
}

char **pc_data::getHometownPtr()
{
    return &m_hometown;
}

void pc_data::setHometown(char *value)
{
    if (m_hometown == nullptr)
    {
        CREATE(m_hometown, char, strlen(value) + 1);
    }
    else
    {
        RECREATE(m_hometown, char, strlen(value) + 1);
    }
    strcpy(m_hometown, value);
}

const char *pc_data::getPromptString() const
{
    return m_promptstr;
}

char **pc_data::getPromptStringPtr()
{
    return &m_promptstr;
}

void pc_data::setPromptString(char *value)
{
    m_promptstr = value;
}

void pc_data::freePromptString()
{
    if (m_promptstr)
    {
        FREE(m_promptstr);
    }
}

extra_list &pc_data::getAdministrationInformation()
{
    return m_info;
}

extra_list &pc_data::getQuestInformation()
{
    return m_quest;
}

sbit8 pc_data::getProfession() const
{
    return m_profession;
}

sbit8 *pc_data::getProfessionPtr()
{
    return &m_profession;
}

void pc_data::setProfession(sbit8 value)
{
    m_profession = value;
}

ubit16 pc_data::getVirtualPlayerLevel() const
{
    return m_vlvl;
}

ubit16 *pc_data::getVirtualPlayerLevelPtr()
{
    return &m_vlvl;
}

void pc_data::setVirtualPlayerLevel(ubit16 value)
{
    m_vlvl = value;
}

void pc_data::incrementVirtualPlayerLevel()
{
    m_vlvl++;
}

sbit32 pc_data::getPlayerUID() const
{
    return m_id;
}

void pc_data::setPlayerUID(sbit32 value)
{
    m_id = value;
}

sbit32 pc_data::getSkillPoints() const
{
    return m_skill_points;
}

sbit32 *pc_data::getSkillPointsPtr()
{
    return &m_skill_points;
}

void pc_data::setSkillPoints(sbit32 value)
{
    m_skill_points = value;
}

void pc_data::increaseSkillPointsBy(sbit32 value)
{
    m_skill_points += value;
}

sbit32 pc_data::getAbilityPoints() const
{
    return m_ability_points;
}

sbit32 *pc_data::getAbilityPointsPtr()
{
    return &m_ability_points;
}

void pc_data::setAbilityPoints(sbit32 value)
{
    m_ability_points = value;
}

void pc_data::increaseAbilityPointsBy(sbit32 value)
{
    m_ability_points += value;
}

ubit16 pc_data::getAllPCFlags() const
{
    return m_flags;
}

ubit16 *pc_data::getAllPCFlagsPtr()
{
    return &m_flags;
}

void pc_data::setPCFlag(ubit16 value)
{
    m_flags |= value;
}

void pc_data::setAllPCFlags(ubit16 value)
{
    m_flags = value;
}

void pc_data::togglePCFlag(ubit16 value)
{
    m_flags ^= value;
}

void pc_data::removePCFlag(ubit16 value)
{
    m_flags &= ~value;
}

ubit16 pc_data::getNumberOfCrimesCommitted() const
{
    return m_nr_of_crimes;
}

ubit16 *pc_data::getNumberOfCrimesCommittedPtr()
{
    return &m_nr_of_crimes;
}

void pc_data::setNumberOfCrimesCommitted(ubit16 value)
{
    m_nr_of_crimes = value;
}

ubit16 pc_data::getNumberOfCrackAttempts() const
{
    return m_crack_attempts;
}

void pc_data::setNumberOfCrackAttempts(ubit16 value)
{
    m_crack_attempts = value;
}

void pc_data::incrementNumberOfCrackAttempts()
{
    m_crack_attempts++;
}

ubit16 pc_data::getLifespan() const
{
    return m_lifespan;
}

ubit16 *pc_data::getLifespanPtr()
{
    return &m_lifespan;
}

void pc_data::setLifespan(ubit16 value)
{
    m_lifespan = value;
}

sbit16 pc_data::getSpellSkillAtIndex(size_t index) const
{
    return m_spells[index];
}

sbit16 *pc_data::getSpellSkillArrayPtr()
{
    return &m_spells[0];
}

sbit16 *pc_data::getSpellSkillAtIndexPtr(size_t index)
{
    return &m_spells[index];
}

void pc_data::setSpellSKillAtIndexTo(size_t index, sbit16 value)
{
    m_spells[index] = value;
}

void pc_data::increaseSpellSkillAtIndexBy(size_t index, sbit16 value)
{
    m_spells[index] += value;
}

void pc_data::decreaseSpellSkillAtIndexBy(size_t index, sbit16 value)
{
    m_spells[index] -= value;
}

ubit8 pc_data::getSpellLevelAtIndex(size_t index) const
{
    return m_spell_lvl[index];
}

ubit8 *pc_data::getSpellLevelAtIndexPtr(size_t index)
{
    return &m_spell_lvl[index];
}

ubit8 *pc_data::getSpellLevelArrayPtr()
{
    return &m_spell_lvl[0];
}

void pc_data::setSpellLevelAtIndexTo(size_t index, ubit8 value)
{
    m_spell_lvl[index] = value;
}

sbit16 pc_data::getSkillAtIndex(size_t index) const
{
    return m_skills[index];
}

sbit16 *pc_data::getSkillAtIndexPtr(size_t index)
{
    return &m_skills[index];
}

sbit16 *pc_data::getSkillArrayPtr()
{
    return &m_skills[0];
}

void pc_data::setSkillAtIndexTo(size_t index, sbit16 value)
{
    m_skills[index] = value;
}

void pc_data::increaseSkillAtIndexBy(size_t index, sbit16 value)
{
    m_skills[index] += value;
}

void pc_data::decreaseSkillAtIndexBy(size_t index, sbit16 value)
{
    m_skills[index] -= value;
}

ubit8 pc_data::getSkillLevelAtIndex(size_t index) const
{
    return m_skill_lvl[index];
}

ubit8 *pc_data::getSkillLevelAtIndexPtr(size_t index)
{
    return &m_skill_lvl[index];
}

ubit8 *pc_data::getSkillLevelArrayPtr()
{
    return &m_skill_lvl[0];
}

void pc_data::setSkillLevelAtIndexTo(size_t index, ubit8 value)
{
    m_skill_lvl[index] = value;
}

sbit16 pc_data::getWeaponSkillAtIndex(size_t index) const
{
    return m_weapons[index];
}

sbit16 *pc_data::getWeaponSkillArrayPtr()
{
    return &m_weapons[0];
}

sbit16 *pc_data::getWeaponSkillAtIndexPtr(size_t index)
{
    return &m_weapons[index];
}

void pc_data::setWeaponSkillAtIndexTo(size_t index, sbit16 value)
{
    m_weapons[index] = value;
}

void pc_data::increaseWeaponSkillAtIndexBy(size_t index, sbit16 value)
{
    m_weapons[index] += value;
}

void pc_data::decreaseWeaponSkillAtIndexBy(size_t index, sbit16 value)
{
    m_weapons[index] -= value;
}

ubit8 pc_data::getWeaponSkillLevelAtIndex(size_t index) const
{
    return m_weapon_lvl[index];
}

ubit8 *pc_data::getWeaponSkillLevelAtIndexPtr(size_t index)
{
    return &m_weapon_lvl[index];
}

ubit8 *pc_data::getWeaponSkillLevelArrayPtr()
{
    return &m_weapon_lvl[0];
}

void pc_data::setWeaponSkillLevelAtIndexTo(size_t index, ubit8 value)
{
    m_weapon_lvl[index] = value;
}

ubit8 pc_data::getAbilityLevelAtIndex(size_t index) const
{
    return m_ability_lvl[index];
}

ubit8 *pc_data::getAbilityLevelAtIndexPtr(size_t index)
{
    return &m_ability_lvl[index];
}

ubit8 *pc_data::getAbilityLevelArrayPtr()
{
    return &m_ability_lvl[0];
}

void pc_data::setAbilityLevelAtIndexTo(size_t index, ubit8 value)
{
    m_ability_lvl[index] = value;
}

sbit8 pc_data::getConditionAtIndex(size_t index) const
{
    return m_conditions[index];
}

sbit8 *pc_data::getConditionAtIndexPtr(size_t index)
{
    return &m_conditions[index];
}

void pc_data::setConditionAtIndexTo(size_t index, sbit8 value)
{
    m_conditions[index] = value;
}

void pc_data::increaseConditionAtIndexBy(size_t index, sbit8 value)
{
    m_conditions[index] += value;
}

void pc_data::decreaseConditionAtIndexBy(size_t index, sbit8 value)
{
    m_conditions[index] -= value;
}

ubit8 pc_data::getAccessLevel() const
{
    return m_nAccessLevel;
}

void pc_data::setAccessLevel(ubit8 value)
{
    m_nAccessLevel = value;
}

const char *pc_data::getPassword() const
{
    return m_pwd;
}

void pc_data::setPassword(const char *value)
{
    strncpy(m_pwd, value, PC_MAX_PASSWORD);
    m_pwd[PC_MAX_PASSWORD - 1] = 0;
}

int pc_data::readPasswordFrom(CByteBuffer &buf)
{
    auto retval = buf.ReadStringCopy(m_pwd, PC_MAX_PASSWORD);
    m_pwd[PC_MAX_PASSWORD - 1] = '\0';
    return retval;
}

void pc_data::truncatePassword()
{
    m_pwd[10] = '\0'; // This will allow me to later extend the password length
}

const char *pc_data::getFilename() const
{
    return m_filename;
}

int pc_data::readFilenameFrom(CByteBuffer &buf)
{
    auto retval = buf.ReadStringCopy(m_filename, PC_MAX_NAME);
    m_filename[PC_MAX_NAME - 1] = 0;
    return retval;
}

void pc_data::setFilename(const char *value)
{
    strncpy(m_filename, value, PC_MAX_NAME);
    m_filename[PC_MAX_NAME - 1] = 0;
}

ubit32 pc_data::getLastHostAtIndex(size_t index) const
{
    return m_lasthosts[index];
}

void pc_data::setLastHostAtIndexTo(size_t index, ubit32 value)
{
    m_lasthosts[index] = value;
}

void pc_data::pushBackLastHost(ubit32 value)
{
    memmove(&m_lasthosts[0], &m_lasthosts[1], sizeof(ubit32) * 4);
    m_lasthosts[4] = value;
}

const color_type &pc_data::getColor() const
{
    return m_color;
}

color_type &pc_data::getColor()
{
    return m_color;
}

void pc_data::toJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const
{
    writer.StartObject();
    {
        json::write_unit_id_kvp("id", this, writer);

        writer.String("npc_data");
        writer.StartObject();
        {
            json::write_object_value_kvp("setup", m_setup, writer);
            json::write_object_value_kvp("time", m_time, writer);
            json::write_char_pointer_kvp("guild", m_guild, writer);
            json::write_char_pointer_kvp("bank", m_bank, writer);
            json::write_char_pointer_kvp("hometown", m_hometown, writer);
            json::write_char_pointer_kvp("promptstr", m_promptstr, writer);
            json::write_object_value_kvp("info", m_info, writer);
            json::write_object_value_kvp("quest", m_quest, writer);
            json::write_kvp("profession", m_profession, writer);
            json::write_kvp("vlvl", m_vlvl, writer);
            json::write_kvp("id", m_id, writer);
            json::write_kvp("skill_points", m_skill_points, writer);
            json::write_kvp("ability_points", m_ability_points, writer);
            std::string bits;
            json::write_kvp("flags", sprintbit(bits, m_flags, g_pc_flags), writer);
            json::write_kvp("nr_of_crimes", m_nr_of_crimes, writer);
            json::write_kvp("crack_attempts", m_crack_attempts, writer);
            json::write_kvp("lifespan", m_lifespan, writer);

            writer.String("spells");
            writer.StartArray();
            for (int i = 0; i < SPL_TREE_MAX; ++i)
            {
                writer.StartObject();
                {
                    json::write_kvp("spell", m_spells[i], writer);
                    json::write_kvp("level", m_spell_lvl[i], writer);
                }
                writer.EndObject();
            }
            writer.EndArray();

            writer.String("skills");
            writer.StartArray();
            for (int i = 0; i < SKI_TREE_MAX; ++i)
            {
                writer.StartObject();
                {
                    json::write_kvp("skill", m_skills[i], writer);
                    json::write_kvp("level", m_skill_lvl[i], writer);
                }
                writer.EndObject();
            }
            writer.EndArray();

            writer.String("weapons");
            writer.StartArray();
            for (int i = 0; i < WPN_TREE_MAX; ++i)
            {
                writer.StartObject();
                {
                    json::write_kvp("wepaon", m_weapons[i], writer);
                    json::write_kvp("level", m_weapon_lvl[i], writer);
                }
                writer.EndObject();
            }
            writer.EndArray();

            json::write_kvp("ability_lvl_MAG", m_ability_lvl[ABIL_MAG], writer);
            json::write_kvp("ability_lvl_DIV", m_ability_lvl[ABIL_DIV], writer);
            json::write_kvp("ability_lvl_STR", m_ability_lvl[ABIL_STR], writer);
            json::write_kvp("ability_lvl_DEX", m_ability_lvl[ABIL_DEX], writer);
            json::write_kvp("ability_lvl_CON", m_ability_lvl[ABIL_CON], writer);
            json::write_kvp("ability_lvl_CHA", m_ability_lvl[ABIL_CHA], writer);
            json::write_kvp("ability_lvl_BRA", m_ability_lvl[ABIL_BRA], writer);
            json::write_kvp("ability_lvlHP", m_ability_lvl[ABIL_HP], writer);

            writer.String("conditions");
            writer.StartObject();
            {
                json::write_kvp("drunk", (bool)m_conditions[DRUNK], writer);
                json::write_kvp("full", (bool)m_conditions[FULL], writer);
                json::write_kvp("thirsty", (bool)m_conditions[THIRST], writer);
            }
            writer.EndObject();

            json::write_kvp("AccessLevel", m_nAccessLevel, writer);
            json::write_kvp("pwd", m_pwd, writer);
            json::write_kvp("filename", m_filename, writer);

            writer.String("lasthosts");
            writer.StartArray();
            for (int i = 0; i < 5; ++i)
            {
                in_addr in;
                in.s_addr = m_lasthosts[i];
                writer.String(inet_ntoa(in));
            }
            writer.EndArray();

            writer.String("color");
            m_color.toJSON(writer);
        }
        writer.EndObject();

        writer.String("char_data");
        char_data::toJSON(writer);
    }
    writer.EndObject();
}
