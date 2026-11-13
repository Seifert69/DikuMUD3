/*
 $Author: All $
 $RCSfile: comm.cpp,v $
 $Date: 2004/03/20 06:13:21 $
 $Revision: 2.5 $
 */
#include "comm.h"

#include "config.h"
#include "constants.h"
#include "descriptor_data.h"
#include "handler.h"
#include "hookmud.h"
#include "interpreter.h"
#include "main_functions.h"
#include "protocol.h"
#include "slog.h"
#include "system.h"
#include "textutil.h"
#include "unitwalk.h"
#include "utils.h"

#include <cctype>
#include <cstring>
#include <functional>

actType_e ActType(int n)
{
    if ((n >= actType_e::eTO_ROOM) && (n <= actType_e::eTO_REST))
    {
        return (actType_e) n;
    }
    else
    {
        slog(LOG_ALL, 0, "actType_e conversion OOB integer %d", n);
        return actType_e::eTO_ALL;
    }
}


actShow_e ActShow(int n)
{
    if ((n >= actShow_e::eA_HIDEINV) && (n <= actShow_e::eA_RAW))
    {
        return (actShow_e) n;
    }
    else
    {
        slog(LOG_ALL, 0, "actShow_e conversion OOB integer %d", n);
        return actShow_e::eA_ALWAYS;
    }
}




cActParameter::cActParameter()
{
    m_u = nullptr;
    m_i = nullptr;
    m_p = nullptr;
}

cActParameter::cActParameter(const unit_data *u)
{
    m_u = u;
    m_i = nullptr;
    m_p = nullptr;
}

cActParameter::cActParameter(const int *i)
{
    m_u = nullptr;
    m_i = i;
    m_p = nullptr;
}

cActParameter::cActParameter(const char *p)
{
    m_u = nullptr;
    m_i = nullptr;
    m_p = p;
}

cActParameter::cActParameter(const dilval *v)
{
    m_u = nullptr;
    m_i = nullptr;
    m_p = nullptr;

    if ((v->type == DilVarType_e::DILV_UP) || (v->type == DilVarType_e::DILV_UPR))
    {
        m_u = (unit_data *)v->val.ptr;
    }
    else if ((v->type == DilVarType_e::DILV_SP) || (v->type == DilVarType_e::DILV_SPR) || (v->type == DilVarType_e::DILV_HASHSTR))
    {
        m_p = (char *)v->val.ptr;
    }
}

int cActParameter::isNull()
{
    return (m_u == nullptr) && (m_i == nullptr) && (m_p == nullptr);
}

/**
 *  Given HTML string src find all <div and all <h1 and replace class='default' with
 *  user's color preferences.  You probably only want to do this for telnet users.
 *  web users can rely on the styles defined in the CSS
 *  <data> packets (e.g. the $json of wstat) are for the web client only, so
 *  they are dropped: telnet would show what is inside them.
 */
void substHTMLcolor(std::string &dest, const char *src, color_type &color)
{
    const char *p = nullptr;

    p = src;
    dest.clear();

    while (*p)
    {
        const char *pAfterData = skipHTMLData(p);
        if (pAfterData != p)
        {
            p = pAfterData;
            continue;
        }
        if ((p[0] == '<') && ((p[1] == 'd' && p[2] == 'i' && p[3] == 'v' && p[4] == ' ') || (p[1] == 'h' && p[2] == '1' && p[3] == ' ')))
        {
            char aTag[256];

            p = getHTMLTag(p, aTag, sizeof(aTag));

            char buf[256];
            int l = 0;

            l = getHTMLValue("class", aTag, buf, sizeof(buf) - 1);

            // We got a color code on our hands, let's see if we need to substitute
            if (l >= 1)
            {
                auto Col = color.get(buf);

                if (Col.empty())
                {
                    Col = g_cServerConfig.getColorType().get(buf);
                }

                if (Col.empty() == false)
                {
                    // Substitute the color
                    char newtag[256];
                    substHTMLTagClass(aTag, "class", Col.c_str(), newtag, sizeof(newtag) - 1);
                    dest.push_back('<');
                    dest.append(newtag);
                    dest.push_back('>');
                    // s printf(dest, "<%s>", newtag);
                    // TAIL(dest);
                }
            }
            continue;
        }
        dest.push_back(*p++);
        //*dest++ = *p++;
    }
    //*dest = 0;
}

