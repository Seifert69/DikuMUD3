/*
 $Author: All $
 $RCSfile: fight.cpp,v $
 $Date: 2003/11/29 03:31:16 $
 $Revision: 2.6 $
 */

#include "fight.h"

#include "affect.h"
#include "char_follow_type.h"
#include "combat.h"
#include "comm.h"
#include "common.h"
#include "db.h"
#include "dilrun.h"
#include "experience.h"
#include "files.h"
#include "handler.h"
#include "interpreter.h"
#include "justice.h"
#include "skills.h"
#include "slog.h"
#include "textutil.h"
#include "unit_affected_type.h"
#include "unit_fptr.h"
#include "utils.h"
#include "vmelimits.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

/* Structures */

struct combat_single_msg
{
    char *to_char;
    char *to_vict;
    char *to_notvict;
};

struct combat_msg_packet
{
    combat_single_msg cmiss;
    combat_single_msg shield;
    combat_single_msg nodam;
    combat_single_msg entire;
    combat_single_msg head;
    combat_single_msg hand;
    combat_single_msg arm;
    combat_single_msg body;
    combat_single_msg legs;
    combat_single_msg feet;
    combat_single_msg die;

    combat_msg_packet *next;
};

struct combat_msg_list
{
    int group;      /* Which message group      */
    int *no;        /* Which message number #'s */
    int no_of_msgs; /* # of msgs to choose from */
    combat_msg_packet *msg;
};

combat_msg_list fight_messages[COM_MAX_MSGS];

/* Returns TRUE if combat is not allowed, and FALSE if combat is allowed */
/* Also used for steal, etc.                                             */
/* If message is true, then a message is sent to the 'att'               */

int pk_test(unit_data *att, unit_data *def, int message)
{
    if (att->isPC() && def->isPC() && att != def)
    {
        if (CHAR_LEVEL(att) <= 3)
        {
            if (message)
            {
                act("You are not old enough to do that!", A_ALWAYS, att, cActParameter(), def, TO_CHAR);
            }
            return TRUE;
        }
        if (CHAR_LEVEL(def) <= 3)
        {
            if (message)
            {
                act("$3e is too young to die now!", A_ALWAYS, att, cActParameter(), def, TO_CHAR);
            }
            return TRUE;
        }
    }

    if (!g_cServerConfig.isBOB())
    {
        return FALSE;
    }

    if ((att != def) && att->isPC() && def->isPC() && (!IS_SET(PC_FLAGS(att), PC_PK_RELAXED) || !IS_SET(PC_FLAGS(def), PC_PK_RELAXED)))
    {
        /* Ok, he's allowed to attack only if the other guy is rewarded! */
        /* if (affected_by_spell(def, ID_REWARD))
             return FALSE; */

        if (message)
        {
            if (!IS_SET(PC_FLAGS(att), PC_PK_RELAXED))
            {
                act("You are not allowed to do this unless you sign the "
                    "book of blood.",
                    A_ALWAYS,
                    att,
                    cActParameter(),
                    def,
                    TO_CHAR);
            }
            else
            {
                act("You are not allowed to do this until $3n signs the "
                    "book of blood.",
                    A_ALWAYS,
                    att,
                    cActParameter(),
                    def,
                    TO_CHAR);
            }
        }
        return TRUE;
    }

    return FALSE;
}

int char_dual_wield(unit_data *ch)
{
    return equipment_type(ch, WEAR_WIELD, ITEM_WEAPON) && equipment_type(ch, WEAR_HOLD, ITEM_WEAPON);
}

// returns true if obj is a slaying weapon against opponent_race
bool is_obj_slaying(unit_data *obj, ubit16 opponent_race)
{
    if (obj && obj->isObj() && (OBJ_TYPE(obj) == ITEM_WEAPON) && (OBJ_VALUE(obj, 3) != RACE_DO_NOT_USE))
    {
        if (OBJ_VALUE(obj, 3) == opponent_race)
        {
            return true;
        }

        extra_descr_data *exd = obj->getExtraList().find_raw("$slayer");

        if (exd == nullptr)
        {
            return false;
        }

        for (int i = 0; i + 1 < exd->vals.Length(); i += 2)
        {
            if ((opponent_race >= exd->vals.Value(i) && (opponent_race <= exd->vals.Value(i + 1))))
            {
                return true;
            }
        }
    }

    return false;
}

/* Given an amount of experience, what is the 'virtual' level of the char? */
int virtual_level(unit_data *ch)
{
    if (ch->isNPC())
    {
        return CHAR_LEVEL(ch);
    }
    else
    {
        if (IS_MORTAL(ch))
        {
            /* Above max mortal is just max mortal (shouldnt be possible) */
            if (CHAR_LEVEL(ch) >= MORTAL_MAX_LEVEL)
            {
                return CHAR_LEVEL(ch);
            }

            if (CHAR_EXP(ch) < required_xp(1 + PC_VIRTUAL_LEVEL(ch)))
            {
                return CHAR_LEVEL(ch);
            }
            else
            {
                return CHAR_LEVEL(ch) + 1 + (CHAR_EXP(ch) - required_xp(1 + CHAR_LEVEL(ch))) / level_xp(CHAR_LEVEL(ch));
            }
        }
        else
        {
            return CHAR_LEVEL(ch);
        }
    }
}

/* -------------------------------------------------------------------- */

int num_in_msg(combat_msg_list *msg, int no)
{
    int i = 0;

    for (i = 0; msg->no[i] != -1; i++)
    {
        if (msg->no[i] == no)
        {
            return TRUE;
        }
    }

    return FALSE;
}

void fread_single(FILE *f1, combat_single_msg *msg)
{
    msg->to_char = fread_string(f1);
    msg->to_vict = fread_string(f1);
    msg->to_notvict = fread_string(f1);
    int mstmp = fscanf(f1, " ");
    if (mstmp < 0)
    {
        slog(LOG_ALL, 0, "ERROR: Unexpected end of stream %d in fight.cpp", mstmp);
        assert(FALSE);
    }
}

