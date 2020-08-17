/*
 $Author: All $
 $RCSfile: act_color.cpp,v $
 $Date: 2004/09/18 19:52:56 $
 $Revision: 2.6 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "handler.h"
#include "interpreter.h"
#include "files.h"
#include "skills.h"
#include "db.h"
#include "spells.h"
#include "vmelimits.h"
#include "affect.h"
#include "utility.h"
#include "trie.h"
#include "textutil.h"
#include "money.h"
#include "protocol.h"
#include "constants.h"
#include "common.h"
#include "guild.h"
#include "dilrun.h"

void do_color(class unit_data *ch, char *aaa, const struct command_info *cmd)
{
    char fore[MAX_INPUT_LENGTH];
    char back[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    char cbuf[MAX_STRING_LENGTH];
    char full_name[21];
    char *print_str;
    int change = FALSE, add = FALSE;

    char *arg = (char *)aaa;
    if (!IS_PC(ch))
    {
        send_to_char("Warning: A non pc trying to change colors", ch);
        return;
    }

    if (!CHAR_DESCRIPTOR(ch))
        return;

    arg = one_argument(arg, buf);

    if (str_is_empty(buf))
    {
        print_str = UPC(ch)->color.key_string(g_cServerConfig.color);
        page_string(CHAR_DESCRIPTOR(ch), print_str);
        send_to_char("<br/><br/>", ch);
        send_to_char("Example: color default <forground color> <background color><br/>", ch);
        FREE(print_str);
        return;
    }
    if (strcmp(buf, "reset") == 0)
    {
        send_to_char("Resetting your colors to default system colors.<br/>", ch);
        UPC(ch)->color.remove_all();
        return;
    }

    if (UPC(ch)->color.get(buf, full_name))
    {
        change = TRUE;
    }

    if (g_cServerConfig.color.get(buf, full_name))
    {
        add = TRUE;
    }

    if (!change && !add)
    {
        send_to_char("No such color.<br/>", ch);
        return;
    }
    arg = one_argument(arg, fore);
    if (str_is_empty(fore))
    {
        if ((change == TRUE) && (add == TRUE))
        {
            if (UPC(ch)->color.remove(full_name))
            {
                sprintf(cbuf, "Reseting %s to sytem colors.<br/>", full_name);
                // send_to_char(cbuf, ch);
                // sprintf (cbuf, "%s%s%s", CONTROL_COLOR_REMOVE, full_name, CONTROL_COLOR_END);
            }
            else
                sprintf(cbuf,
                        "Error: Can not reset %s to default color, report to admin.<br/>",
                        full_name);
        }
        else
        {
            sprintf(cbuf, "%s is already set to system color.<br/>", full_name);
        }
        send_to_char(cbuf, ch);
        return;
    }

    if (!is_forground(fore))
    {
        sprintf(cbuf, "Invalid color for the forground color you typed '%s'<br/>", fore);
        send_to_char(cbuf, ch);
        return;
    }

    arg = one_argument(arg, back);
    if (str_is_empty(back))
    {
        send_to_char("You must supply a backgroud color.<br/>", ch);
        return;
    }
    if (!is_background(back))
    {
        sprintf(cbuf, "Invalid color for the background color you typed '%s'<br/>", back);
        send_to_char(cbuf, ch);
        return;
    }

    sprintf(cbuf, "%s %s", fore, back);

    if (change == TRUE)
    {
        string mystr; 

        mystr = UPC(ch)->color.change(full_name, cbuf);
        sprintf(cbuf, "Color %s changed.<br/>", mystr.c_str());
        send_to_char(cbuf, ch);
        return;
    }
    if ((add == TRUE) && (change == FALSE))
    {
        print_str = UPC(ch)->color.insert(full_name, cbuf);
        sprintf(cbuf, "Color %s changed.<br/>", print_str);
        send_to_char(cbuf, ch);
        FREE(print_str);
        return;
    }
    return;
}

// Test validity of e.g. cg or cpg
int is_forground(char *cstr)
{
    if ((strlen(cstr) > 3) || (strlen(cstr) < 2))
        return FALSE;

    if (cstr[0] != 'c')
        return FALSE;

    if (strlen(cstr) == 3)
    {
        if (cstr[1] != 'p')
            return (FALSE);
        cstr++;
    }

    cstr++; // skip c (or c and p)

    switch (*cstr)
    {
    case 'p':
        return FALSE;
    case 'n':
    case 'r':
    case 'g':
    case 'y':
    case 'b':
    case 'm':
    case 'c':
    case 'w':
        break;
    default:
        return FALSE;
    }

    return TRUE;
}

int is_background(char *cstr)
{
    if (strlen(cstr) != 2)
        return FALSE;

    if (cstr[0] != 'b')
        return FALSE;

    switch (cstr[1])
    {
    case 'n':
    case 'r':
    case 'g':
    case 'y':
    case 'b':
    case 'm':
    case 'c':
    case 'w':
        break;
    default:
        return FALSE;
    }

    return TRUE;
}
