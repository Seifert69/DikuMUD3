#include "char_data.h"

#include "json_helper.h"
#include "vmelimits.h"
size_t char_data::g_world_nochars = 0;

char_data::char_data(ubit8 unit_type, file_index_type *fi)
    : unit_data(unit_type, fi)
    , m_descriptor{nullptr}
    , m_combat{nullptr}
    , m_master{nullptr}
    , m_last_room{nullptr}
    , m_points{}
    , m_followers{nullptr}
    , m_last_attacker{nullptr}
    , m_money{nullptr}
    , m_last_attacker_type{0}
{
    g_world_nochars++;

    // MS2020 memset(&points, 0, sizeof (points));
    // Removed because class constructor already initialize class variables.
}

char_data::~char_data()
{
#ifdef DMSERVER
    if (m_money)
    {
        FREE(m_money);
    }
    if (m_last_attacker)
    {
        FREE(m_last_attacker);
    }
#endif
    g_world_nochars--;
}

const descriptor_data *char_data::getDescriptor() const
{
    return m_descriptor;
}

descriptor_data *char_data::getDescriptor()
{
    return m_descriptor;
}

void char_data::setDescriptor(descriptor_data *value)
{
    m_descriptor = value;
}

const cCombat *char_data::getCombat() const
{
    return m_combat;
}

cCombat *char_data::getCombat()
{
    return m_combat;
}

void char_data::setCombat(cCombat *value)
{
    m_combat = value;
}

const unit_data *char_data::getMaster() const
{
    return m_master;
}

unit_data *char_data::getMaster()
{
    return m_master;
}

unit_data **char_data::getMasterPtr()
{
    return &m_master;
}

void char_data::setMaster(unit_data *value)
{
    m_master = value;
}

const unit_data *char_data::getLastLocation() const
{
    return m_last_room;
}

unit_data *char_data::getLastLocation()
{
    return m_last_room;
}

unit_data **char_data::getLastLocationPtr()
{
    return &m_last_room;
}

void char_data::setLastLocation(unit_data *value)
{
    m_last_room = value;
}

void char_data::readFrom(CByteBuffer &buf, ubit8 unit_version, unit_data *unit, int &error)
{
    return m_points.readFrom(buf, unit_version, unit, error);
}

ubit32 *char_data::getCharacterFlagsPtr()
{
    return m_points.getCharacterFlagsPtr();
}

ubit32 char_data::getCharacterFlags() const
{
    return m_points.getCharacterFlags();
}

void char_data::setAllCharacterFlags(ubit32 value)
{
    m_points.setAllCharacterFlags(value);
}

void char_data::setCharacterFlag(ubit32 value)
{
    m_points.setCharacterFlag(value);
}

void char_data::removeCharacterFlag(ubit32 value)
{
    m_points.removeCharacterFlag(value);
}

void char_data::toggleCharacterFlag(ubit32 value)
{
    m_points.toggleCharacterFlag(value);
}

sbit32 char_data::getPlayerExperience() const
{
    return m_points.getPlayerExperience();
}

sbit32 *char_data::getPlayerExperiencePtr()
{
    return m_points.getPlayerExperiencePtr();
}

void char_data::setPlayerExperience(sbit32 value)
{
    m_points.setPlayerExperience(value);
}

void char_data::increasePlayerExperienceBy(sbit32 value)
{
    m_points.increasePlayerExperienceBy(value);
}

ubit16 char_data::getRace() const
{
    return m_points.getRace();
}

ubit16 *char_data::getRacePtr()
{
    return m_points.getRacePtr();
}

void char_data::setRace(ubit16 value)
{
    m_points.setRace(value);
}

sbit16 char_data::getMana() const
{
    return m_points.getMana();
}

sbit16 *char_data::getManaPtr()
{
    return m_points.getManaPtr();
}

void char_data::setMana(sbit16 value)
{
    m_points.setMana(value);
}

void char_data::decreaseManaBy(sbit16 value)
{
    m_points.decreaseManaBy(value);
}