int load_msg_prehead(FILE *f1, combat_msg_list *msg)
{
    int no[200];
    int pos = 0;
    int grp = 0;
    char shit[100];

    /* A group of -1 indicates end */
    int mstmp = fscanf(f1, " %d", &grp);
    if (mstmp < 0)
    {
        slog(LOG_ALL, 0, "ERROR: Unexpected end of stream %d in fight.cpp", mstmp);
        assert(FALSE);
    }

    if (grp == -1)
    {
        return -1;
    }

    pos = 0;

    do
    {
        mstmp = fscanf(f1, " %d", &no[pos++]);
        if (mstmp < 1)
        {
            slog(LOG_ALL, 0, "ERROR: Unexpected end of stream %d in fight.cpp", mstmp);
            assert(FALSE);
        }
    } while ((no[pos - 1] != -1) && (pos < 199));

    no[pos - 1] = -1;

    mstmp = fscanf(f1, " %[^\n\r] ", shit); /* Skip any remark */
    if (mstmp < 0)
    {
        slog(LOG_ALL, 0, "ERROR: Unexpected end of stream %d in fight.cpp", mstmp);
        assert(FALSE);
    }

    CREATE(msg->no, int, pos);
    for (pos--; pos >= 0; pos--)
    {
        msg->no[pos] = no[pos];
    }

    return grp;
}

void load_messages()
{
    FILE *f1 = nullptr;
    int i = 0;
    int grp = 0;
    combat_msg_packet *messages = nullptr;

    if (!(f1 = fopen(g_cServerConfig.getFileInEtcDir(MESS_FILE).c_str(), "r")))
    {
        slog(LOG_ALL, 0, "Unable to read fight messages.");
        exit(11);
    }

    char shit[200];
    fread_line_commented(f1, shit, sizeof(shit));

    for (i = 0; i < COM_MAX_MSGS; i++)
    {
        fight_messages[i].group = -1;
        fight_messages[i].no = nullptr; /* Nil */
        fight_messages[i].no_of_msgs = 0;
        fight_messages[i].msg = nullptr;
    }

    i = 0;
    while (TRUE)
    {
        grp = load_msg_prehead(f1, &fight_messages[i]);

        if (grp == -1)
        {
            break;
        }

        if (i >= COM_MAX_MSGS)
        {
            slog(LOG_OFF, 0, "Too many combat messages.");
            exit(12);
        }

        CREATE(messages, combat_msg_packet, 1);

        fight_messages[i].no_of_msgs++;
        fight_messages[i].group = grp;
        /* fight_messages[i].no already set in load_msg_prehead */
        messages->next = fight_messages[i].msg;
        fight_messages[i].msg = messages;

        fread_single(f1, &(messages->cmiss));
        fread_single(f1, &(messages->shield));
        fread_single(f1, &(messages->nodam));
        fread_single(f1, &(messages->entire));
        fread_single(f1, &(messages->head));
        fread_single(f1, &(messages->hand));
        fread_single(f1, &(messages->arm));
        fread_single(f1, &(messages->body));
        fread_single(f1, &(messages->legs));
        fread_single(f1, &(messages->feet));
        fread_single(f1, &(messages->die));

        i++;
    }

    fclose(f1);
}

/* -------------------------------------------------------------------- */

/* Returns index in [0..8] */
int damage_index(int dam, int maxhp)
{
    int p = 0;

    if (dam < 1)
    {
        return 0;
    }
    else if (dam >= maxhp)
    {
        return 8;
    }

    p = (100 * dam) / maxhp; /* Calculate relative damage (%) */

    if (p < 5)
    {
        return 0;
    }
    else if (p < 10)
    {
        return 1;
    }
    else if (p < 15)
    {
        return 2;
    }
    else if (p < 20)
    {
        return 3;
    }
    else if (p < 25)
    { /* 20%..25% is default at own level */
        return 4;
    }
    else if (p < 35)
    {
        return 5;
    }
    else if (p < 50)
    {
        return 6;
    }
    else if (p < 70)
    {
        return 7;
    }
    else
    {
        return 8;
    }
}

char *sub_damage(char *str, int damage, int max_hp)
{
    static char buf[256];
    char *cp = nullptr;

    /* We can always make more/new blocks */
    const char *damage_blocks[16][9] = {{"ineptly", /*       A       */
                                         "awkwardly",
                                         "clumsily",
                                         "simply", /* How it appears/looks */
                                         "competently",
                                         "smoothly",
                                         "elegantly",
                                         "expertly",
                                         "superbly"},

                                        {"meekly", /*       B      */
                                         "tamely",
                                         "gently",
                                         "moderately", /* The temper/mood of the attacker */
                                         "boldly",
                                         "cruelly",
                                         "grimly",
                                         "viciously",
                                         "savagely"},

                                        {"caress", /*       C       */
                                         "tickle",
                                         "scratch",
                                         "cut", /*   Cut and Slash Weaponry 1 */
                                         "chop",
                                         "gash",
                                         "lacerate",
                                         "mangle",
                                         "mutilate"},

                                        {"caresses", /*       D       */
                                         "tickles",
                                         "scratches",
                                         "cuts", /*   Cut and Slash Weaponry 2 */
                                         "chops",
                                         "gashes",
                                         "lacerates",
                                         "mangles",
                                         "mutilates"},

                                        {"caressed", /*       E       */
                                         "tickled",
                                         "scratched",
                                         "cut", /*   Cut and Slash Weaponry 1 */
                                         "chopped",
                                         "gashed",
                                         "lacerated",
                                         "mangled",
                                         "mutilated"},

                                        {"comb", /*       F       */
                                         "shave",
                                         "scratch",
                                         "rip", /*   Cut and Slash Weaponry 3 */
                                         "slash",
                                         "rend",
                                         "incise",
                                         "rupture",
                                         "mince"},

                                        {"combs", /*       G       */
                                         "shaves",
                                         "scratches",
                                         "rips", /*   Cut and Slash Weaponry 4 */
                                         "slashes",
                                         "rends",
                                         "incises",
                                         "ruptures",
                                         "minces"},

                                        {"combed", /*       H       */
                                         "shaved",
                                         "scratched",
                                         "ripped", /*   Cut and Slash Weaponry 3 */
                                         "slashed",
                                         "rended",
                                         "incised",
                                         "ruptured",
                                         "minced"},

                                        {"nudge", /*       I       */
                                         "graze",
                                         "whack",
                                         "beat", /*   Bludgeon Weaponry   */
                                         "thump",
                                         "flog",
                                         "pound",
                                         "slam",
                                         "batter"},

                                        {"nudges", /*       J      */
                                         "grazes",
                                         "whacks",
                                         "beats", /*   Bludgeon Weaponry   */
                                         "thumps",
                                         "flogs",
                                         "pounds",
                                         "slams",
                                         "batters"},

                                        {"nudged", /*       K       */
                                         "grazed",
                                         "whacked",
                                         "beaten", /*   Bludgeon Weaponry   */
                                         "thumped",
                                         "flogged",
                                         "pounded",
                                         "slammed",
                                         "battered"},

                                        {"poke", /*       L      */
                                         "sting",
                                         "prick",
                                         "stab", /*   Piercing Weaponry   */
                                         "pierce",
                                         "spike",
                                         "penetrate",
                                         "lance",
                                         "impale"},

                                        {"pokes", /*       M      */
                                         "stings",
                                         "pricks",
                                         "stabs", /*   Piercing Weaponry   */
                                         "pierces",
                                         "spikes",
                                         "penetrates",
                                         "lances",
                                         "impales"},

                                        {"poked", /*       N      */
                                         "stung",
                                         "pricked",
                                         "stabbed", /*   Piercing Weaponry   */
                                         "pierced",
                                         "spiked",
                                         "penetrated",
                                         "lanced",
                                         "impaled"},

                                        {"insignificantly", /*       O      */
                                         "slightly",
                                         "lightly",
                                         "fairly", /*   Adverbes   */
                                         "seriously",
                                         "severely",
                                         "gravely",
                                         "critically",
                                         "fatally"},

                                        {"nudge", /*       P       */
                                         "graze",
                                         "bump",
                                         "knock", /*   Fist        */
                                         "strike",
                                         "thump",
                                         "batter",
                                         "redesign",
                                         "shatter"}};

    assert(str);

    cp = buf;

    for (; *str; str++)
    {
        if (*str == '#')
        {
            str++;
            if (*str >= 'A' && *str <= 'P')
            {
                strcpy(cp, damage_blocks[*str - 'A'][damage_index(damage, max_hp)]);
            }
            else
            {
                slog(LOG_ALL, 0, "Illegal damage block reference.");
                strcpy(cp, "ILLEGAL BLOCK");
            }
            TAIL(cp);
        }
        else
        {
            *(cp++) = *str;
        }
    }

    *cp = 0;

    return buf;
}