/**
 *  Public routines for system-to-player communication
 *  Sends directly to multiplexer.
 */
void send_to_descriptor(const char *messg, descriptor_data *d)
{
    if (d && messg && *messg)
    {
        bool bWeb = d->getMultiHookPtr()->bWebsockets;
        std::string dest; // The text for a telnet client

        if (!bWeb)
        {
            const unit_data *u = d->cgetCharacter();

            if (!u || !u->isPC())
            { // switched or snooped?
                u = d->cgetOriginalCharacter();
            }

            assert(u->isPC());
            dest.reserve(strlen(messg) * 1.1);
            // char buf[strlen(messg) + 10000];

            substHTMLcolor(dest, messg, UPC(u)->getColor());
            if (dest.empty())
            {
                return; // Only <data> packets, which telnet doesn't get
            }
        }

        // Packets alone are nothing to read, so they don't call for a new prompt
        if (d->getPromptMode() == PROMPT_IGNORE && !(bWeb && *messg == '<' && str_is_html_data(messg)))
        {
            d->setPromptMode(PROMPT_EXPECT);
            send_to_descriptor("<br/>", d);
        }

        protocol_send_text(d->getMultiHookPtr(), d->getMultiHookID(), bWeb ? messg : dest.c_str(), MULTI_TEXT_CHAR);

        if (d->getSnoopData().getSnoopBy())
        {
            // A web client would take the packets (an $alias list, ...) as
            // its own
            std::string snooped = str_remove_html_data(messg);
            if (!snooped.empty())
            {
                send_to_descriptor(SNOOP_PROMPT, CHAR_DESCRIPTOR(d->getSnoopData().getSnoopBy()));
                send_to_descriptor(snooped, CHAR_DESCRIPTOR(d->getSnoopData().getSnoopBy()));
            }
        }
    }
}

void send_to_descriptor(const std::string &messg, descriptor_data *d)
{
    send_to_descriptor(messg.c_str(), d);
}

void send_to_descriptor(const std::string &messg, const descriptor_data *d)
{
    send_to_descriptor(messg.c_str(), const_cast<descriptor_data *>(d));
}

std::string json_packet(const std::string &json, bool bWebClient)
{
    if (bWebClient)
    {
        return "<data type='$json'>" + json + "</data>";
    }
    return str_html_attr_escape(json.c_str()) + "<br/>";
}

void page_string(descriptor_data *d, const char *messg)
{
    // If a unit test is being run, send results there too
    unit_tests::OutputCapture::page_string(d, messg);

    if (d && messg && *messg)
    {
        std::string mystr;

        mystr = "<paged>";
        mystr.append(messg);
        mystr.append("</paged>");

        protocol_send_text(d->getMultiHookPtr(), d->getMultiHookID(), mystr.c_str(), MULTI_PAGE_CHAR);

        if (d->cgetSnoopData().getSnoopBy())
        {
            send_to_descriptor(SNOOP_PROMPT, CHAR_DESCRIPTOR(d->getSnoopData().getSnoopBy()));
            send_to_descriptor(messg, CHAR_DESCRIPTOR(d->getSnoopData().getSnoopBy()));
        }
    }
}

void page_string(descriptor_data *d, const std::string &messg)
{
    page_string(d, messg.c_str());
}

void page_string(const descriptor_data *d, const std::string &messg)
{
    page_string(const_cast<descriptor_data *>(d), messg);
}

void send_to_char(const char *messg, const unit_data *ch)
{
    // If a unit test is being run, send results there too
    unit_tests::OutputCapture::send_to_char(messg, ch);

    if (ch->isChar())
    {
        send_to_descriptor(messg, const_cast<descriptor_data *>(CHAR_DESCRIPTOR(ch)));
    }
}

void send_to_char(const std::string &messg, const unit_data *ch)
{
    send_to_char(messg.c_str(), ch);
}

void send_to_all(const char *messg)
{
    descriptor_data *i = nullptr;

    if (messg && *messg)
    {
        for (i = g_descriptor_list; i; i = i->getNext())
        {
            if (descriptor_is_playing(i))
            {
                send_to_descriptor(messg, i);
            }
        }
    }
}