sbit16 char_data::getEndurance() const
{
    return m_points.getEndurance();
}

sbit16 *char_data::getEndurancePtr()
{
    return m_points.getEndurancePtr();
}

void char_data::setEndurance(sbit16 value)
{
    m_points.setEndurance(value);
}

void char_data::decreaseEnduranceBy(sbit16 value)
{
    m_points.decreaseEnduranceBy(value);
}

sbit16 char_data::getOffensiveBonus() const
{
    return m_points.getOffensiveBonus();
}

void char_data::setOffensiveBonus(sbit16 value)
{
    m_points.setOffensiveBonus(value);
}

sbit16 *char_data::getOffensiveBonusPtr()
{
    return m_points.getOffensiveBonusPtr();
}

sbit16 char_data::getDefensiveBonus() const
{
    return m_points.getDefensiveBonus();
}

void char_data::setDefensiveBonus(sbit16 value)
{
    m_points.setDefensiveBonus(value);
}

sbit16 *char_data::getDefensiveBonusPtr()
{
    return m_points.getDefensiveBonusPtr();
}

int char_data::getNumberOfMeleeAttacks(int naturalAttacks)
{
    return std::max(1, (naturalAttacks * getSpeedPercentage()) / 100);
}

sbit16 char_data::getSpeed() const
{
    return m_points.getSpeed();
}

void char_data::setSpeed(sbit16 value)
{
    m_points.setSpeed(value);
}

void char_data::modifySpeedBy(sbit16 delta)
{
    m_points.modifySpeedBy(delta);
}

int char_data::getSpeedPercentage()
{
    sbit16 s = m_points.getSpeed();

    if (s < SPEED_MIN)
    {
        s = SPEED_MIN;
    }

    if (s == SPEED_DEFAULT)
    {
        return 100;
    }

    if (getUnitType() == UNIT_ST_PC)
    {
        return std::max(std::min(200, (100 * SPEED_DEFAULT) / s), 25);
    }
    return std::max(std::min(300, (100 * SPEED_DEFAULT) / s), 25);
}

ubit8 char_data::getNaturalArmor() const
{
    return m_points.getNaturalArmor();
}

void char_data::setNaturalArmor(ubit8 value)
{
    m_points.setNaturalArmor(value);
}

ubit8 *char_data::getNaturalArmorPtr()
{
    return m_points.getNaturalArmorPtr();
}

ubit8 char_data::getAttackType() const
{
    return m_points.getAttackType();
}

void char_data::setAttackType(ubit8 value)
{
    m_points.setAttackType(value);
}

ubit8 *char_data::getAttackTypePtr()
{
    return m_points.getAttackTypePtr();
}

ubit8 char_data::getSex() const
{
    return m_points.getSex();
}

void char_data::setSex(ubit8 value)
{
    m_points.setSex(value);
}

ubit8 *char_data::getSexPtr()
{
    return m_points.getSexPtr();
}

ubit8 char_data::getLevel() const
{
    return m_points.getLevel();
}

void char_data::setLevel(ubit8 value)
{
    m_points.setLevel(value);
}

ubit8 *char_data::getLevelPtr()
{
    return m_points.getLevelPtr();
}

void char_data::incrementLevel()
{
    m_points.incrementLevel();
}

ubit8 char_data::getPosition() const
{
    return m_points.getPosition();
}

void char_data::setPosition(ubit8 value)
{
    m_points.setPosition(value);
}

ubit8 *char_data::getPositionPtr()
{
    return m_points.getPositionPtr();
}

sbit16 char_data::getAbilityAtIndex(size_t index) const
{
    return m_points.getAbilityAtIndex(index);
}

sbit16 *char_data::getAbilityAtIndexPtr(size_t index)
{
    return m_points.getAbilityAtIndexPtr(index);
}

sbit16 char_data::getSTR() const
{
    return m_points.getSTR();
}

sbit16 char_data::getDEX() const
{
    return m_points.getDEX();
}

sbit16 char_data::getCON() const
{
    return m_points.getCON();
}