static void combat_send(combat_single_msg *msg,
                        unit_data *arg1,
                        unit_data *arg2,
                        unit_data *arg3,
                        int dam,
                        const char *color1,
                        const char *color2,
                        const char *color3)
{
    if (msg->to_char)
    {
        cact(sub_damage(msg->to_char, dam, arg3->getMaximumHitpoints()), A_SOMEONE, arg1, arg2, arg3, TO_CHAR, color1);
    }

    if (msg->to_vict)
    {
        cact(sub_damage(msg->to_vict, dam, arg3->getMaximumHitpoints()), A_ALWAYS, arg1, arg2, arg3, TO_VICT, color2);
    }

    if (msg->to_notvict)
    {
        cact(sub_damage(msg->to_notvict, dam, arg3->getMaximumHitpoints()), A_SOMEONE, arg1, arg2, arg3, TO_NOTVICT, color3);
    }
}

/* Hit Location is one of:                                */
/* WEAR_HEAD, WEAR_HANDS, WEAR_ARMS, WEAR_BODY,           */
/* WEAR_LEGS or WEAR_FEET for actual body locations.      */
/* Use WEAR_SHIELD if hit is blocked by a shield          */
/* Use COM_MSG_DEAD when a killing blow                   */
/* Use COM_MSG_MISS when a complete miss                  */
/* Use COM_MSG_NODAM when a hit but no damage is given    */

void combat_message(unit_data *att, unit_data *def, unit_data *medium, int damage, int msg_group, int msg_number, int hit_location)
{
    combat_msg_packet *msg = nullptr;
    int i = 0;
    int r = 0;

    msg = nullptr;

    for (i = 0; i < COM_MAX_MSGS; i++)
    {
        if ((fight_messages[i].group == msg_group) && num_in_msg(&fight_messages[i], msg_number))
        {
            r = number(0, fight_messages[i].no_of_msgs - 1);
            msg = fight_messages[i].msg;
            for (; msg && r; msg = msg->next, r--)
            {
                ;
            }
            break;
        }
    }

    if (msg)
    {
        switch (hit_location)
        {
            case COM_MSG_MISS:
                combat_send(&(msg->cmiss), att, medium, def, damage, "miss_me", "miss_opponent", "miss_other");
                break;

            case COM_MSG_EBODY:
                combat_send(&(msg->entire), att, medium, def, damage, "hit_me", "hit_opponent", "hit_other");
                break;

            case COM_MSG_NODAM:
                combat_send(&(msg->nodam), att, medium, def, damage, "nodam_me", "nodam_opponent", "nodam_other");
                break;

            case WEAR_BODY:
                combat_send(&(msg->body), att, medium, def, damage, "hit_me", "hit_opponent", "hit_other");
                break;

            case WEAR_ARMS:
                combat_send(&(msg->arm), att, medium, def, damage, "hit_me", "hit_opponent", "hit_other");
                break;

            case WEAR_LEGS:
                combat_send(&(msg->legs), att, medium, def, damage, "hit_me", "hit_opponent", "hit_other");
                break;

            case WEAR_HEAD:
                combat_send(&(msg->head), att, medium, def, damage, "hit_me", "hit_opponent", "hit_other");
                break;

            case WEAR_HANDS:
                combat_send(&(msg->hand), att, medium, def, damage, "hit_me", "hit_opponent", "hit_other");
                break;

            case WEAR_FEET:
                combat_send(&(msg->feet), att, medium, def, damage, "hit_me", "hit_opponent", "hit_other");
                break;

            case WEAR_SHIELD:
                combat_send(&(msg->shield), att, medium, def, damage, "shield_me", "shield_opponent", "shield_other");
                break;

            case COM_MSG_DEAD:
                combat_send(&(msg->die), att, medium, def, damage, "hit_me", "hit_opponent", "hit_other");
                break;

            default:
                act("Undefined hitlocation!", A_SOMEONE, att, cActParameter(), cActParameter(), TO_ALL);
                break;
        }
    }
    else
    {
        act("Undefined message numbers!", A_SOMEONE, att, cActParameter(), cActParameter(), TO_ALL);
        slog(LOG_ALL, 0, "combat_message: Unknown message group %d, number %d", msg_group, msg_number);
    }
}