void send_to_all(const std::string &messg)
{
    send_to_all(messg.c_str());
}

void send_to_zone_outdoor(const zone_type *z, const char *messg)
{
    descriptor_data *i = nullptr;

    if (messg && *messg)
    {
        for (i = g_descriptor_list; i; i = i->getNext())
        {
            if (descriptor_is_playing(i) && i->cgetCharacter()->isOutside() && unit_zone(i->cgetCharacter()) == z &&
                CHAR_AWAKE(i->cgetCharacter()) && !IS_SET(i->cgetCharacter()->getUnitIn()->getUnitFlags(), UNIT_FL_NO_WEATHER) &&
                !IS_SET(unit_room(i->getCharacter())->getUnitFlags(), UNIT_FL_NO_WEATHER))
            {
                send_to_descriptor(messg, i);
            }
        }
    }
}

void send_to_outdoor(const char *messg)
{
    descriptor_data *i = nullptr;

    if (messg && *messg)
    {
        for (i = g_descriptor_list; i; i = i->getNext())
        {
            if (descriptor_is_playing(i) && i->cgetCharacter()->isOutside() && CHAR_AWAKE(i->cgetCharacter()) &&
                !IS_SET(i->cgetCharacter()->getUnitIn()->getUnitFlags(), UNIT_FL_NO_WEATHER) &&
                !IS_SET(unit_room(i->getCharacter())->getUnitFlags(), UNIT_FL_NO_WEATHER))
            {
                send_to_descriptor(messg, i);
            }
        }
    }
}

