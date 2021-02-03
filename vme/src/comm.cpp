/*
 $Author: All $
 $RCSfile: comm.cpp,v $
 $Date: 2004/03/20 06:13:21 $
 $Revision: 2.5 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "structs.h"
#include "utils.h"
#include "utility.h"
#include "system.h"
#include "comm.h"
#include "textutil.h"
#include "protocol.h"
#include "interpreter.h"
#include "handler.h"
#include "hookmud.h"
#include "constants.h"
#include "config.h"

/* external vars */
extern class descriptor_data *descriptor_list;


cActParameter::cActParameter()
{
    m_u = NULL;
    m_i = NULL;
    m_p = NULL;
}

cActParameter::cActParameter(const class unit_data *u)
{
    m_u = u;
    m_i = NULL;
    m_p = NULL;
}

cActParameter::cActParameter(const int *i)
{
    m_u = NULL;
    m_i = i;
    m_p = NULL;
}

cActParameter::cActParameter(const char *p)
{
    m_u = NULL;
    m_i = NULL;
    m_p = p;
}

cActParameter::cActParameter(const dilval *v)
{
    m_u = NULL;
    m_i = NULL;
    m_p = NULL;

    if ((v->type == DILV_UP) || (v->type == DILV_UPR))
        m_u = (struct unit_data *) v->val.ptr;
    else if ((v->type == DILV_SP) || (v->type == DILV_SPR) || (v->type == DILV_HASHSTR))
        m_p = (char *) v->val.ptr;
}

int cActParameter::isNull(void)
{
    return (m_u == NULL) && (m_i == NULL) && (m_p == NULL);
}