/* -------------------------------------------------------------------- */

void update_pos(unit_data *victim)
{
    if ((victim->getCurrentHitpoints() > 0) && (CHAR_POS(victim) > POSITION_STUNNED))
    {
        return;
    }
    else if (victim->getCurrentHitpoints() > 0)
    {
        UCHAR(victim)->setPosition(POSITION_STANDING);
    }
    else if (victim->getCurrentHitpoints() <= -11)
    {
        UCHAR(victim)->setPosition(POSITION_DEAD);
    }
    else if (victim->getCurrentHitpoints() <= -6)
    {
        UCHAR(victim)->setPosition(POSITION_MORTALLYW);
    }
    else if (victim->getCurrentHitpoints() <= -3)
    {
        UCHAR(victim)->setPosition(POSITION_INCAP);
    }
    else
    {
        UCHAR(victim)->setPosition(POSITION_STUNNED);
    }
}

/* -------------------------------------------------------------------- */

/* When ch kills victim */
static void change_alignment(unit_data *slayer, unit_data *victim)
{
    int adjust = 0;
    int diff = 0;

    diff = slayer->getAlignment() - victim->getAlignment();

    if (slayer->isGood())
    {
        if (victim->isEvil())
        { /* Diff >= 700 */
            adjust = (diff - 700) / 70 + 1;
        }
        else if (victim->isNeutral())
        {
            if (victim->getAlignment() <= -100)
            {
                adjust = 1;
            }
            else if (victim->getAlignment() >= 100)
            {
                adjust = -3;
            }
        }
        else
        { /* Victim is good */
            adjust = -(slayer->getAlignment() + victim->getAlignment() - 700) / 5 - 3;
        }
    }
    else if (slayer->isEvil())
    {
        if (victim->isGood())
        { /* Diff <= -700 */
            adjust = (diff + 700) / 35 - 2;
        }
        else if (diff > 0)
        {
            adjust = +5;
        }
    }
    else /* Neutral slayer - diff in -1350..+1350 */
    {
        if (diff > 350)
        {
            adjust = (diff - 350) / 30 + 1;
        }
        else if (diff < -350)
        {
            adjust = (diff + 350) / 30 - 1;
        }
    }

    diff = CHAR_LEVEL(slayer) - CHAR_LEVEL(victim);
    if (diff > 0)
    {
        adjust += (adjust * MIN(10, diff)) / 10;
    }
    else if (diff < 0)
    {
        adjust += (adjust * MAX(-10, diff)) / 20;
    }

    adjust = MIN(200, adjust);
    adjust = MAX(-200, adjust);

    slayer->changeAlignmentBy(adjust);
}

/* Do all the gain stuff for CH where no is the number of players */
/* which are to share the "share" of experience                   */
static void person_gain(unit_data *ch, unit_data *dead, int share, int grouped, int maxlevel)
{
    if (share > 0)
    {
        if (ch->isGood() && dead->isEvil())
        {
            share += share / 5;
        }

        if (ch->isEvil() && dead->isGood())
        {
            share += share / 10;
        }
    }

    if (ch->isPC())
    {
        if (share > 0)
        {
            /* No need to reduce XP since flee is now a skill
               if (IS_SET(CHAR_FLAGS(ch), CHAR_WIMPY))
               share /= 2; // Only 50% in wimpy */

            if (CHAR_LEVEL(ch) < maxlevel - 5)
            {
                share -= (MIN(95, 7 * (maxlevel - (CHAR_LEVEL(ch) + 5))) * share) / 100;
            }
        }

        gain_exp(ch, share);

        if (share > 0)
        {
            cact("You receive $2d experience points.", A_ALWAYS, ch, &share, cActParameter(), TO_CHAR, "xpgain");
        }
        else if (share == 0)
        {
            cact("You receive no experience points.", A_ALWAYS, ch, cActParameter(), cActParameter(), TO_CHAR, "xpgain");
        }
        else /* less than zero */
        {
            cact("You are penalized by $2d experience.", A_ALWAYS, ch, &share, cActParameter(), TO_CHAR, "xpgain");
        }
    }

    change_alignment(ch, dead);
}

/* This takes 100% care of victim and attackers change in experience */
/* and alignments. Do not fiddle with these values anywhere else     */
/* when a kill has been made                                         */
static void exp_align_gain(unit_data *ch, unit_data *victim)
{
    int rellevel = 0;
    int sumlevel = 0;
    int maxlevel = 0;
    int minlevel = 0;
    int no_members = 1;
    int share = 0;
    unit_data *head = nullptr;
    char_follow_type *f = nullptr;

    maxlevel = CHAR_LEVEL(ch);

    if (victim->isPC() && IS_SET(PC_FLAGS(victim), PC_SPIRIT))
    {
        act("Oh dear, what a mess!", A_SOMEONE, victim, cActParameter(), cActParameter(), TO_ROOM);
        slog(LOG_EXTENSIVE, 0, "Oh dear, a spirit was killed!");
        return;
    }

    head = ch; /* Who is head of potential group? */

    if (CHAR_HAS_FLAG(ch, CHAR_GROUP) && CHAR_MASTER(ch) && CHAR_HAS_FLAG(CHAR_MASTER(ch), CHAR_GROUP) &&
        same_surroundings(ch, CHAR_MASTER(ch)))
    {
        head = CHAR_MASTER(ch);
    }

    if (victim->isPC())
    {
        share = 0;
    }
    else /* NPC killed */
    {
        unit_affected_type *paf = nullptr;

        paf = affected_by_spell(victim, ID_MAX_ATTACKER);

        if (paf)
        {
            maxlevel = MAX(virtual_level(head), paf->getDataAtIndex(0));
        }
        else
        {
            maxlevel = virtual_level(head);
        }

        minlevel = maxlevel;
        sumlevel = maxlevel;
        no_members = 1;

        if (CHAR_HAS_FLAG(ch, CHAR_GROUP))
        {
            for (f = CHAR_FOLLOWERS(head); f; f = f->getNext())
            {
                if (CHAR_HAS_FLAG(f->getFollower(), CHAR_GROUP) && same_surroundings(head, f->getFollower()))
                {
                    sumlevel += virtual_level(f->getFollower());

                    maxlevel = MAX(virtual_level(f->getFollower()), maxlevel);
                    minlevel = MIN(virtual_level(f->getFollower()), minlevel);

                    no_members++;
                }
            }

            assert((no_members > 0) && (sumlevel >= 0));
        }

        share = CHAR_EXP(victim);

        if (share > 0)
        {
            /* rellevel = virtual_level(victim) - maxlevel - (no_members - 1); */
            rellevel = virtual_level(victim) - maxlevel;

            /* Check for level difference */

            if (rellevel < 0)
            {
                share = (MAX(0, 100 + 15 * rellevel) * share) / 100;
            }
            else if (rellevel > 0)
            {
                share = ((100 + 15 * rellevel) * share) / 100;
            }

            if (no_members > 1)
            {
                share = (4 * share) / (3 + no_members);
            }

            share = MIN(100 * no_members + 300, share);
        }
    }

    share = (8 * share) / 10; /* At 90% now... */

    if (!CHAR_HAS_FLAG(ch, CHAR_GROUP))
    {
        person_gain(ch, victim, share, FALSE, maxlevel);
    }
    else
    {
        if (CHAR_HAS_FLAG(head, CHAR_GROUP) && same_surroundings(head, ch))
        {
            person_gain(head, victim, share, (no_members > 1), maxlevel);
        }

        for (f = CHAR_FOLLOWERS(head); f; f = f->getNext())
        {
            if (CHAR_HAS_FLAG(f->getFollower(), CHAR_GROUP) && same_surroundings(f->getFollower(), ch))
            {
                person_gain(f->getFollower(), victim, share, (no_members > 1), maxlevel);
            }
        }
    }
}