// The text of an act() for "to", without a <br/> (act() and cact() add one, sact() doesn't).
// buf = dest, MAX_STRING_LENGTH long, with room left for the <br/>. False when there is nothing
// for "to" (buf "") or the text stops at an illegal $-code (buf what came before it).
bool act_generate(char *buf,
                  const char *str,
                  actShow_e show_type,
                  cActParameter arg1,
                  cActParameter arg2,
                  cActParameter arg3,
                  actType_e type,
                  const unit_data *to)
{
    const char *strp = nullptr;
    char *point = nullptr;
    const char *i = nullptr;
    // MS2020 int uppercase = FALSE;

    cActParameter *sub = nullptr;
    /*union {
        const void *vo;
        class unit_data *un;
        const char *str;
        const int *num;
    } sub;*/

    *buf = 0;

    if (str == nullptr) // DIL's act() with a string never set
    {
        return false;
    }

    if (!to->isChar() || arg1.m_u == nullptr)
    {
        return false;
    }

    // if (!IS_CHAR(to) || !CHAR_DESCRIPTOR(to) || arg1.m_u == NULL)
    //    return;

    if (to == arg1.m_u && (type == TO_ROOM || type == TO_NOTVICT || type == TO_REST))
    {
        return false;
    }

    if (to == arg3.m_u && type == TO_NOTVICT)
    {
        return false;
    }

    if (to->getUnitIn() == arg1.m_u && type == TO_REST)
    {
        return false;
    }

    if (show_type != eA_RAW)
    {
        if ((show_type == actShow_e::eA_HIDEINV && !CHAR_CAN_SEE(to, arg1.m_u)) || (show_type != actShow_e::eA_ALWAYS && !CHAR_AWAKE(to)))
        {
            return false;
        }
    }

    // A $-code used inside an HTML tag of str, e.g. <div class='say_other' from='$1n'>,
    // is an attribute value: escape it so a ' or > in a name can't break the tag
    bool bInTag = false;
    std::string sEscaped;

    // A longer text is cut short, leaving room for the <br/> act() and cact() add
    const char *const end = buf + MAX_STRING_LENGTH - sizeof("<br/>");

    for (strp = str, point = buf;;)
    {
        if (point >= end)
        {
            *(point++) = 0;
            break;
        }

        if (*strp == '$')
        {
            switch (*++strp)
            {
                case '1':
                    sub = &arg1;
                    break;
                case '2':
                    sub = &arg2;
                    break;
                case '3':
                    sub = &arg3;
                    break;
                case '$':
                    i = "$";
                    break;

                default:
                    slog(LOG_ALL, 0, "Illegal first code to act(): %s", str);
                    *point = 0;
                    return false;
            }

            if (i == nullptr)
            {
                switch (*++strp)
                {
                    case 'n':
                        if (sub->m_u != nullptr)
                        {
                            if (CHAR_CAN_SEE(to, sub->m_u) || show_type == eA_RAW)
                            {
                                if (sub->m_u->isPC())
                                {
                                    /* Upper-case it */
                                    // MS 2020 uppercase = TRUE;
                                    i = ((unit_data *)sub->m_u)->getNames().Name();
                                }
                                else
                                {
                                    i = sub->m_u->getTitle().c_str();
                                }
                            }
                            else
                            {
                                i = SOMETON(sub->m_u);
                            }
                        }
                        else
                        {
                            slog(LOG_ALL, 0, "NULL n code to act(): %s", str);
                        }
                        break;
                    case 'N':
                        if (sub->m_u != nullptr)
                        {
                            if (show_type == eA_RAW)
                                i = ((unit_data *)sub->m_u)->getNames().Name();
                            else
                                i = UNIT_SEE_NAME(to, (unit_data *)sub->m_u);
                        }
                        else
                        {
                            slog(LOG_ALL, 0, "NULL N code to act(): %s", str);
                        }
                        break;
                    case 'm':
                        if (sub->m_u != nullptr)
                        {
                            i = HMHR(to, sub->m_u);
                        }
                        else
                        {
                            slog(LOG_ALL, 0, "NULL m code to act(): %s", str);
                        }
                        break;
                    case 's':
                        if (sub->m_u != nullptr)
                        {
                            i = HSHR(to, sub->m_u);
                        }
                        else
                        {
                            slog(LOG_ALL, 0, "NULL s code to act(): %s", str);
                        }
                        break;
                    case 'e':
                        if (sub->m_u != nullptr)
                        {
                            i = HESH(to, sub->m_u);
                        }
                        else
                        {
                            slog(LOG_ALL, 0, "NULL e code to act(): %s", str);
                        }
                        break;
                    case 'p':
                        if (sub->m_u != nullptr)
                        {
                            if (sub->m_u->isChar())
                            {
                                i = g_char_pos[CHAR_POS(sub->m_u)];
                            }
                            else
                            {
                                i = "lying";
                            }
                        }
                        else
                        {
                            slog(LOG_ALL, 0, "NULL p code to act(): %s", str);
                        }
                        break;
                    case 'a':
                        if (sub->m_u != nullptr)
                        {
                            i = UNIT_ANA((unit_data *)sub->m_u);
                        }
                        else if (sub->m_p != nullptr)
                        {
                            i = ANA(*sub->m_p);
                        }
                        else
                        {
                            slog(LOG_ALL, 0, "NULL a code to act(): %s", str);
                        }
                        break;
                    case 'd':
                        if (sub->m_i != nullptr)
                        {
                            i = itoa(*(sub->m_i));
                        }
                        else
                        {
                            slog(LOG_ALL, 0, "NULL i code to act(): %s", str);
                        }
                        break;
                    case 't':
                        if (sub->m_p == nullptr)
                        {
                            slog(LOG_ALL, 0, "NULL t code to act(): %s", str);
                        }
                        i = sub->m_p;
                        break;
                    default:
                        slog(LOG_ALL, 0, "ERROR: Illegal second code to act(): %s", str);
                        *point = 0;
                        return false;
                } /* switch */
            }

            if (i == nullptr)
            {
                i = "NULL";
            }

            if (bInTag)
            {
                sEscaped = str_html_attr_escape(i);
                i = sEscaped.c_str();
            }

            /*	  if (uppercase && *i)
                  {
                    *point++ = toupper(*i);
                    i++;
                    uppercase = FALSE;
                 }
            */
            while (point < end && (*point = *(i++)))
            {
                point++;
            }

            i = nullptr;

            ++strp;
        }
        else
        {
            if (*strp == '<')
            {
                bInTag = true;
            }
            else if (*strp == '>')
            {
                bInTag = false;
            }

            if (!(*(point++) = *(strp++)))
            {
                break;
            }
        }
    }

    char *cpos = nullptr;

    /* Cap the first letter, but skip all colour and control codes! */
    // MS2020: rewrite to skip HTML codes and uppercase first letter
    point = buf;
    while (*point == '<')
    {
        cpos = strchr(point, '>');

        if (cpos)
        {
            point = cpos + 1; // Skip the '>' char
        }
        else
        {
            break; // A '<' that never closes, e.g. "<3": no tag to skip
        }
    }
    *point = toupper(*point);
    return true;
}