sbit16 char_data::getCHA() const
{
    return m_points.getCHA();
}

sbit16 char_data::getBRA() const
{
    return m_points.getBRA();
}

sbit16 char_data::getMAG() const
{
    return m_points.getMAG();
}

sbit16 char_data::getDIV() const
{
    return m_points.getDIV();
}

sbit16 char_data::getHPP() const
{
    return m_points.getHPP();
}

void char_data::setSTR(sbit16 value)
{
    m_points.setSTR(value);
}

void char_data::setDEX(sbit16 value)
{
    m_points.setDEX(value);
}

void char_data::setCON(sbit16 value)
{
    m_points.setCON(value);
}

void char_data::setCHA(sbit16 value)
{
    m_points.setCHA(value);
}

void char_data::setBRA(sbit16 value)
{
    m_points.setBRA(value);
}

void char_data::setMAG(sbit16 value)
{
    m_points.setMAG(value);
}

void char_data::setDIV(sbit16 value)
{
    m_points.setDIV(value);
}

void char_data::setHPP(sbit16 value)
{
    m_points.setHPP(value);
}

void char_data::setAbilityAtIndexTo(size_t index, sbit16 value)
{
    m_points.setAbilityAtIndexTo(index, value);
}

void char_data::increaseAbilityAtIndexBy(size_t index, sbit16 value)
{
    m_points.increaseAbilityAtIndexBy(index, value);
}

void char_data::decreaseAbilityAtIndexBy(size_t index, sbit16 value)
{
    m_points.decreaseAbilityAtIndexBy(index, value);
}

std::array<sbit16, ABIL_TREE_MAX> &char_data::getAbilitiesArray()
{
    return m_points.getAbilitiesArray();
}

const char_follow_type *char_data::getFollowers() const
{
    return m_followers;
}

char_follow_type *char_data::getFollowers()
{
    return m_followers;
}

void char_data::setFollowers(char_follow_type *value)
{
    m_followers = value;
}

const char *char_data::getLastAttacker() const
{
    return m_last_attacker;
}

void char_data::freeLastAttacker()
{
    if (m_last_attacker)
    {
        FREE(m_last_attacker);
    }
}

void char_data::setLastAttacker(char *value)
{
    m_last_attacker = value;
}

const char *char_data::getMoney() const
{
    return m_money;
}

char *char_data::getMoney()
{
    return m_money;
}

char **char_data::getMoneyPtr()
{
    return &m_money;
}

void char_data::freeMoney()
{
    FREE(m_money);
}

void char_data::setMoney(char *value)
{
    m_money = value;
}

ubit8 char_data::getLastAttackerType() const
{
    return m_last_attacker_type;
}

void char_data::setLastAttackerType(ubit8 value)
{
    m_last_attacker_type = value;
}

void char_data::toJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const
{
    writer.StartObject();
    {
        json::write_unit_id_kvp("id", this, writer);

        writer.String("char_data");
        writer.StartObject();
        {
            json::write_object_pointer_kvp("descriptor", m_descriptor, writer);
            json::write_object_pointer_kvp("combat", m_combat, writer);
            json::write_unit_id_kvp("master", m_master, writer);
            json::write_unit_id_kvp("last_room", m_last_room, writer);

            writer.String("points");
            m_points.toJSON(writer);

            writer.String("Followers");
            auto follow = m_followers;
            writer.StartArray();
            while (follow)
            {
                follow->toJSON(writer);
                follow = follow->getNext();
            }
            writer.EndArray();

            json::write_char_pointer_kvp("last_attacker", m_last_attacker, writer);
            json::write_char_pointer_kvp("money", m_money, writer);
            json::write_kvp("last_attacker_type", m_last_attacker_type, writer);
            json::write_kvp("hit_limit", hit_limit(const_cast<char_data *>(this)), writer);
            json::write_kvp("hit_gain", hit_gain(const_cast<char_data *>(this)), writer);
        }
        writer.EndObject();

        writer.String("unit_data");
        unit_data::toJSON(writer);
    }
    writer.EndObject();
}