/*
int lose_exp(class unit_data *ch)
{
   int loss, i;

   assert(IS_PC(ch));

   // This first line takes care of any xp earned above required level.

   loss = MAX(0, (CHAR_EXP(ch) - required_xp(PC_VIRTUAL_LEVEL(ch))) / 2);

   // This line makes sure, that you lose at most half a level...

   loss = MIN(loss, level_xp(PC_VIRTUAL_LEVEL(ch)) / 2);

   // This line takes care of the case where you have less or almost
   // equal XP to your required. You thus lose at least 1/5th your
   // level.

   loss = MAX(loss, level_xp(PC_VIRTUAL_LEVEL(ch)) / 5);

   // This line takes care of newbies, setting the lower bound..
   i = MAX(0, (CHAR_EXP(ch) - required_xp(START_LEVEL)) / 2);

   if (loss > i)
      loss = i;

   assert(loss >= 0);

   return loss;
}
*/

/* Die is only called when a PC or NPC is killed for real, causing XP loss
   and transfer of rewards */

void die(unit_data *ch)
{
    diltemplate *death = nullptr;

    if (ch->is_destructed())
    {
        return;
    }

    death = find_dil_template("death@death");
    if (death)
    {
        send_death(ch);
        dilprg *prg = dil_copy_template(death, ch, nullptr);
        if (prg)
        {
            prg->waitcmd = WAITCMD_MAXINST - 1;
            dil_activate(prg);
        }
        return;
    }
}

/* -------------------------------------------------------------------- */

/* Call when adding or subtracting hitpoints to/from a character */
void modify_hit(unit_data *ch, int hit)
{
    if (CHAR_POS(ch) > POSITION_DEAD)
    {
        ch->changeCurrentHitpointsBy(hit);
        /// @todo move limits into the setter
        ch->setCurrentHitpoints(MIN(hit_limit(ch), ch->getCurrentHitpoints()));

        update_pos(ch);

        if (CHAR_POS(ch) == POSITION_DEAD)
        {
            die(ch);
        }
    }
}