/// Who an act() is about when it is written for one: the victim (arg3) for TO_VICT and
/// TO_NOTVICT, the actor (arg1) otherwise.
const unit_data *act_viewer(cActParameter arg1, cActParameter arg3, actType_e type)
{
    return ((type == TO_VICT) || (type == TO_NOTVICT)) ? arg3.m_u : arg1.m_u;
}

/// The text of an act() for the one it is about (act_viewer()), without a <br/>; "" when they
/// can't see it (or for TO_ROOM/TO_REST/TO_NOTVICT, which leave that one out).
/// buf = dest, MAX_STRING_LENGTH long. True as act_generate().
bool sact(char *buf, const char *str, actShow_e show_type, cActParameter arg1, cActParameter arg2, cActParameter arg3, actType_e type)
{
    *buf = 0;

    if (!str || !*str)
    {
        return false;
    }

    const unit_data *to = act_viewer(arg1, arg3, type);
    return to && to->isChar() && act_generate(buf, str, show_type, arg1, arg2, arg3, type, to);
}

/// Everyone an act() reaches, each character given to send(): for TO_CHAR the actor and for
/// TO_VICT the victim alone; otherwise everyone in the actor's room and in its transparent units,
/// and when the actor's room is a transparent unit, those in the room around it as well.
template<typename SEND>
static void act_walk(cActParameter arg1, cActParameter arg3, actType_e type, SEND &&send)
{
    if ((type == TO_VICT) || (type == TO_CHAR))
    {
        const unit_data *to = act_viewer(arg1, arg3, type);
        if (to && to->isChar())
        {
            send(to);
        }
        return;
    }

    if (arg1.m_u == nullptr)
    {
        return;
    }

    walk_surroundings(arg1.m_u, [&](unit_data *u, unit_data *, bool) {
        if (u->isChar())
        {
            send(u);
        }
        return u->getNumberOfCharactersInsideUnit() ? eWalk::on : eWalk::skip; // no one inside to tell
    });
}

// act_walk() for the unit tests (declared there)
void act_recipients(cActParameter arg1, cActParameter arg3, actType_e type, const std::function<void(const unit_data *)> &send)
{
    act_walk(arg1, arg3, type, send);
}

/// act() and cact(): the text to each one connected. act() sends what act_generate() gives, with
/// the <br/> when the text is whole; cact() sends a text that isn't blank inside a
/// <div class='colortype'>.
static void act_send(const char *str,
                     actShow_e show_type,
                     cActParameter arg1,
                     cActParameter arg2,
                     cActParameter arg3,
                     actType_e type,
                     const char *colortype)
{
    if (!str || !*str)
    {
        return;
    }

    const std::string div = colortype ? divcolor(colortype) : "";
    act_walk(arg1, arg3, type, [&](const unit_data *to) {
        if (!CHAR_DESCRIPTOR(to))
        {
            return;
        }
        char buf[MAX_STRING_LENGTH];
        bool whole = act_generate(buf, str, show_type, arg1, arg2, arg3, type, to);
        if (!colortype)
        {
            if (whole)
            {
                strcat(buf, "<br/>"); // act_generate leaves room for it
            }
            send_to_descriptor(buf, CHAR_DESCRIPTOR(to));
        }
        else if (!str_is_empty(buf))
        {
            send_to_descriptor(div + buf + "</div><br/>", CHAR_DESCRIPTOR(to));
        }
    });
}

void act(const char *str, actShow_e show_type, cActParameter arg1, cActParameter arg2, cActParameter arg3, actType_e type)
{
    act_send(str, show_type, arg1, arg2, arg3, type, nullptr);
}

void cact(const char *str, actShow_e show_type, cActParameter arg1, cActParameter arg2, cActParameter arg3, actType_e type, const char *colortype)
{
    act_send(str, show_type, arg1, arg2, arg3, type, colortype);
}
