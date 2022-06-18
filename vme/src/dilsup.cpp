/*
 $Author: tperry $
 $RCSfile: dilsup.cpp,v $
 $Date: 2003/01/02 23:39:20 $
 $Revision: 2.3 $
 */

#include "account.h"
#include "db.h"
#include "descriptor_data.h"
#include "interpreter.h"
#include "utils.h"

#include <cstring>

int pay_point_charlie(unit_data *ch, unit_data *to)
{
    if (ch->isPC() && g_cServerConfig.isAccounting() && IS_MORTAL(ch))
    {
        if (CHAR_DESCRIPTOR(ch))
        {
            if (g_cServerConfig.FromLAN(CHAR_DESCRIPTOR(ch)->getHostname()))
            {
                return TRUE;
            }
        }

        if (to->getFileIndex()->getZone() && to->getFileIndex()->getZone()->getPayOnly())
        {
            if ((to->getFileIndex()->getZone()->getPayOnly() == 1) && (PC_ACCOUNT(ch).getTotalCredit() <= 0))
            {
                account_paypoint(ch);
                return FALSE;
            }
            else if ((to->getFileIndex()->getZone()->getPayOnly() == 2) &&
                     (PC_ACCOUNT(ch).getFlatRateExpirationDate() < (ubit32)time(nullptr)))
            {
                account_paypoint(ch);
                return FALSE;
            }
            else if ((to->getFileIndex()->getZone()->getPayOnly() == 3) &&
                     ((PC_ACCOUNT(ch).getTotalCredit() > 0) || (PC_ACCOUNT(ch).getFlatRateExpirationDate() > (ubit32)time(nullptr))))
            {
                account_paypoint(ch);
                return FALSE;
            }
        }
    }

    return TRUE;
}

static int getkeyword(unit_data *obj)
{
    int keyword = -2;

    if (obj->isWornOn(MANIPULATE_WEAR_SHIELD))
    {
        keyword = WEAR_SHIELD;
    }
    if (obj->isWornOn(MANIPULATE_WEAR_FINGER))
    {
        keyword = WEAR_FINGER_L;
    }
    if (obj->isWornOn(MANIPULATE_WEAR_NECK))
    {
        keyword = WEAR_NECK_1;
    }
    if (obj->isWornOn(MANIPULATE_WEAR_WRIST))
    {
        keyword = WEAR_WRIST_L;
    }
    if (obj->isWornOn(MANIPULATE_WEAR_WAIST))
    {
        keyword = WEAR_WAIST;
    }
    if (obj->isWornOn(MANIPULATE_WEAR_ARMS))
    {
        keyword = WEAR_ARMS;
    }
    if (obj->isWornOn(MANIPULATE_WEAR_HANDS))
    {
        keyword = WEAR_HANDS;
    }
    if (obj->isWornOn(MANIPULATE_WEAR_FEET))
    {
        keyword = WEAR_FEET;
    }
    if (obj->isWornOn(MANIPULATE_WEAR_LEGS))
    {
        keyword = WEAR_LEGS;
    }
    if (obj->isWornOn(MANIPULATE_WEAR_ABOUT))
    {
        keyword = WEAR_ABOUT;
    }
    if (obj->isWornOn(MANIPULATE_WEAR_HEAD))
    {
        keyword = WEAR_HEAD;
    }
    if (obj->isWornOn(MANIPULATE_WEAR_BODY))
    {
        keyword = WEAR_BODY;
    }
    if (obj->isWornOn(MANIPULATE_WIELD))
    {
        keyword = WEAR_WIELD;
    }
    if (obj->isWornOn(MANIPULATE_WEAR_CHEST))
    {
        keyword = WEAR_CHEST;
    }
    if (obj->isWornOn(MANIPULATE_WEAR_BACK))
    {
        keyword = WEAR_BACK;
    }
    if (obj->isWornOn(MANIPULATE_WEAR_EAR))
    {
        keyword = WEAR_EAR_L;
    }
    if (obj->isWornOn(MANIPULATE_WEAR_ANKLE))
    {
        keyword = WEAR_ANKLE_L;
    }

    return keyword;
}

/* Return NULL if unit fits, pointer to string otherwise */
static const char *wear_size(unit_data *ch, unit_data *obj, int var)
{
    if (ch->getSize() == 0)
    {
        return "error";
    }

    int percent = 0;

    if (ch->getSize() > 0)
    {
        percent = (100 * obj->getSize()) / ch->getSize();
    }
    else
    {
        percent = (100 * obj->getSize());
    }

    if (percent < 100 - var - (100 - var) / 2)
    {
        return "way too small";
    }
    else if (percent < (100 - var))
    {
        return "too small";
    }
    else if (percent > 100 + var + (100 + var) / 2)
    {
        return "way too large";
    }
    else if (percent > (100 + var))
    {
        return "too large";
    }

    return nullptr;
}

const char *obj_wear_size(unit_data *ch, unit_data *obj, int keyword)
{
    if (keyword == -1)
    {
        keyword = getkeyword(obj);
    }

    switch (keyword)
    {
        case WEAR_FINGER_R:
        case WEAR_FINGER_L:
            return wear_size(ch, obj, 50);

        case WEAR_NECK_1:
        case WEAR_NECK_2:
            return wear_size(ch, obj, 50);

        case WEAR_BODY:
            return wear_size(ch, obj, 40);

        case WEAR_HEAD:
            return wear_size(ch, obj, 50);

        case WEAR_LEGS:
            return wear_size(ch, obj, 50);

        case WEAR_FEET:
            return wear_size(ch, obj, 50);

        case WEAR_HANDS:
            return wear_size(ch, obj, 50);

        case WEAR_ARMS:
            return wear_size(ch, obj, 50);

        case WEAR_ABOUT:
            return wear_size(ch, obj, 50);

        case WEAR_WAIST:
            return wear_size(ch, obj, 50);

        case WEAR_WRIST_L:
        case WEAR_WRIST_R:
            return wear_size(ch, obj, 50);

        case WEAR_WIELD:
            return wear_size(ch, obj, 50);

        case WEAR_HOLD:
            if (OBJ_TYPE(obj) == ITEM_WEAPON)
            {
                return wear_size(ch, obj, 50);
            }
            else
            {
                return nullptr;
            }

        case WEAR_SHIELD:
            return wear_size(ch, obj, 50);

        case WEAR_CHEST:
            return wear_size(ch, obj, 50);

        case WEAR_BACK:
            return wear_size(ch, obj, 50);

        case WEAR_EAR_L:
        case WEAR_EAR_R:
            return wear_size(ch, obj, 50);

        case WEAR_ANKLE_L:
        case WEAR_ANKLE_R:
            return wear_size(ch, obj, 50);
    }

    return nullptr;
}