/*  
 *  Given HTML string src find all <div and all <h1 and replace class='default' with
 *  user's color preferences.  You probably only want to do this for telnet users.
 *  web users can rely on the styles defined in the CSS
*/
void substHTMLcolor(string &dest, const char *src, class color_type &color)
{
    const char *p;

    p = src;
    dest.clear();

    while (*p)
    {
        if ((p[0] == '<') &&
            ((p[1] == 'd' && p[2] == 'i' && p[3] == 'v' && p[4] == ' ') ||
             (p[1] == 'h' && p[2] == '1' && p[3] == ' ')))
        {
            char aTag[256];

            p = getHTMLTag(p, aTag, sizeof(aTag));

            char buf[256];
            int l;

            l = getHTMLValue("class", aTag, buf, sizeof(buf) - 1);

            // We got a color code on our hands, let's see if we need to substitute
            if (l >= 1)
            {
                const char *pCol;

                pCol = color.get(buf);

                if (pCol == NULL)
                    pCol = g_cServerConfig.color.get(buf);

                if (pCol)
                {
                    // Substitute the color
                    char newtag[256];
                    substHTMLTagClass(aTag, "class", pCol, newtag, sizeof(newtag) - 1);
                    dest.push_back('<');
                    dest.append(newtag);
                    dest.push_back('>');
                    //sprintf(dest, "<%s>", newtag);
                    //TAIL(dest);
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
void send_to_descriptor(const char *messg, class descriptor_data *d)
{
    void multi_close(struct multi_element * pe);

    if (d && messg && *messg)
    {
        if (d->prompt_mode == PROMPT_IGNORE)
        {
            d->prompt_mode = PROMPT_EXPECT;
            send_to_descriptor("<br/>", d);
        }

        if (d->multi->bWebsockets)
            protocol_send_text(d->multi, d->id, messg, MULTI_TEXT_CHAR);
        else
        {
            struct unit_data *u  = d->character;

            if (!u || !IS_PC(u)) // switched or snooped?
                u = d->original;

            assert(IS_PC(u));
            string dest;
            dest.reserve(strlen(messg)*1.1);
            //char buf[strlen(messg) + 10000];

            substHTMLcolor(dest, messg, UPC(u)->color);
            protocol_send_text(d->multi, d->id, dest.c_str(), MULTI_TEXT_CHAR);
        }


        if (d->snoop.snoop_by)
        {
            send_to_descriptor(SNOOP_PROMPT, CHAR_DESCRIPTOR(d->snoop.snoop_by));
            send_to_descriptor(messg, CHAR_DESCRIPTOR(d->snoop.snoop_by));
        }
    }
}

void page_string(class descriptor_data *d, const char *messg)
{
    if (d && messg && *messg)
    {
        std::string mystr;

        mystr = "<paged>";
        mystr.append(messg);
        mystr.append("</paged>");

        protocol_send_text(d->multi, d->id, mystr.c_str(), MULTI_PAGE_CHAR);

        if (d->snoop.snoop_by)
        {
            send_to_descriptor(SNOOP_PROMPT,
                               CHAR_DESCRIPTOR(d->snoop.snoop_by));
            send_to_descriptor(messg, CHAR_DESCRIPTOR(d->snoop.snoop_by));
        }
    }
}

void send_to_char(const char *messg, const class unit_data *ch)
{
    if (IS_CHAR(ch))
        send_to_descriptor(messg, CHAR_DESCRIPTOR(ch));
}

void send_to_all(const char *messg)
{
    class descriptor_data *i;

    if (messg && *messg)
        for (i = descriptor_list; i; i = i->next)
            if (descriptor_is_playing(i))
                send_to_descriptor(messg, i);
}

void send_to_zone_outdoor(const class zone_type *z, const char *messg)
{
    class descriptor_data *i;

    if (messg && *messg)
        for (i = descriptor_list; i; i = i->next)
            if (descriptor_is_playing(i) &&
                UNIT_IS_OUTSIDE(i->character) &&
                unit_zone(i->character) == z &&
                CHAR_AWAKE(i->character) &&
                !IS_SET(UNIT_FLAGS(UNIT_IN(i->character)), UNIT_FL_NO_WEATHER) &&
                !IS_SET(UNIT_FLAGS(unit_room(i->character)), UNIT_FL_NO_WEATHER))
                send_to_descriptor(messg, i);
}

void send_to_outdoor(const char *messg)
{
    class descriptor_data *i;

    if (messg && *messg)
        for (i = descriptor_list; i; i = i->next)
            if (descriptor_is_playing(i) &&
                UNIT_IS_OUTSIDE(i->character) &&
                CHAR_AWAKE(i->character) &&
                !IS_SET(UNIT_FLAGS(UNIT_IN(i->character)), UNIT_FL_NO_WEATHER) &&
                !IS_SET(UNIT_FLAGS(unit_room(i->character)), UNIT_FL_NO_WEATHER))
                send_to_descriptor(messg, i);
}

// To deal with both telnet and HTML newlines, act() should *always*
// append a <br/> and sact() should never add a <br/>. buf = dest
void act_generate(char *buf, const char *str, int show_type,
                  cActParameter arg1, cActParameter arg2, cActParameter arg3,
                  int type, const class unit_data *to, int bNewline)
{
    register const char *strp;
    register char *point;
    register const char *i = NULL;
    // MS2020 register int uppercase = FALSE;

    class cActParameter *sub;
    /*union {
        const void *vo;
        class unit_data *un;
        const char *str;
        const int *num;
    } sub;*/

    *buf = 0;

    if (!IS_CHAR(to) || arg1.m_u == NULL)
        return;

    //if (!IS_CHAR(to) || !CHAR_DESCRIPTOR(to) || arg1.m_u == NULL)
    //    return;

    if (to == arg1.m_u &&
        (type == TO_ROOM || type == TO_NOTVICT || type == TO_REST))
        return;

    if (to == arg3.m_u && type == TO_NOTVICT)
        return;

    if (UNIT_IN(to) == arg1.m_u && type == TO_REST)
        return;

    if ((show_type == A_HIDEINV &&
         !CHAR_CAN_SEE(to, arg1.m_u)) ||
        (show_type != A_ALWAYS && !CHAR_AWAKE(to)))
        return;

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

            if (i == NULL)
            {
                switch (*++strp)
                {
                case 'n':
                    if (sub->m_u != NULL)
                    {
                        if (CHAR_CAN_SEE(to, sub->m_u))
                        {
                            if (IS_PC(sub->m_u))
                            {
                                /* Upper-case it */
                                // MS 2020 uppercase = TRUE;
                                i = UNIT_NAME((struct unit_data *) sub->m_u);
                            }
                            else
                                i = UNIT_TITLE(sub->m_u).c_str();
                        }
                        else
                            i = SOMETON(sub->m_u);
                    }
                    else
                        slog(LOG_ALL, 0, "NULL n code to act(): %s", str);
                    break;
                case 'N':
                    if (sub->m_u != NULL)
                        i = UNIT_SEE_NAME(to, (struct unit_data *) sub->m_u);
                    else
                        slog(LOG_ALL, 0, "NULL N code to act(): %s", str);
                    break;
                case 'm':
                    if (sub->m_u != NULL)
                        i = HMHR(to, sub->m_u);
                    else
                        slog(LOG_ALL, 0, "NULL m code to act(): %s", str);
                    break;
                case 's':
                    if (sub->m_u != NULL)
                        i = HSHR(to, sub->m_u);
                    else
                        slog(LOG_ALL, 0, "NULL s code to act(): %s", str);
                    break;
                case 'e':
                    if (sub->m_u != NULL)
                        i = HESH(to, sub->m_u);
                    else
                        slog(LOG_ALL, 0, "NULL e code to act(): %s", str);
                    break;
                case 'p':
                    if (sub->m_u != NULL)
                    {
                        if (IS_CHAR(sub->m_u))
                            i = char_pos[CHAR_POS(sub->m_u)];
                        else
                            i = "lying";
                    }
                    else
                        slog(LOG_ALL, 0, "NULL p code to act(): %s", str);
                    break;
                case 'a':
                    if (sub->m_u != NULL)
                        i = UNIT_ANA((struct unit_data *) sub->m_u);
                    else
                        slog(LOG_ALL, 0, "NULL a code to act(): %s", str);
                    break;
                case 'd':
                    if (sub->m_i != NULL)
                        i = itoa(*(sub->m_i));
                    else
                        slog(LOG_ALL, 0, "NULL i code to act(): %s", str);
                    break;
                case 't':
                    if (sub->m_p == NULL)
                        slog(LOG_ALL, 0, "NULL t code to act(): %s", str);
                    i = sub->m_p;
                    break;
                default:
                    slog(LOG_ALL, 0,
                            "ERROR: Illegal second code to act(): %s", str);
                    *point = 0;
                    return;
                } /* switch */
            }

            if (i == NULL)
                i = "NULL";

            /*	  if (uppercase && *i)
            	  {
            	    *point++ = toupper(*i);
            	    i++;
            	    uppercase = FALSE;
            	 }
            */
            while ((*point = *(i++)))
                point++;

            i = NULL;

            ++strp;
        }
        else if (!(*(point++) = *(strp++)))
            break;
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

    char *cpos;

    /* Cap the first letter, but skip all colour and control codes! */
    // MS2020: rewrite to skip HTML codes and uppercase first letter
    point = buf;
    while (*point == '<')
    {
        cpos = strchr(point, '>');

        if (cpos)
            point = cpos + 1; // Skip the '>' char
    }
    *point = toupper(*point);
}

// Executes exactly like act() for TO_VICT and TO_CHAR. But never adds newline
// Can't handle other
// because of variance with each target's ability to e.g. see.
// Is there a need for sactother which takes a "to" argument? If we add
// to argument here it will be confusing for TO_VICT and TO_CHAR scenarios.
void sact(char *buf, const char *str, int show_type,
          cActParameter arg1, cActParameter arg2, cActParameter arg3, int type)
{
    const class unit_data *to;

    /* This to catch old-style FALSE/TRUE calls...  */
    assert(show_type == A_SOMEONE || show_type == A_HIDEINV || show_type == A_ALWAYS);

    if (!str || !*str)
        return;

    *buf = 0;

    if (type == TO_VICT)
    {
        to = arg3.m_u;
        if (IS_CHAR(to))
            act_generate(buf, str, show_type, arg1, arg2, arg3, type, to, 0); // No newline
        return;
    }
    else if (type == TO_CHAR)
    {
        to = arg1.m_u;
        if (IS_CHAR(to))
            act_generate(buf, str, show_type, arg1, arg2, arg3, type, to, 0);
    }
}

// Always adds <br/> at the end
void act(const char *str, int show_type,
         cActParameter arg1, cActParameter arg2, cActParameter arg3, int type)
{
    const class unit_data *to, *u;
    char buf[MAX_STRING_LENGTH];

    /* This to catch old-style FALSE/TRUE calls...  */
    assert(show_type == A_SOMEONE || show_type == A_HIDEINV || show_type == A_ALWAYS);

    if (!str || !*str)
        return;

    if (type == TO_VICT)
        to = arg3.m_u;
    else if (type == TO_CHAR)
        to = arg1.m_u;
    else if (arg1.m_u == NULL || UNIT_IN(arg1.m_u) == NULL)
        return;
    else
        to = UNIT_CONTAINS(UNIT_IN(arg1.m_u));

    /* same unit or to person */
    for (; to; to = to->next)
    {
        if (IS_CHAR(to) && CHAR_DESCRIPTOR(to))
        {
            act_generate(buf, str, show_type, arg1, arg2, arg3, type, to);
            send_to_descriptor(buf, CHAR_DESCRIPTOR(to));
        }

        if (type == TO_VICT || type == TO_CHAR)
            return;
        if (UNIT_CHARS(to) && UNIT_IS_TRANSPARENT(to))
            for (u = UNIT_CONTAINS(to); u; u = u->next)
                if (IS_CHAR(u) && CHAR_DESCRIPTOR(u))
                {
                    act_generate(buf, str, show_type, arg1, arg2, arg3, type, u);
                    send_to_descriptor(buf, CHAR_DESCRIPTOR(u));
                }
    }

    /* other units outside transparent unit */
    if (UNIT_IN(arg1.m_u) && (to = UNIT_IN(UNIT_IN(arg1.m_u))) && UNIT_IS_TRANSPARENT(UNIT_IN(arg1.m_u)))
        for (to = UNIT_CONTAINS(to); to; to = to->next)
        {
            if (IS_CHAR(to) && CHAR_DESCRIPTOR(to))
            {
                act_generate(buf, str, show_type, arg1, arg2, arg3, type, to);
                send_to_descriptor(buf, CHAR_DESCRIPTOR(to));
            }

            if (UNIT_CHARS(to) && UNIT_IS_TRANSPARENT(to) && to != UNIT_IN(arg1.m_u))
                for (u = UNIT_CONTAINS(to); u; u = u->next)
                    if (IS_CHAR(u) && CHAR_DESCRIPTOR(u))
                    {
                        act_generate(buf, str, show_type, arg1, arg2, arg3, type, u);
                        send_to_descriptor(buf, CHAR_DESCRIPTOR(u));
                    }
        }
}

void cact(const char *str, int show_type,
          cActParameter arg1, cActParameter arg2, cActParameter arg3, int type,
          const char *colortype)
{
    const class unit_data *to, *u;
    char buf[MAX_STRING_LENGTH];
    char temp[MAX_STRING_LENGTH], *t;
    char *b = buf;
    /* This to catch old-style FALSE/TRUE calls...  */
    assert(show_type == A_SOMEONE || show_type == A_HIDEINV || show_type == A_ALWAYS);

    if (!str || !*str)
        return;

    if (type == TO_VICT)
        to = arg3.m_u;
    else if (type == TO_CHAR)
        to = arg1.m_u;
    else if (arg1.m_u == NULL || UNIT_IN(arg1.m_u) == NULL)
        return;
    else
        to = UNIT_CONTAINS(UNIT_IN(arg1.m_u));

    /* same unit or to person */
    for (; to; to = to->next)
    {
        if (IS_CHAR(to)  && CHAR_DESCRIPTOR(to))
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
            return;

        if (UNIT_CHARS(to) && UNIT_IS_TRANSPARENT(to))
            for (u = UNIT_CONTAINS(to); u; u = u->next)
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

    /* other units outside transparent unit */
    if ((to = UNIT_IN(UNIT_IN(arg1.m_u))) && UNIT_IS_TRANSPARENT(UNIT_IN(arg1.m_u)))
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
                for (u = UNIT_CONTAINS(to); u; u = u->next)
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
