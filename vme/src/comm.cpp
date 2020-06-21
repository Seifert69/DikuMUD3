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

/* external vars */
extern class descriptor_data *descriptor_list;

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

        protocol_send_text(d->multi, d->id, messg, MULTI_TEXT_CHAR);

        if (d->snoop.snoop_by)
        {
            send_to_descriptor(SNOOP_PROMPT,
                               CHAR_DESCRIPTOR(d->snoop.snoop_by));
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
                  const void *arg1, const void *arg2, const void *arg3,
                  int type, class unit_data *to, int bNewline)
{
    register const char *strp;
    register char *point;
    register const char *i = NULL;
    // MS2020 register int uppercase = FALSE;

    union {
        const void *vo;
        class unit_data *un;
        const char *str;
        const int *num;
    } sub;

    *buf = 0;

    if (!IS_CHAR(to) || !CHAR_DESCRIPTOR(to) || arg1 == NULL)
        return;

    if (to == (class unit_data *)arg1 &&
        (type == TO_ROOM || type == TO_NOTVICT || type == TO_REST))
        return;

    if (to == (class unit_data *)arg3 && type == TO_NOTVICT)
        return;

    if (UNIT_IN(to) == (class unit_data *)arg1 && type == TO_REST)
        return;

    if ((show_type == A_HIDEINV &&
         !CHAR_CAN_SEE(to, (class unit_data *)arg1)) ||
        (show_type != A_ALWAYS && !CHAR_AWAKE(to)))
        return;

    for (strp = str, point = buf;;)
    {
        if (*strp == '$')
        {
            switch (*++strp)
            {
            case '1':
                sub.vo = arg1;
                break;
            case '2':
                sub.vo = arg2;
                break;
            case '3':
                sub.vo = arg3;
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
                if (sub.vo != NULL)
                {
                    switch (*++strp)
                    {
                    case 'n':
                        if (CHAR_CAN_SEE(to, sub.un))
                        {
                            if (IS_PC(sub.un))
                            {
                                /* Upper-case it */
                                // MS 2020 uppercase = TRUE;
                                i = UNIT_NAME(sub.un);
                            }
                            else
                                i = UNIT_TITLE(sub.un).c_str();
                        }
                        else
                            i = SOMETON(sub.un);
                        break;
                    case 'N':
                        i = UNIT_SEE_NAME(to, sub.un);
                        break;
                    case 'm':
                        i = HMHR(to, sub.un);
                        break;
                    case 's':
                        i = HSHR(to, sub.un);
                        break;
                    case 'e':
                        i = HESH(to, sub.un);
                        break;
                    case 'p':
                        if (IS_CHAR(sub.un))
                            i = char_pos[CHAR_POS(sub.un)];
                        else
                            i = "lying";
                        break;
                    case 'a':
                        i = UNIT_ANA(sub.un);
                        break;
                    case 'd':
                        i = itoa(*(sub.num));
                        break;
                    case 't':
                        i = sub.str;
                        break;
                    default:
                        slog(LOG_ALL, 0,
                             "ERROR: Illegal second code to act(): %s", str);
                        *point = 0;
                        return;
                    } /* switch */
                }
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
          const void *arg1, const void *arg2, const void *arg3, int type)
{
    class unit_data *to;

    /* This to catch old-style FALSE/TRUE calls...  */
    assert(show_type == A_SOMEONE || show_type == A_HIDEINV || show_type == A_ALWAYS);

    if (!str || !*str)
        return;

    *buf = 0;
    
    if (type == TO_VICT)
    {
        to = (class unit_data *)arg3;
        if (IS_CHAR(to))
            act_generate(buf, str, show_type, arg1, arg2, arg3, type, to, 0); // No newline
        return;
    }
    else if (type == TO_CHAR)
    {
        to = (class unit_data *)arg1;
        if (IS_CHAR(to))
            act_generate(buf, str, show_type, arg1, arg2, arg3, type, to, 0);
    }
}

// Always adds <br/> at the end 
void act(const char *str, int show_type,
         const void *arg1, const void *arg2, const void *arg3, int type)
{
    class unit_data *to, *u;
    char buf[MAX_STRING_LENGTH];

    /* This to catch old-style FALSE/TRUE calls...  */
    assert(show_type == A_SOMEONE || show_type == A_HIDEINV || show_type == A_ALWAYS);

    if (!str || !*str)
        return;

    if (type == TO_VICT)
        to = (class unit_data *)arg3;
    else if (type == TO_CHAR)
        to = (class unit_data *)arg1;
    else if (arg1 == NULL || UNIT_IN((class unit_data *)arg1) == NULL)
        return;
    else
        to = UNIT_CONTAINS(UNIT_IN((class unit_data *)arg1));

    /* same unit or to person */
    for (; to; to = to->next)
    {
        if (IS_CHAR(to))
        {
            act_generate(buf, str, show_type, arg1, arg2, arg3, type, to);
            send_to_descriptor(buf, CHAR_DESCRIPTOR(to));
        }

        if (type == TO_VICT || type == TO_CHAR)
            return;
        if (UNIT_CHARS(to) && UNIT_IS_TRANSPARENT(to))
            for (u = UNIT_CONTAINS(to); u; u = u->next)
                if
                    IS_CHAR(u)
                    {
                        act_generate(buf, str, show_type, arg1, arg2, arg3, type, u);
                        send_to_descriptor(buf, CHAR_DESCRIPTOR(u));
                    }
    }

    /* other units outside transparent unit */
    if ((to = UNIT_IN(UNIT_IN((class unit_data *)arg1))) && UNIT_IS_TRANSPARENT(UNIT_IN((class unit_data *)arg1)))
        for (to = UNIT_CONTAINS(to); to; to = to->next)
        {
            if (IS_CHAR(to))
            {
                act_generate(buf, str, show_type, arg1, arg2, arg3, type, to);
                send_to_descriptor(buf, CHAR_DESCRIPTOR(to));
            }

            if (UNIT_CHARS(to) && UNIT_IS_TRANSPARENT(to) && to != UNIT_IN((class unit_data *)arg1))
                for (u = UNIT_CONTAINS(to); u; u = u->next)
                    if (IS_CHAR(u))
                    {
                        act_generate(buf, str, show_type, arg1, arg2, arg3, type, u);
                        send_to_descriptor(buf, CHAR_DESCRIPTOR(u));
                    }
        }
}

void cact(const char *str, int show_type,
          const void *arg1, const void *arg2, const void *arg3, int type,
          const char *colortype)
{
    class unit_data *to, *u;
    char buf[MAX_STRING_LENGTH];
    char temp[MAX_STRING_LENGTH], *t;
    char *b = buf;
    /* This to catch old-style FALSE/TRUE calls...  */
    assert(show_type == A_SOMEONE || show_type == A_HIDEINV || show_type == A_ALWAYS);

    if (!str || !*str)
        return;

    if (type == TO_VICT)
        to = (class unit_data *)arg3;
    else if (type == TO_CHAR)
        to = (class unit_data *)arg1;
    else if (arg1 == NULL || UNIT_IN((class unit_data *)arg1) == NULL)
        return;
    else
        to = UNIT_CONTAINS(UNIT_IN((class unit_data *)arg1));

    /* same unit or to person */
    for (; to; to = to->next)
    {
        if (IS_CHAR(to))
        {
            *buf = 0;
            b = buf;
            *temp = 0;
            t = temp;
            act_generate(t, str, show_type, arg1, arg2, arg3, type, to, false);
            if (!str_is_empty(t))
            {
                strcat(b, getcolor(colortype)); // Adds <div class='colortype'>
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
                if
                    IS_CHAR(u)
                    {
                        *buf = 0;
                        b = buf;
                        strcat(b, getcolor(colortype));
                        TAIL(b);
                        act_generate(buf, str, show_type, arg1, arg2, arg3, type, u, false);
                        TAIL(b);
                        strcat(b, "</div><br/>");
                        send_to_descriptor(buf, CHAR_DESCRIPTOR(u));
                    }
    }

    /* other units outside transparent unit */
    if ((to = UNIT_IN(UNIT_IN((class unit_data *)arg1))) && UNIT_IS_TRANSPARENT(UNIT_IN((class unit_data *)arg1)))
        for (to = UNIT_CONTAINS(to); to; to = to->next)
        {
            if (IS_CHAR(to))
            {
                *buf = 0;
                b = buf;
                strcat(b, getcolor(colortype));
                TAIL(b);
                act_generate(buf, str, show_type, arg1, arg2, arg3, type, to, false);
                TAIL(b);
                strcat(b, "</div><br/>");
                send_to_descriptor(buf, CHAR_DESCRIPTOR(to));
            }

            if (UNIT_CHARS(to) && UNIT_IS_TRANSPARENT(to) && to != UNIT_IN((class unit_data *)arg1))
                for (u = UNIT_CONTAINS(to); u; u = u->next)
                    if (IS_CHAR(u))
                    {
                        *buf = 0;
                        b = buf;
                        strcat(b, getcolor(colortype));
                        TAIL(b);
                        act_generate(buf, str, show_type, arg1, arg2, arg3, type, u,false);
                        TAIL(b);
                        strcat(b, "</div><br/>");
                        send_to_descriptor(buf, CHAR_DESCRIPTOR(u));
                    }
        }
}