void damage(unit_data *ch,
            unit_data *victim,
            unit_data *medium,
            int dam,
            int attack_group,
            int attack_number,
            int hit_location,
            int bDisplay)
{
    int max_hit = 0;
    unit_affected_type *paf = nullptr;
    unit_data *sch = nullptr;

    if (ch->is_destructed() || victim->is_destructed())
    {
        return;
    }

    /* Minimum is 0 damage points [ no maximum! ] */
    dam = MAX(dam, 0);

    /* If neither are allowed to attack each other... */
    if (pk_test(ch, victim, FALSE) && pk_test(victim, ch, FALSE))
    {
        dam = 0;
    }

    if (IS_IMMORTAL(victim))
    {
        dam = 0;
    }

    if ((CHAR_POS(victim) == POSITION_SLEEPING) && (dam > 0))
    {
        UCHAR(victim)->setPosition(POSITION_RESTING);
        send_to_char("OUCH! You wake up!<br/>", victim);
    }

    if (victim != ch)
    {
        UCHAR(victim)->freeLastAttacker();
        if (ch->getNames().Name())
        {
            UCHAR(victim)->setLastAttacker(str_dup(ch->getNames().Name()));
        }
        UCHAR(victim)->setLastAttackerType(ch->getUnitType());
        if (IS_SET(CHAR_FLAGS(victim), CHAR_HIDE))
        {
            if ((paf = affected_by_spell(victim, ID_HIDE)))
            {
                destroy_affect(paf);
            }
            UCHAR(victim)->removeCharacterFlag(CHAR_HIDE);
        }

        if (IS_SET(victim->getUnitFlags(), UNIT_FL_INVISIBLE))
        {
            while ((paf = affected_by_spell(victim, ID_INVISIBILITY)))
            {
                destroy_affect(paf);
            }
            victim->removeUnitFlag(UNIT_FL_INVISIBLE);
        }

        if ((paf = affected_by_spell(ch, ID_SANCTUARY)))
        {
            destroy_affect(paf);
        }

        offend_legal_state(ch, victim);

        /* Set victim in fighting position
             if (CHAR_POS(victim) > POSITION_STUNNED)
             {  Don't check for STUNNED! Victim must be set fighting for murder
             resolution!!! */

        if (!CHAR_FIGHTING(victim))
        {
            if (victim->isPC() && IS_IMMORTAL(victim) && ch->isPC())
            {
                cact("$1n ignores your feeble threats.", A_ALWAYS, victim, cActParameter(), ch, TO_VICT, "miss_me");
                cact("$1n ignores $3n's feeble threats.", A_ALWAYS, victim, cActParameter(), ch, TO_NOTVICT, "miss_other");
            }
            else
            {
                set_fighting(victim, ch, TRUE);
            }
        }
        else
        {
            set_fighting(ch, victim); // If already fighting it makes no diff
        }

        if (CHAR_MASTER(victim) == ch)
        {
            stop_following(victim);
            send_to_char("You stop following the jerk that is hitting you.<br/>", victim);
        }
    }

    auto arg = diku::format_to_str("%d %d %d", attack_group, attack_number, hit_location);

    if (send_ack(ch, medium, victim, &dam, &g_cmd_auto_damage, arg.c_str(), victim) == SFR_BLOCK)
    {
        return;
    }
    if (victim != ch)
    {
        if ((paf = affected_by_spell(victim, ID_MAX_ATTACKER)))
        {
            paf->setDataAtIndex(0, MAX(CHAR_LEVEL(ch), paf->getDataAtIndex(0)));
        }
        else
        {
            unit_affected_type af;

            af.setID(ID_MAX_ATTACKER);
            af.setDuration(4);
            af.setBeat(WAIT_SEC * 60 * 5);
            af.setFirstFI(TIF_NONE);
            af.setTickFI(TIF_NONE);
            af.setLastFI(TIF_NONE);
            af.setApplyFI(APF_NONE);
            af.setDataAtIndex(0, CHAR_LEVEL(ch));
            af.setDataAtIndex(1, 0);
            af.setDataAtIndex(2, 0);

            create_affect(victim, &af);
        }
    }
    victim->changeCurrentHitpointsBy(-1 * dam);

    update_pos(victim);

    if (bDisplay)
    {
        if (CHAR_POS(victim) > POSITION_DEAD)
        {
            combat_message(ch, victim, medium, dam, attack_group, attack_number, hit_location);
        }
        else
        {
            combat_message(ch, victim, medium, dam, attack_group, attack_number, COM_MSG_DEAD);
        }
    }

    switch (CHAR_POS(victim))
    {
        case POSITION_MORTALLYW:
            send_to_char("You are mortally wounded!<br/>", victim);
            act("$1n is mortally wounded!", A_SOMEONE, victim, cActParameter(), cActParameter(), TO_ROOM);
            break;

        case POSITION_INCAP:
            send_to_char("You are incapacitated!<br/>", victim);
            act("$1n is incapacitated!", A_SOMEONE, victim, cActParameter(), cActParameter(), TO_ROOM);
            break;

        case POSITION_STUNNED:
            act("$1n is stunned, but will probably regain consciousness again.",
                A_HIDEINV,
                victim,
                cActParameter(),
                cActParameter(),
                TO_ROOM);
            act("You're stunned, but will probably regain consciousness again.",
                A_SOMEONE,
                victim,
                cActParameter(),
                cActParameter(),
                TO_CHAR);
            break;

        case POSITION_DEAD:
            act("$1n is dead!", A_HIDEINV, victim, cActParameter(), cActParameter(), TO_ROOM);
            break;

        default: /* >= POSITION SLEEPING */
            assert(CHAR_POS(victim) >= POSITION_SLEEPING);

            max_hit = hit_limit(victim);

            if (dam > (max_hit / 5))
            {
                cact("That Really did HURT!", A_SOMEONE, victim, cActParameter(), cActParameter(), TO_CHAR, "hit_me");
                cact("$1n screams with agony!", A_SOMEONE, victim, cActParameter(), cActParameter(), TO_ROOM, "hit_other");
            }

            if (victim->getCurrentHitpoints() < (max_hit / 5))
            {
                cact("You wish that your wounds would stop BLEEDING that much!",
                     A_SOMEONE,
                     victim,
                     cActParameter(),
                     cActParameter(),
                     TO_CHAR,
                     "hit_me");
            }

            if ((dam > victim->getCurrentHitpoints() / 4) || (victim->getCurrentHitpoints() < (max_hit / 4)))
            {
                if (IS_SET(CHAR_FLAGS(victim), CHAR_WIMPY))
                {
                    command_interpreter(victim, "flee");
                    return;
                }
            }
            break;
    }

    /* MS: Moved check to stop_fighting till after DEAD check so that the
        special routines can check WHO actually killed the person in question
        by testing is FIGHTING(u) == victim. When FIGHTING(u) is NULL
        it is supposed to equal one self... */

    /* Murder! */
    if (CHAR_POS(victim) == POSITION_DEAD)
    {
        sch = nullptr;
        if (ch == victim)
        {
            if (CHAR_LAST_ATTACKER(victim))
            {
                for (sch = victim->getUnitIn()->getUnitContains(); sch; sch = sch->getNext())
                {
                    if (CHAR_LAST_ATTACKER(victim) && sch->getNames().Name())
                    {
                        if (strcmp(CHAR_LAST_ATTACKER(victim), sch->getNames().Name()) == 0)
                        {
                            if (CHAR_LAST_ATTACKER_TYPE(victim) == sch->getUnitType())
                            {
                                break;
                            }
                        }
                    }
                }
            }
            if (sch)
            {
                ch = sch;
            }
        }

        if (ch != victim)
        {
            exp_align_gain(ch, victim);

            /* Except in self defence & legal target makes a crime */
            if (!IS_SET(CHAR_FLAGS(victim), CHAR_LEGAL_TARGET) && !IS_SET(CHAR_FLAGS(ch), CHAR_SELF_DEFENCE))
            {
                if (victim->isPC())
                {
                    if (!IS_SET(CHAR_FLAGS(victim), CHAR_OUTLAW) || IS_SET(CHAR_FLAGS(victim), CHAR_PROTECTED))
                    {
                        log_crime(ch, victim, CRIME_PK);
                    }
                }
                else
                {
                    if (IS_SET(CHAR_FLAGS(victim), CHAR_PROTECTED))
                    {
                        log_crime(ch, victim, CRIME_MURDER);
                    }
                }
            }
        }

        if (!victim->isNPC())
        {
            slog(LOG_EXTENSIVE,
                 MAX(ch->getLevelOfWizardInvisibility(), victim->getLevelOfWizardInvisibility()),
                 "%s killed by %s at %s",
                 STR(victim->getNames().Name()),
                 TITLENAME(ch),
                 STR(TITLENAME(ch->getUnitIn())));
        }

        if (victim == ch)
        {
            UCHAR(ch)->setCharacterFlag(CHAR_KILL_SELF);
            die(ch);
        }
        else
        {
            stop_fighting(victim);
            set_fighting(victim, ch, TRUE);
            set_fighting(ch, victim, TRUE);

            die(victim); /* Lose experience */
        }

        return;
    }

    if (CHAR_POS(victim) <= POSITION_STUNNED)
    {
        if (CHAR_FIGHTING(victim))
        {
            stop_fighting(victim);
        }
        if (CHAR_FIGHTING(ch) == victim)
        {
            stop_fighting(ch);
        }
    }

    if (victim->isPC() && !CHAR_DESCRIPTOR(victim) && CHAR_AWAKE(victim))
    {
        command_interpreter(victim, "flee");
        if (victim->is_destructed() || CHAR_POS(victim) == POSITION_DEAD)
        {
            return;
        }
    }
}

