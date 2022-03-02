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
#include "structs.h"
#include "system.h"
#include "textutil.h"
#include "utils.h"

#include <cctype>
#include <cstring>

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

    if ((v->type == DILV_UP) || (v->type == DILV_UPR))
    {
        m_u = (unit_data *)v->val.ptr;
    }
    else if ((v->type == DILV_SP) || (v->type == DILV_SPR) || (v->type == DILV_HASHSTR))
    {
        m_p = (char *)v->val.ptr;
    }
}

int cActParameter::isNull()
{
    return (m_u == nullptr) && (m_i == nullptr) && (m_p == nullptr);
}

/*
 *  Given HTML string src find all <div and all <h1 and replace class='default' with
 *  user's color preferences.  You probably only want to do this for telnet users.
 *  web users can rely on the styles defined in the CSS
 */
void substHTMLcolor(std::string &dest, const char *src, color_type &color)
{
    const char *p = nullptr;

    p = src;
    dest.clear();

    while (*p)
    {
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

/*
 *  Public routines for system-to-player communication
 *  Sends directly to multiplexer.
 */
void send_to_descriptor(const char *messg, descriptor_data *d)
{
    if (d && messg && *messg)
    {
        if (d->getPromptMode() == PROMPT_IGNORE)
        {
            d->setPromptMode(PROMPT_EXPECT);
            send_to_descriptor("<br/>", d);
        }

        if (d->getMultiHookPtr()->bWebsockets)
        {
            protocol_send_text(d->getMultiHookPtr(), d->getMultiHookID(), messg, MULTI_TEXT_CHAR);
        }
        else
        {
            const unit_data *u = d->cgetCharacter();

            if (!u || !IS_PC(u))
            { // switched or snooped?
                u = d->cgetOriginalCharacter();
            }

            assert(IS_PC(u));
            std::string dest;
            dest.reserve(strlen(messg) * 1.1);
            // char buf[strlen(messg) + 10000];

            substHTMLcolor(dest, messg, UPC(u)->color);
            protocol_send_text(d->getMultiHookPtr(), d->getMultiHookID(), dest.c_str(), MULTI_TEXT_CHAR);
        }

        if (d->cgetSnoopData().getSnoopBy())
        {
            send_to_descriptor(SNOOP_PROMPT, CHAR_DESCRIPTOR(d->cgetSnoopData().getSnoopBy()));
            send_to_descriptor(messg, CHAR_DESCRIPTOR(d->cgetSnoopData().getSnoopBy()));
        }
    }
}

void send_to_descriptor(const std::string &messg, descriptor_data *d)
{
    send_to_descriptor(messg.c_str(), d);
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
            send_to_descriptor(SNOOP_PROMPT, CHAR_DESCRIPTOR(d->cgetSnoopData().getSnoopBy()));
            send_to_descriptor(messg, CHAR_DESCRIPTOR(d->cgetSnoopData().getSnoopBy()));
        }
    }
}

void page_string(descriptor_data *d, const std::string &messg)
{
    page_string(d, messg.c_str());
}

void send_to_char(const char *messg, const unit_data *ch)
{
    // If a unit test is being run, send results there too
    unit_tests::OutputCapture::send_to_char(messg, ch);

    if (IS_CHAR(ch))
    {
        send_to_descriptor(messg, CHAR_DESCRIPTOR(ch));
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
            if (descriptor_is_playing(i) && UNIT_IS_OUTSIDE(i->cgetCharacter()) && unit_zone(i->cgetCharacter()) == z &&
                CHAR_AWAKE(i->cgetCharacter()) && !IS_SET(UNIT_FLAGS(UNIT_IN(i->cgetCharacter())), UNIT_FL_NO_WEATHER) &&
                !IS_SET(UNIT_FLAGS(unit_room(i->getCharacter())), UNIT_FL_NO_WEATHER))
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
            if (descriptor_is_playing(i) && UNIT_IS_OUTSIDE(i->cgetCharacter()) && CHAR_AWAKE(i->cgetCharacter()) &&
                !IS_SET(UNIT_FLAGS(UNIT_IN(i->cgetCharacter())), UNIT_FL_NO_WEATHER) &&
                !IS_SET(UNIT_FLAGS(unit_room(i->getCharacter())), UNIT_FL_NO_WEATHER))
            {
                send_to_descriptor(messg, i);
            }
        }
    }
}

