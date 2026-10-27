/*
 $Author: tperry $
 $RCSfile: combat.cpp,v $
 $Date: 2001/04/29 03:46:05 $
 $Revision: 2.1 $
 */
#include "combat_packet.h"
#include "comm.h"
#include "descriptor_data.h"
#include "experience.h"
#include "fight.h"
#include "formatter.h"
#include "hookmud.h"
#include "interpreter.h"
#include "json_helper.h"
#include "modify.h"
#include "protocol.h"
#include "skills.h"
#include "slog.h"
#include "textutil.h"
#include "unitfind.h"
#include "utility.h"
#include "utils.h"

#include <algorithm>
#include <vector>

/// The Global Combat List...
cCombatList g_CombatList;

cCombatList::cCombatList()
{
    nMaxTop = 10;

    CREATE(pElems, cCombat *, nMaxTop);

    nTop = 0;
    nIdx = -1;
}

cCombatList::~cCombatList()
{
    FREE(pElems);

    nMaxTop = 0;
    nTop = 0;
}

void cCombatList::add(cCombat *Combat)
{
    if (nTop >= nMaxTop)
    {
        nMaxTop += 10;
        RECREATE(pElems, cCombat *, nMaxTop);
    }

    pElems[nTop++] = Combat;
}

void cCombatList::sub(cCombat *pc)
{
    for (int i = 0; i < nTop; i++)
    {
        if (pElems[i] == pc)
        {
            if (nTop - i > 1)
            {
                memmove(&pElems[i], &pElems[i + 1], sizeof(cCombat *) * (nTop - i - 1));
            }

            pElems[nTop - 1] = nullptr;

            nTop--;

            if (nIdx != -1) /* Uh oh, we are running through the list... */
            {
                if (i < nIdx)
                {
                    nIdx--;
                }
            }
        }
    }
}