void break_object(unit_data *obj)
{
    if (OBJ_EQP_POS(obj))
    {
        unequip_object(obj);
    }

    obj->setTitle(diku::format_to_str("%s (broken)", obj->getTitle()));

    UOBJ(obj)->setValueAtIndexTo(0, 0);
    UOBJ(obj)->setValueAtIndexTo(1, 0);
    UOBJ(obj)->setValueAtIndexTo(2, 0);
    UOBJ(obj)->setValueAtIndexTo(3, 0);
    UOBJ(obj)->setObjectItemType(ITEM_TRASH);

    affect_clear_unit(obj);

    UOBJ(obj)->setPriceInGP(0);
    UOBJ(obj)->setPricePerDay(0);
    UOBJ(obj)->setMagicResistance(0);
}

/* 'ch' is optional, and will receive a message if 'obj' breaks */
void damage_object(unit_data *ch, unit_data *obj, int dam)
{
    if (obj == nullptr)
    {
        return;
    }

    assert(obj->isObj());

    if (obj->getCurrentHitpoints() < 0)
    { /* Endless */
        return;
    }

    if (OBJ_TYPE(obj) == ITEM_WEAPON)
    {
        dam /= 4; // Adjustments requested...
    }
    else if (OBJ_TYPE(obj) == ITEM_ARMOR)
    {
        dam *= 1; // Ditto...
    }

    switch (OBJ_TYPE(obj))
    {
        case ITEM_SHIELD:
            dam *= 2;
            /* Fallthrough */

        case ITEM_WEAPON:
        case ITEM_ARMOR:
            dam -= OBJ_VALUE(obj, 2); /* Just subtract (or add) 25 magic points */
    }

    dam /= 10;

    if (dam > 0)
    {
        obj->changeCurrentHitpointsBy(-1 * dam);

        if (obj->getCurrentHitpoints() < 0)
        {
            if (ch)
            {
                act("Your $3N is broken by the impact!!!", A_ALWAYS, ch, cActParameter(), cActParameter(obj), TO_CHAR);
                act("$1n's $3N is broken by the impact!!!", A_ALWAYS, ch, cActParameter(), cActParameter(obj), TO_ROOM);
            }

            break_object(obj);
        }
    }
}

/* Returns TRUE if ok */
static int check_combat(unit_data *ch)
{
    if (ch->is_destructed())
    {
        assert(!CHAR_COMBAT(ch));
        return FALSE;
    }

    if (!CHAR_AWAKE(ch))
    {
        stop_fighting(ch);
    }
    else
    {
        while (CHAR_FIGHTING(ch))
        {
            if (!same_surroundings(ch, CHAR_FIGHTING(ch)))
            {
                stop_fighting(ch, CHAR_FIGHTING(ch));
            }
            else
            {
                break;
            }
        }
    }

    return CHAR_FIGHTING(ch) != nullptr;
}

// Above level 50 a e.g. 100 roll becomes (with / 18):
//  L51 -> 117
//  L60 -> 167
//  L70 -> 222
//  L100 -> 389
//  L150 -> 667
//  L200 -> 944
//  L250 -> 1222
//
//  I've started with / 25 to not give everyone on the game a huge chock.
//
int roll_boost(int roll, int level)
{
    if (level <= 50)
    {
        return roll;
    }
    else
    {
        return roll * (level - 30) / 25; // / 18;
    }
}