// To deal with both telnet and HTML newlines, act() should *always*
// append a <br/> and sact() should never add a <br/>. buf = dest
void act_generate(char *buf,
                  const char *str,
                  int show_type,
                  cActParameter arg1,
                  cActParameter arg2,
                  cActParameter arg3,
                  int type,
                  const unit_data *to,
                  int bNewline)
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

    if (!IS_CHAR(to) || arg1.m_u == nullptr)
    {
        return;
    }

    // if (!IS_CHAR(to) || !CHAR_DESCRIPTOR(to) || arg1.m_u == NULL)
    //    return;

    if (to == arg1.m_u && (type == TO_ROOM || type == TO_NOTVICT || type == TO_REST))
    {
        return;
    }

    if (to == arg3.m_u && type == TO_NOTVICT)
    {
        return;
    }

    if (UNIT_IN(to) == arg1.m_u && type == TO_REST)
    {
        return;
    }

    if ((show_type == A_HIDEINV && !CHAR_CAN_SEE(to, arg1.m_u)) || (show_type != A_ALWAYS && !CHAR_AWAKE(to)))
    {
        return;
    }

    for (strp = str, point = buf;;)
    {
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
                    return;
            }

            if (i == nullptr)
            {
                switch (*++strp)
                {
                    case 'n':
                        if (sub->m_u != nullptr)
                        {
                            if (CHAR_CAN_SEE(to, sub->m_u))
                            {
                                if (IS_PC(sub->m_u))
                                {
                                    /* Upper-case it */
                                    // MS 2020 uppercase = TRUE;
                                    i = UNIT_NAME((unit_data *)sub->m_u);
                                }
                                else
                                {
                                    i = UNIT_TITLE(sub->m_u).c_str();
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
                            if (IS_CHAR(sub->m_u))
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
                        return;
                } /* switch */
            }

            if (i == nullptr)
            {
                i = "NULL";
            }

            /*	  if (uppercase && *i)
                  {
                    *point++ = toupper(*i);
                    i++;
                    uppercase = FALSE;
                 }
            */
            while ((*point = *(i++)))
            {
                point++;
            }

            i = nullptr;

            ++strp;
        }
        else if (!(*(point++) = *(strp++)))
        {
            break;
        }
    }

    point--;
    if (bNewline)
    {
        if (point > buf)
        {
            *(point) = 0;
            strcat(point, "<br/>");
        }
        else
        {
            strcat(buf, "<br/>");
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
    }
    *point = toupper(*point);
}

// Executes exactly like act() for TO_VICT and TO_CHAR. But never adds newline
// Can't handle other
// because of variance with each target's ability to e.g. see.
// Is there a need for sactother which takes a "to" argument? If we add
// to argument here it will be confusing for TO_VICT and TO_CHAR scenarios.
void sact(char *buf, const char *str, int show_type, cActParameter arg1, cActParameter arg2, cActParameter arg3, int type)
{
    const unit_data *to = nullptr;

    /* This to catch old-style FALSE/TRUE calls...  */
    assert(show_type == A_SOMEONE || show_type == A_HIDEINV || show_type == A_ALWAYS);

    if (!str || !*str)
    {
        return;
    }

    *buf = 0;

    if (type == TO_VICT)
    {
        to = arg3.m_u;
        if (IS_CHAR(to))
        {
            act_generate(buf, str, show_type, arg1, arg2, arg3, type, to, 0); // No newline
        }
        return;
    }
    else if (type == TO_CHAR)
    {
        to = arg1.m_u;
        if (IS_CHAR(to))
        {
            act_generate(buf, str, show_type, arg1, arg2, arg3, type, to, 0);
        }
    }
}

// Always adds <br/> at the end
void act(const char *str, int show_type, cActParameter arg1, cActParameter arg2, cActParameter arg3, int type)
{
    const unit_data *to = nullptr;
    const unit_data *u = nullptr;
    char buf[MAX_STRING_LENGTH];

    /* This to catch old-style FALSE/TRUE calls...  */
    assert(show_type == A_SOMEONE || show_type == A_HIDEINV || show_type == A_ALWAYS);

    if (!str || !*str)
    {
        return;
    }

    if (type == TO_VICT)
    {
        to = arg3.m_u;
    }
    else if (type == TO_CHAR)
    {
        to = arg1.m_u;
    }
    else if (arg1.m_u == nullptr || UNIT_IN(arg1.m_u) == nullptr)
    {
        return;
    }
    else
    {
        to = UNIT_CONTAINS(UNIT_IN(arg1.m_u));
    }

    /* same unit or to person */
    for (; to; to = to->next)
    {
        if (IS_CHAR(to) && CHAR_DESCRIPTOR(to))
        {
            act_generate(buf, str, show_type, arg1, arg2, arg3, type, to);
            send_to_descriptor(buf, CHAR_DESCRIPTOR(to));
        }

        if (type == TO_VICT || type == TO_CHAR)
        {
            return;
        }
        if (UNIT_CHARS(to) && UNIT_IS_TRANSPARENT(to))
        {
            for (u = UNIT_CONTAINS(to); u; u = u->next)
            {
                if (IS_CHAR(u) && CHAR_DESCRIPTOR(u))
                {
                    act_generate(buf, str, show_type, arg1, arg2, arg3, type, u);
                    send_to_descriptor(buf, CHAR_DESCRIPTOR(u));
                }
            }
        }
    }

    /* other units outside transparent unit */
    if (UNIT_IN(arg1.m_u) && (to = UNIT_IN(UNIT_IN(arg1.m_u))) && UNIT_IS_TRANSPARENT(UNIT_IN(arg1.m_u)))
    {
        for (to = UNIT_CONTAINS(to); to; to = to->next)
        {
            if (IS_CHAR(to) && CHAR_DESCRIPTOR(to))
            {
                act_generate(buf, str, show_type, arg1, arg2, arg3, type, to);
                send_to_descriptor(buf, CHAR_DESCRIPTOR(to));
            }

            if (UNIT_CHARS(to) && UNIT_IS_TRANSPARENT(to) && to != UNIT_IN(arg1.m_u))
            {
                for (u = UNIT_CONTAINS(to); u; u = u->next)
                {
                    if (IS_CHAR(u) && CHAR_DESCRIPTOR(u))
                    {
                        act_generate(buf, str, show_type, arg1, arg2, arg3, type, u);
                        send_to_descriptor(buf, CHAR_DESCRIPTOR(u));
                    }
                }
            }
        }
    }
}

void cact(const char *str, int show_type, cActParameter arg1, cActParameter arg2, cActParameter arg3, int type, const char *colortype)
{
    const unit_data *to = nullptr;
    const unit_data *u = nullptr;
    char buf[MAX_STRING_LENGTH];
    char temp[MAX_STRING_LENGTH];
    char *t = nullptr;
    char *b = buf;
    /* This to catch old-style FALSE/TRUE calls...  */
    assert(show_type == A_SOMEONE || show_type == A_HIDEINV || show_type == A_ALWAYS);

    if (!str || !*str)
    {
        return;
    }

    if (type == TO_VICT)
    {
        to = arg3.m_u;
    }
    else if (type == TO_CHAR)
    {
        to = arg1.m_u;
    }
    else if (arg1.m_u == nullptr || UNIT_IN(arg1.m_u) == nullptr)
    {
        return;
    }
    else
    {
        to = UNIT_CONTAINS(UNIT_IN(arg1.m_u));
    }

    /* same unit or to person */
    for (; to; to = to->next)
    {
        if (IS_CHAR(to) && CHAR_DESCRIPTOR(to))
        {
            *buf = 0;
            b = buf;
            *temp = 0;
            t = temp;
            act_generate(t, str, show_type, arg1, arg2, arg3, type, to, false);
            if (!str_is_empty(t))
            {
                strcat(b, divcolor(colortype)); // Adds <div class='colortype'>
                TAIL(b);
                strcat(b, t);
                TAIL(b);
                strcat(b, "</div><br/>");
                send_to_descriptor(buf, CHAR_DESCRIPTOR(to));
            }
        }

        if (type == TO_VICT || type == TO_CHAR)
        {
            return;
        }

        if (UNIT_CHARS(to) && UNIT_IS_TRANSPARENT(to))
        {
            for (u = UNIT_CONTAINS(to); u; u = u->next)
            {
                if (IS_CHAR(u) && CHAR_DESCRIPTOR(u))
                {
                    *buf = 0;
                    b = buf;

                    strcat(b, divcolor(colortype));
                    TAIL(b);
                    act_generate(buf, str, show_type, arg1, arg2, arg3, type, u, false);
                    TAIL(b);
                    strcat(b, "</div><br/>");
                    send_to_descriptor(buf, CHAR_DESCRIPTOR(u));
                }
            }
        }
    }

    /* other units outside transparent unit */
    if ((to = UNIT_IN(UNIT_IN(arg1.m_u))) && UNIT_IS_TRANSPARENT(UNIT_IN(arg1.m_u)))
    {
        for (to = UNIT_CONTAINS(to); to; to = to->next)
        {
            if (IS_CHAR(to) && CHAR_DESCRIPTOR(to))
            {
                *buf = 0;
                b = buf;
                strcat(b, divcolor(colortype));
                TAIL(b);
                act_generate(buf, str, show_type, arg1, arg2, arg3, type, to, false);
                TAIL(b);
                strcat(b, "</div><br/>");
                send_to_descriptor(buf, CHAR_DESCRIPTOR(to));
            }

            if (UNIT_CHARS(to) && UNIT_IS_TRANSPARENT(to) && to != UNIT_IN(arg1.m_u))
            {
                for (u = UNIT_CONTAINS(to); u; u = u->next)
                {
                    if (IS_CHAR(u) && CHAR_DESCRIPTOR(u))
                    {
                        *buf = 0;
                        b = buf;
                        strcat(b, divcolor(colortype));
                        TAIL(b);
                        act_generate(buf, str, show_type, arg1, arg2, arg3, type, u, false);
                        TAIL(b);
                        strcat(b, "</div><br/>");
                        send_to_descriptor(buf, CHAR_DESCRIPTOR(u));
                    }
                }
            }
        }
    }
}