static int combat_compare(const void *v1, const void *v2)
{
    cCombat *e1 = *((cCombat **)v1);
    cCombat *e2 = *((cCombat **)v2);

    if (e1->When() > e2->When())
    {
        return +1;
    }
    else if (e1->When() < e2->When())
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

void cCombatList::Sort()
{
    if (nTop > 0)
    {
        qsort(pElems, nTop, sizeof(cCombat *), combat_compare);
    }
}

// Need to document here how character speed, weapons speed, combat command speed
// works together. I've looked at it for an hour and it remains elusive... :-)
//
// This is what it says in vme.h
//   The combat speed is 12 by default and is lower when faster.
//   4 is the fastest speed and means 3 attacks per combat round.
//   200 is slowest and means 1 attack per 200/12 combat rounds.
//   SPEED_DEFAULT is also the same as the duration of one combat round.
//
// So think this is how it works.
//   Characters has default speed of 12. Haste will make this number
//   lower (4 minimum).
//
//   In each combat round, first all combatants have their actions set to zero.
//     Then we loop through all combatants, each combat action will add to their
//     action count.
//     Then we repeat the loop until everyone has spent at least 12 actions
//
//     If a character is hasted, their CHAR_SPEED is lower. And they'll spend
//     less actions on a melee attack.
//
//     The number of actions consumed by an action such as kick happens in the
//     interpreter; the CHAR_SPEED is unrelated to this.
//
//     The speed of a weapon doesn't currenty seem to impact these actions.
//
//      It is a little bit odd ...
//
void cCombatList::PerformViolence()
{
    if (nTop < 1)
    {
        return;
    }

    Sort();

    // Happens just ONCE per turn, give everybody 12 actions...
    for (nIdx = 0; nIdx < nTop; nIdx++)
    {
        pElems[nIdx]->nAttackNo = 0;
        pElems[nIdx]->nRound++;
        // if (pElems[nIdx]->nWhen > 0)  -- Removed, I don't understand it's purpose
        pElems[nIdx]->nWhen = MAX(0, pElems[nIdx]->nWhen - SPEED_DEFAULT);
        // send_to_char("Combat begin<br/>", pElems[nIdx]->pOwner);
    }

    bool bAnyaction;

    do
    {
        bAnyaction = false;

        for (nIdx = 0; nIdx < nTop; nIdx++)
        {
            if (pElems[nIdx]->nWhen >= SPEED_DEFAULT)
            {
                break; // The rest are larger... Always sorted, see end of while
            }

            cCombat *tmp = pElems[nIdx]; // In case element is destroyed

            if (pElems[nIdx]->cmd[0]) // Execute a combat command...
            {
                bAnyaction = true;

                int t = pElems[nIdx]->nWhen;

                char *c = str_dup(pElems[nIdx]->cmd);

                /*std::string s;
                s = "Combat before command ";
                s.append(pElems[nIdx]->pOwner->names.Name());
                s.append(" ");
                s.append(itoa(pElems[nIdx]->nWhen));
                s.append("<br/>");
                send_to_char(s, pElems[nIdx]->pOwner);*/

                pElems[nIdx]->cmd[0] = 0;
                command_interpreter(pElems[nIdx]->pOwner, c);

                /*if (pElems[nIdx] == tmp)
                {
                    s = "Combat after command ";
                    s.append(pElems[nIdx]->pOwner->names.Name());
                    s.append(" ");
                    s.append(itoa(pElems[nIdx]->nWhen));
                    s.append("<br/>");
                    send_to_char(s, pElems[nIdx]->pOwner);
                }*/

                if ((tmp == pElems[nIdx]) && (t == tmp->nWhen))
                    slog(LOG_ALL, 0, "Combat action %s didn't use any combat time", c);

                FREE(c);
            }
            else
            {
                if (CHAR_FIGHTING(pElems[nIdx]->pOwner))
                {
                    pElems[nIdx]->nAttackNo++;
                    if (char_dual_wield(pElems[nIdx]->pOwner))
                    {
                        int n = UCHAR(pElems[nIdx]->pOwner)->getNumberOfMeleeAttacks(2);

                        unit_data *pWeapon = nullptr;
                        int nWeaponType;
                        int nWeaponSpeed;

                        getWeapon(pElems[nIdx]->pOwner, &pWeapon, &nWeaponType, &nWeaponSpeed, (pElems[nIdx]->nAttackNo & 1));
                        int actions = pElems[nIdx]->calculateHastedActions(nWeaponSpeed, UCHAR(pElems[nIdx]->pOwner)->getSpeedPercentage());

                        if ((pElems[nIdx]->nAttackNo <= n) && (pElems[nIdx]->nWhen + actions <= SPEED_DEFAULT))
                        {
                            /* std::string s;
                            s = "Combat before dual wield melee ";
                            s.append(pElems[nIdx]->pOwner->names.Name());
                            s.append(" ");
                            s.append(itoa(pElems[nIdx]->nWhen));
                            s.append("<br/>");
                            send_to_char(s, pElems[nIdx]->pOwner);*/

                            bAnyaction = true;
                            melee_violence(pElems[nIdx]->pOwner, (pElems[nIdx]->nAttackNo & 1));

                            /* if (pElems[nIdx] == tmp)
                            {
                                s = "Combat after dual wield melee ";
                                s.append(pElems[nIdx]->pOwner->names.Name());
                                s.append(" ");
                                s.append(itoa(pElems[nIdx]->nWhen));
                                s.append("<br/>");
                                send_to_char(s, pElems[nIdx]->pOwner);
                            }*/

                            // if ((tmp == pElems[nIdx]) && (t == tmp->nWhen))
                            //     slog(LOG_ALL, 0, "Dual wield melee action didn't use any combat time");

                            // if ((nIdx != -1) && (nIdx < nTop) && (tmp == pElems[nIdx]))
                            //   tmp->changeSpeed(SPEED_DEFAULT / 2, IS_PC(tmp->pOwner)); // Use weapon speed instead
                        }
                    }
                    else
                    {
                        int n = UCHAR(pElems[nIdx]->pOwner)->getNumberOfMeleeAttacks(1);

                        unit_data *pWeapon = nullptr;
                        int nWeaponType;
                        int nWeaponSpeed;

                        getWeapon(pElems[nIdx]->pOwner, &pWeapon, &nWeaponType, &nWeaponSpeed, (pElems[nIdx]->nAttackNo & 1));
                        int actions = pElems[nIdx]->calculateHastedActions(nWeaponSpeed, UCHAR(pElems[nIdx]->pOwner)->getSpeedPercentage());

                        if ((pElems[nIdx]->nAttackNo <= n) && (pElems[nIdx]->nWhen + actions <= SPEED_DEFAULT))
                        {
                            bAnyaction = true;
                            /* std::string s;
                            s = "Combat before normal wield melee ";
                            s.append(pElems[nIdx]->pOwner->names.Name());
                            s.append(" ");
                            s.append(itoa(pElems[nIdx]->nWhen));
                            s.append("<br/>");
                            send_to_char(s, pElems[nIdx]->pOwner);*/

                            melee_violence(pElems[nIdx]->pOwner, TRUE);

                            /* if (pElems[nIdx] == tmp)
                            {
                                s = "Combat after normal wield melee ";
                                s.append(pElems[nIdx]->pOwner->names.Name());
                                s.append(" ");
                                s.append(itoa(pElems[nIdx]->nWhen));
                                s.append("<br/>");
                                send_to_char(s, pElems[nIdx]->pOwner);
                            }*/

                            // if ((tmp == pElems[nIdx]) && (t == tmp->nWhen))
                            //     slog(LOG_ALL, 0, "Normal melee  action didn't use any combat time");
                            //  if ((nIdx != -1) && (nIdx < nTop) && (tmp == pElems[nIdx]))
                            //     tmp->changeSpeed(SPEED_DEFAULT, IS_PC(tmp->pOwner));
                        }
                    }
                }
            }
        } // for

        if (bAnyaction)
            Sort();
    } while (bAnyaction && nTop > 0 && pElems[0]->nWhen < SPEED_DEFAULT);

    // for (nIdx = 0; nIdx < nTop; nIdx++) { send_to_char("Combat end<br/>", pElems[nIdx]->pOwner); }

    // The round is over: tell web clients where their fight stands.
    // Sending never changes the list, so a plain loop is safe here.
    for (nIdx = 0; nIdx < nTop; nIdx++)
    {
        pElems[nIdx]->sendWebRound();
    }
    for (nIdx = 0; nIdx < nTop; nIdx++)
    {
        pElems[nIdx]->clearRoundBlows();
    }

    nIdx = -1;
}

void cCombatList::status(const unit_data *ch)
{
    auto msg = diku::format_to_str("The Global Combat List contains [%d] entries.<br/>", nTop);
    send_to_char(msg, ch);
}

/* ======================================================================= */
/*                                                                         */
/*                    Web client '$combat' packets                         */
/*                                                                         */
/* ======================================================================= */

/// ch's descriptor if it is a web client (websocket mplex), else nullptr.
/// Telnet clients have no use for the packets (StripHTML() drops them).
static descriptor_data *web_descriptor(unit_data *ch)
{
    if (ch == nullptr || !ch->isChar() || ch->is_destructed())
    {
        return nullptr;
    }

    descriptor_data *d = CHAR_DESCRIPTOR(ch);

    if (d == nullptr || d->getMultiHookPtr() == nullptr || !d->getMultiHookPtr()->bWebsockets)
    {
        return nullptr;
    }

    return d;
}

/// Sends a '$combat' packet straight to ch's web client. Deliberately not
/// send_to_descriptor(): that would echo it to a (maybe telnet) snooper.
static void web_send(unit_data *ch, const std::string &json)
{
    descriptor_data *d = web_descriptor(ch);

    if (d)
    {
        std::string packet = combat_packet::wrap(json);
        protocol_send_text(d->getMultiHookPtr(), d->getMultiHookID(), packet.c_str(), MULTI_TEXT_CHAR);
    }
}

static bool contains(const std::vector<unit_data *> &v, const unit_data *u)
{
    return std::find(v.begin(), v.end(), u) != v.end();
}

/// True if a and b are both grouped under the same leader (see the group command)
static bool web_same_group(unit_data *a, unit_data *b)
{
    if (!CHAR_HAS_FLAG(a, CHAR_GROUP) || !CHAR_HAS_FLAG(b, CHAR_GROUP))
    {
        return false;
    }

    unit_data *la = CHAR_MASTER(a) ? CHAR_MASTER(a) : a;
    unit_data *lb = CHAR_MASTER(b) ? CHAR_MASTER(b) : b;

    return la == lb;
}

/// The others in viewer's fight, in the same room: foes (viewer's opponents,
/// anyone meleeing viewer, anyone an ally fights) and allies (group members in
/// combat, and those fighting one of viewer's opponents). At most
/// MAX_ENTRIES - 1 in total, foes first.
static void web_combat_members(unit_data *viewer, std::vector<unit_data *> &foes, std::vector<unit_data *> &allies)
{
    unit_data *room = viewer->getUnitIn();
    cCombat *pc = CHAR_COMBAT(viewer);

    if (room == nullptr || pc == nullptr)
    {
        return;
    }

    auto addFoe = [&](unit_data *u) {
        if (u && u != viewer && u->getUnitIn() == room && !u->is_destructed() && !contains(foes, u) && !contains(allies, u))
        {
            foes.push_back(u);
        }
    };

    for (int i = 0; i < pc->NoOpponents(); i++)
    {
        addFoe(pc->Opponent(i));
    }

    const size_t nOpponents = foes.size();

    for (unit_data *u = room->getUnitContains(); u; u = u->getNext())
    {
        if (u == viewer || !u->isChar() || u->is_destructed() || CHAR_COMBAT(u) == nullptr || contains(foes, u))
        {
            continue;
        }

        if (CHAR_FIGHTING(u) == viewer)
        {
            foes.push_back(u);
            continue;
        }

        if (web_same_group(viewer, u))
        {
            allies.push_back(u); // a group fights together, whatever each member is hitting
            continue;
        }

        cCombat *uc = CHAR_COMBAT(u);

        for (int i = 0; i < uc->NoOpponents(); i++)
        {
            if (std::find(foes.begin(), foes.begin() + nOpponents, uc->Opponent(i)) != foes.begin() + nOpponents)
            {
                allies.push_back(u);
                break;
            }
        }
    }

    // Whatever the allies fight here is hostile too
    for (auto *a : allies)
    {
        cCombat *ac = CHAR_COMBAT(a);

        for (int i = 0; i < ac->NoOpponents(); i++)
        {
            addFoe(ac->Opponent(i));
        }
    }

    const size_t nMaxOthers = combat_packet::MAX_ENTRIES - 1;

    if (foes.size() > nMaxOthers)
    {
        foes.resize(nMaxOthers);
    }

    if (allies.size() > nMaxOthers - foes.size())
    {
        allies.resize(nMaxOthers - foes.size());
    }
}

static combat_packet::Entry web_combat_entry(unit_data *viewer, unit_data *u, bool bFoe)
{
    combat_packet::Entry e;

    e.id = combat_packet::opaqueId(u);
    e.me = (u == viewer);
    e.name = e.me ? "you" : UNIT_SEE_TITLE(viewer, u);
    e.hp = combat_packet::hpPercent(u->getCurrentHitpoints(), u->getMaximumHitpoints());
    e.foe = bFoe;

    if (CHAR_COMBAT(u))
    {
        e.byYou = CHAR_COMBAT(u)->roundBlowSteps(viewer, true);
        e.byOthers = CHAR_COMBAT(u)->roundBlowSteps(viewer, false);
    }

    unit_data *t = CHAR_FIGHTING(u);

    if (t == viewer)
    {
        e.target = "you";
    }
    else if (t)
    {
        e.target = UNIT_SEE_TITLE(viewer, t);
    }

    return e;
}

void cCombat::addRoundBlow(const unit_data *att, int step)
{
    if (vRoundBlows.size() < 32) // haste and a crowd, but no runaway list
    {
        vRoundBlows.emplace_back(att, step);
    }
}

std::vector<int> cCombat::roundBlowSteps(const unit_data *viewer, bool byViewer) const
{
    std::vector<int> steps;

    for (const auto &b : vRoundBlows)
    {
        if ((b.first == viewer) == byViewer)
        {
            steps.push_back(b.second);
        }
    }
    return steps;
}

void cCombat::sendWebRound()
{
    if (web_descriptor(pOwner) == nullptr)
    {
        return;
    }

    std::vector<unit_data *> foes;
    std::vector<unit_data *> allies;
    web_combat_members(pOwner, foes, allies);

    // Nobody to fight here (e.g. the owner was moved to another room while its
    // combat goes on): to the web client the fight is over.
    if (foes.empty())
    {
        if (bWebSent)
        {
            web_send(pOwner, combat_packet::endJSON(nRound));
            bWebSent = false;
        }
        return;
    }

    std::vector<combat_packet::Entry> list;
    list.reserve(foes.size() + allies.size() + 1);

    for (auto *u : foes)
    {
        list.push_back(web_combat_entry(pOwner, u, true));
    }

    for (auto *u : allies)
    {
        list.push_back(web_combat_entry(pOwner, u, false));
    }

    list.push_back(web_combat_entry(pOwner, pOwner, false));

    web_send(pOwner, combat_packet::listJSON(nRound, list));
    bWebSent = true;
}

/// ch is dying: tell the web clients in the room that have ch in their
/// '$combat' list. Only looks at the room ch is in. Call it while the fight
/// is still standing (see the kill path in damage()).
void combat_web_death(unit_data *ch)
{
    if (ch == nullptr || !ch->isChar() || ch->getUnitIn() == nullptr)
    {
        return;
    }

    std::string json;

    for (unit_data *u = ch->getUnitIn()->getUnitContains(); u; u = u->getNext())
    {
        if (u == ch || !u->isChar() || CHAR_COMBAT(u) == nullptr || web_descriptor(u) == nullptr)
        {
            continue;
        }

        std::vector<unit_data *> foes;
        std::vector<unit_data *> allies;
        web_combat_members(u, foes, allies);

        if (contains(foes, ch) || contains(allies, ch))
        {
            if (json.empty())
            {
                json = combat_packet::deathJSON(combat_packet::opaqueId(ch));
            }
            web_send(u, json);
        }
    }
}

/* ======================================================================= */
/*                                                                         */
/*                         The Combat Element                              */
/*                                                                         */
/* ======================================================================= */

cCombat::cCombat(unit_data *owner, int bMelee)
{
    assert(owner);

    pOwner = owner;
    pMelee = nullptr;

    nWhen = 0;
    cmd[0] = 0;

    pOpponents = nullptr;
    nNoOpponents = 0;

    g_CombatList.add(this);
}

cCombat::~cCombat()
{
    while (nNoOpponents > 0)
    {
        subOpponent(pOpponents[nNoOpponents - 1]); // Faster sub at tail
    }

    if (pOpponents)
    {
        FREE(pOpponents);
        pOpponents = nullptr;
    }

    if (bWebSent)
    {
        web_send(pOwner, combat_packet::endJSON(nRound));
    }

    UCHAR(pOwner)->setCombat(nullptr);

    g_CombatList.sub(this);

    UCHAR(pOwner)->setPosition(POSITION_STANDING);
    update_pos(pOwner);

    pOwner = nullptr;
}

void cCombat::setCommand(const char *arg)
{
    strncpy(cmd, arg, MAX_INPUT_LENGTH);
    cmd[MAX_INPUT_LENGTH] = 0;
}

int cCombat::calculateHastedActions(int actions, int hastePercentage)
{
    if (!is_in(hastePercentage, 25, 300))
    {
        slog(LOG_ALL, 0, "hastePercentage is invalid", hastePercentage);
        hastePercentage = 100;
    }

    if (hastePercentage == 100)
    {
        return MAX(4, actions);
    }
    else
    {
        return MAX(ceil((4 * 100.0) / hastePercentage), ceil((actions * 100.0) / hastePercentage));
    }
}

void cCombat::changeSpeed(int actions, int speedPercentage)
{
    nWhen += calculateHastedActions(actions, speedPercentage);
}

int cCombat::findOpponentIdx(unit_data *target)
{
    for (int i = 0; i < nNoOpponents; i++)
    {
        if (pOpponents[i] == target)
        {
            return i;
        }
    }

    return -1;
}

unit_data *cCombat::FindOpponent(unit_data *victim)
{
    int i = findOpponentIdx(victim);

    if (i == -1)
    {
        return nullptr;
    }
    else
    {
        return pOpponents[i];
    }
}

void cCombat::add(unit_data *victim)
{
    assert(victim);

    nNoOpponents++;

    if (nNoOpponents == 1)
    {
        CREATE(pOpponents, unit_data *, 1);
    }
    else
    {
        RECREATE(pOpponents, unit_data *, nNoOpponents);
    }
    pOpponents[nNoOpponents - 1] = victim;
}

void cCombat::sub(int idx)
{
    int bWas = FALSE;

    if (idx == -1)
    {
        return;
    }

    assert(nNoOpponents > 0);
    assert(idx >= 0 && idx < nNoOpponents);

    // Never mind about realloc, it will be free'd soon anyhow... how long
    // can a combat take anyway?

    if (pOpponents[idx] == pMelee)
    {
        pMelee = nullptr;
        bWas = TRUE;
    }

    if (nNoOpponents - idx > 1)
    {
        memmove(&pOpponents[idx], &pOpponents[idx + 1], sizeof(unit_data *) * (nNoOpponents - idx - 1));
    }

    pOpponents[nNoOpponents - 1] = nullptr;
    nNoOpponents--;

    if (nNoOpponents < 1)
    {
        FREE(pOpponents);
        pOpponents = nullptr;
        delete this; // We are done...
    }
    else if (bWas)
    {
        setMelee(Opponent(number(0, nNoOpponents - 1)));
    }
}

void cCombat::setMelee(unit_data *victim)
{
    if (pMelee != victim)
        CHAR_COMBAT(pOwner)->changeSpeed((3 * SPEED_DEFAULT) / 4, 100); // Prevent hasted dual wields from killing 4 / round
    pMelee = victim;
}

/// Add another opponent. A very important feature is, that opponents
/// always exists as pairs and if one is removed so is the other.
void cCombat::addOpponent(unit_data *victim, int bMelee = FALSE)
{
    // This if is needed since we call recursively for the victim

    if (!FindOpponent(victim))
    {
        add(victim);

        if (!CHAR_COMBAT(victim))
        {
            UCHAR(victim)->setCombat(new cCombat(victim, bMelee));
        }

        CHAR_COMBAT(victim)->add(pOwner);
    }

    if (bMelee && pMelee == nullptr)
    {
        setMelee(victim);
    }
}

void cCombat::subOpponent(unit_data *victim)
{
    if (nNoOpponents < 1)
    {
        return;
    }

    int i = findOpponentIdx(victim);

    if (i == -1)
    { // Not found
        return;
    }

    CHAR_COMBAT(victim)->sub(CHAR_COMBAT(victim)->findOpponentIdx(pOwner));
    sub(i);
}

unit_data *cCombat::Opponent(int i)
{
    if (i >= nNoOpponents)
    {
        return nullptr;
    }
    else
    {
        return pOpponents[i];
    }
}

void cCombat::status(const unit_data *god)
{
    auto msg = diku::format_to_str("Combat Status of '%s':<br/>"
                                   "Combat Speed [%d]  Turn [%d]<br/>"
                                   "Melee Opponent '%s'<br/>"
                                   "Total of %d Opponents:<br/><br/>",
                                   STR(pOwner->getNames().Name()),
                                   CHAR_SPEED(pOwner),
                                   nWhen,
                                   CHAR_FIGHTING(pOwner) ? STR(CHAR_FIGHTING(pOwner)->getNames().Name()) : "NONE",
                                   nNoOpponents);

    for (int i = 0; i < nNoOpponents; i++)
    {
        msg += diku::format_to_str("   %s<br/>", STR(pOpponents[i]->getNames().Name()));
    }

    send_to_char(msg, god);
}

void cCombat::toJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const
{
    writer.StartObject();
    {
        json::write_kvp("AttackNo", nAttackNo, writer);
        json::write_kvp("When", nWhen, writer);
        json::write_unit_id_kvp("Owner", pOwner, writer);
        json::write_unit_id_kvp("Melee", pMelee, writer);
        json::write_kvp("NoOpponents", nNoOpponents, writer);

        writer.String("Opponents");
        writer.StartArray();
        for (int i = 0; i < nNoOpponents; ++i)
        {
            json::write_unit_id(pOpponents[i], writer);
        }
        writer.EndArray();

        json::write_kvp("cmd", cmd, writer);
    }
    writer.EndObject();
}

/* ======================================================================= */
/*                                                                         */
/*                         Utility Routines                                */
/*                                                                         */
/* ======================================================================= */

/* start one char fighting another (yes, it is horrible, I know... )  */
void set_fighting(unit_data *ch, unit_data *vict, int bMelee)
{
    if (ch == vict)
    {
        return;
    }

    /* No check for awake! If you die, FIGHTING() is set to point to murderer */

    if (ch->is_destructed() || vict->is_destructed())
    {
        return;
    }

    if (CHAR_COMBAT(ch) == nullptr)
    {
        UCHAR(ch)->setCombat(new cCombat(ch, bMelee));
    }

    CHAR_COMBAT(ch)->addOpponent(vict, bMelee);

    UCHAR(ch)->setPosition(POSITION_FIGHTING);
}

/* start one char fighting another (yes, it is horrible, I know... )  */
void add_fighting(unit_data *ch, unit_data *vict, int bMelee)
{
    if (ch == vict)
    {
        return;
    }

    /* No check for awake! If you die, FIGHTING() is set to point to murderer */

    if (ch->is_destructed() || vict->is_destructed())
    {
        return;
    }

    if (CHAR_COMBAT(ch) == nullptr)
    {
        UCHAR(ch)->setCombat(new cCombat(ch, bMelee));
    }

    CHAR_COMBAT(ch)->addOpponent(vict, bMelee);
}

/* remove a char from the list of fighting chars */
void stop_fighting(unit_data *ch, unit_data *victim)
{
    if (victim == nullptr) // Stop all combat...
    {
        while (CHAR_COMBAT(ch))
        {
            CHAR_COMBAT(ch)->subOpponent(CHAR_COMBAT(ch)->Opponent());
        }
    }
    else
    {
        CHAR_COMBAT(ch)->subOpponent(victim);
    }

    if (CHAR_COMBAT(ch) == nullptr)
    {
        UCHAR(ch)->removeCharacterFlag(CHAR_SELF_DEFENCE);
        UCHAR(ch)->setPosition(POSITION_STANDING);
        update_pos(ch);
    }
}

/* ======================================================================= */
/*                                                                         */
/*                         Status Routines                                 */
/*                                                                         */
/* ======================================================================= */

void stat_combat(unit_data *god, unit_data *u, const char *pStr)
{
    unit_data *u2 = nullptr;

    if (!u->isChar())
    {
        act("$2n is not a pc / npc.", eA_ALWAYS, god, u, cActParameter(), eTO_CHAR);
        return;
    }

    u2 = nullptr;

    if (!str_is_empty(pStr))
    {
        u2 = find_unit(god, (char **)&pStr, nullptr, FIND_UNIT_SURRO);
    }

    if (u2 == nullptr)
    {
        u2 = god;
    }

    if (CHAR_FIGHTING(u))
    {
        u2 = CHAR_FIGHTING(u);
    }

    if (!u2->isChar())
    {
        act("$2n is not a pc / npc.", eA_ALWAYS, god, u2, cActParameter(), eTO_CHAR);
        return;
    }

    g_CombatList.status(god);

    if (!CHAR_COMBAT(u))
    {
        act("No combat structure on '$2n'", eA_ALWAYS, god, u, cActParameter(), eTO_CHAR);
    }
    else
    {
        CHAR_COMBAT(u)->status(god);
    }

    std::string str;
    melee_bonus(u, u2, WEAR_BODY, nullptr, nullptr, nullptr, nullptr, TRUE, &str);
    send_to_char(str.c_str(), god);
    melee_bonus(u2, u, WEAR_BODY, nullptr, nullptr, nullptr, nullptr, TRUE, &str);
    send_to_char(str.c_str(), god);
}

void stat_spell(unit_data *god, unit_data *u, const char *pStr)
{
    unit_data *u2 = nullptr;

    if (!u->isChar())
    {
        act("$2n is not a pc / npc.", eA_ALWAYS, god, u, cActParameter(), eTO_CHAR);
        return;
    }

    u2 = nullptr;

    if (!str_is_empty(pStr))
    {
        u2 = find_unit(god, (char **)&pStr, nullptr, FIND_UNIT_SURRO);
    }

    if (u2 == nullptr)
    {
        u2 = god;
    }

    if (CHAR_FIGHTING(u))
    {
        u2 = CHAR_FIGHTING(u);
    }

    if (!u2->isChar())
    {
        act("$2n is not a pc / npc.", eA_ALWAYS, god, u2, cActParameter(), eTO_CHAR);
        return;
    }

    g_CombatList.status(god);

    if (!CHAR_COMBAT(u))
    {
        act("No combat structure on '$2n'", eA_ALWAYS, god, u, cActParameter(), eTO_CHAR);
    }
    else
    {
        CHAR_COMBAT(u)->status(god);
    }

    int i = search_block(skip_blanks(pStr), g_SplColl.text, false);
    if (i == -1)
    {
        i = SPL_ACID_BREATH;
    }

    std::string str;
    spell_bonus(u, u, u2, WEAR_BODY, i, nullptr, nullptr, &str);
    send_to_char(str.c_str(), god);
    spell_bonus(u2, u2, u, WEAR_BODY, i, nullptr, nullptr, &str);
    send_to_char(str.c_str(), god);
}