/* -1 if fails, >= 0 amount of damage */
int one_hit(unit_data *att, unit_data *def, int bonus, int att_weapon_type, int primary, int attack)
{
    int dam = 0;
    int hm = 0;
    int hit_loc = 0;
    int roll = 0;

    int def_armour_type = 0;
    int def_shield_bonus = 0;

    unit_data *att_weapon = nullptr;
    unit_data *def_armour = nullptr;
    unit_data *def_shield = nullptr;

    assert(att->isChar() && def->isChar());

    if (CHAR_POS(att) < POSITION_SLEEPING)
    {
        return -1;
    }

    if (CHAR_POS(def) <= POSITION_DEAD)
    {
        return -1;
    }

    if (att == def)
    {
        return -1;
    }

    if (!same_surroundings(att, def))
    {
        return -1;
    }

    if (attack)
    {
        if (!CHAR_FIGHTING(att))
        {
            set_fighting(att, def, TRUE);
        }

        if (CHAR_POS(att) != POSITION_FIGHTING)
        {
            UCHAR(att)->setPosition(POSITION_FIGHTING);
        }
        add_fighting(att, def, TRUE);
    }
    else
    {
        add_fighting(att, def, FALSE);
    }

    hit_loc = hit_location(att, def);

    hm = melee_bonus(att, def, hit_loc, &att_weapon_type, &att_weapon, &def_armour_type, &def_armour, primary);

    hm += bonus;

    roll = open100();

    if (CHAR_COMBAT(att))
    {
        // The weapon's speed is subtracted
        CHAR_COMBAT(att)->changeSpeed(g_wpn_info[att_weapon_type].speed, UCHAR(att)->getSpeedPercentage());
    }

    if (att_weapon && weapon_fumble(att_weapon, roll))
    {
        cact("You fumble with your $2N!", A_ALWAYS, att, att_weapon, cActParameter(), TO_CHAR, "miss_me");
        cact("$1n fumbles with $1s $2N!", A_ALWAYS, att, att_weapon, cActParameter(), TO_ROOM, "miss_other");
        hm = MIN(-10, roll - open100());
        damage_object(att, att_weapon, hm);
        if (CHAR_COMBAT(att))
        {
            CHAR_COMBAT(att)->changeSpeed(SPEED_DEFAULT / 2, 100); // Lose ½ round when you fumble
        }

        return 0;
    }
    else
    {
        roll_description(att, "hit", roll);
        hm += roll_boost(roll, CHAR_LEVEL(att));
    }

    if (!check_combat(att))
    {
        return -1;
    }

    send_combat(att);

    if (!check_combat(att))
    {
        return -1;
    }

    dam = 0;

    if (!CHAR_AWAKE(def))
    {
        hm = MAX(hm, 100);
    }

    if (hm < 0) /* a miss */
    {
        damage(att, def, att_weapon, 0, MSG_TYPE_WEAPON, att_weapon_type, COM_MSG_MISS);
    }
    else /* A hit! hm > 0 */
    {
        if (att_weapon)
        {
            if (is_obj_slaying(att_weapon, CHAR_RACE(def)))
            {
                hm += hm / 5; /* Add 20% bonus extra again to damage */
                act("Your $2N glows!", A_ALWAYS, att, att_weapon, cActParameter(), TO_CHAR);
                act("$1n's $2N glows!", A_HIDEINV, att, att_weapon, cActParameter(), TO_ROOM);
            }
            dam = weapon_damage(hm, att_weapon_type, def_armour_type);
        }
        else
        {
            dam = natural_damage(hm, att_weapon_type, def_armour_type, att->getBaseWeight());
        }

        if (dam > 0)
        {
            damage_object(att, att_weapon, dam);
        }

        def_shield_bonus = shield_bonus(att, def, &def_shield);

        if (CHAR_AWAKE(def) && def_shield && g_wpn_info[att_weapon_type].shield != SHIELD_M_USELESS)
        {
            if ((g_wpn_info[att_weapon_type].shield == SHIELD_M_BLOCK) && (number(1, 100) <= def_shield_bonus))
            {
                damage_object(def, def_shield, dam);
                damage(att, def, def_shield, 0, MSG_TYPE_WEAPON, att_weapon_type, WEAR_SHIELD);
                return 0;
            }
            if ((g_wpn_info[att_weapon_type].shield == SHIELD_M_REDUCE) && (number(1, 100) <= def_shield_bonus))
            {
                dam -= (dam * def_shield_bonus) / 100;
            }
        }

        if (dam > 0)
        {
            damage_object(def, def_armour, dam);

            /* May the victim bleed under my beatiful system */
            if (def_armour || CHAR_IS_HUMANOID(def))
            {
                damage(att, def, att_weapon, dam, MSG_TYPE_WEAPON, att_weapon_type, hit_loc);
            }
            else
            {
                damage(att, def, att_weapon, dam, MSG_TYPE_WEAPON, att_weapon_type, COM_MSG_EBODY);
            }
        }
        else
        {
            damage(att, def, att_weapon, 0, MSG_TYPE_WEAPON, att_weapon_type, COM_MSG_NODAM);
        }
    }

    return dam;
}

int simple_one_hit(unit_data *att, unit_data *def)
{
    return one_hit(att, def, 0, WPN_ROOT, TRUE, TRUE);
}

/* control the fights going on */
void melee_violence(unit_data *ch, int primary)
{
    if (!check_combat(ch))
    {
        return;
    }

    if (CHAR_POS(ch) == POSITION_FIGHTING)
    {
        one_hit(ch, CHAR_FIGHTING(ch), 0, WPN_ROOT, primary, TRUE);
    }
    else
    {
        act("You get back in a fighting position, ready to fight!", A_SOMEONE, ch, cActParameter(), cActParameter(), TO_CHAR);
        act("$1n gets back in a fighting position ready to fight!", A_SOMEONE, ch, cActParameter(), cActParameter(), TO_ROOM);
        UCHAR(ch)->setPosition(POSITION_FIGHTING);
    }
}

/* -------------------------------------------------------------------- */
/*                      H U N T I N G   S Y S T E M                     */
/* -------------------------------------------------------------------- */

struct hunt_data
{
    int no;
    int was_legal;
    unit_data *victim;
};

/* The hunting routine will prefer to hit anyone hunted to those */
/* already engaged in combat. This is to prevent players flee    */
/* and then enter room as the backmost attacker such that a new  */
/* 'tank' is allowed to be hit at.                               */

/* MS: Changed only to re-attack on tick command                 */
/*     Otherwise two characters hunting get really many attacks  */
/*     as they were activated by the flee command                */
int hunting(spec_arg *sarg)
{
    hunt_data *h = nullptr;

    if (sarg->cmd->no != CMD_AUTO_TICK)
    {
        return SFR_SHARE;
    }

    h = (hunt_data *)sarg->fptr->getData();
    assert(h);

    h->no--;

    if (h->no <= 0)
    {
        destroy_fptr(sarg->owner, sarg->fptr); /* Will free data automatically */
        return SFR_BLOCK;
    }

    if (CHAR_AWAKE(sarg->owner))
    {
        /* with new transparency, this should be a list
             of all visible chars in surroundings */

        if (scan4_ref(sarg->owner, h->victim))
        {
            if (!h->victim->isChar())
            {
                destroy_fptr(sarg->owner, sarg->fptr); /* Will free automatic. */
                return SFR_BLOCK;
            }

            if (!CHAR_CAN_SEE(sarg->owner, h->victim))
            {
                return SFR_SHARE;
            }

            act("$1n growls viciously at $3n.", A_SOMEONE, sarg->owner, cActParameter(), h->victim, TO_NOTVICT);
            act("$1n growls at you.", A_SOMEONE, sarg->owner, cActParameter(), h->victim, TO_VICT);

            /* If the victim was a legal target, then it must be such */
            /* again                                                  */
            if (h->was_legal)
            {
                UCHAR(h->victim)->setCharacterFlag(CHAR_LEGAL_TARGET);
            }

            if (!CHAR_COMBAT(sarg->owner) || !CHAR_COMBAT(sarg->owner)->FindOpponent(h->victim))
            {
                set_fighting(sarg->owner, h->victim, TRUE);
            }

            destroy_fptr(sarg->owner, sarg->fptr); /* Will free fptr above */

            return SFR_BLOCK;
        }
    }

    return SFR_SHARE;
}
