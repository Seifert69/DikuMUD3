/*
 $Author: All $
 $RCSfile: translate.cpp,v $
 $Date: 2001/05/06 07:08:03 $
 $Revision: 2.1 $
 */

#ifdef _WINDOWS
#include <string.h>
#include <winsock.h>
#include "telnet.h"
#endif

#ifdef LINUX
#include <strings.h>
#include <arpa/telnet.h>
#endif

#include <assert.h>
#include <stdio.h>

#include "mplex.h"
#include "ClientConnector.h"
#include "ttydef.h"
#include "translate.h"

static ubit8 default_colours[3][24] = {
    /* Default (no change in table) */
    {CONTROL_FG_BLACK_CHAR, CONTROL_FG_RED_CHAR, CONTROL_FG_GREEN_CHAR,
     CONTROL_FG_YELLOW_CHAR, CONTROL_FG_BLUE_CHAR, CONTROL_FG_MAGENTA_CHAR,
     CONTROL_FG_CYAN_CHAR, CONTROL_FG_WHITE_CHAR,

     CONTROL_FGB_BLACK_CHAR, CONTROL_FGB_RED_CHAR, CONTROL_FGB_GREEN_CHAR,
     CONTROL_FGB_YELLOW_CHAR, CONTROL_FGB_BLUE_CHAR, CONTROL_FGB_MAGENTA_CHAR,
     CONTROL_FGB_CYAN_CHAR, CONTROL_FGB_WHITE_CHAR,

     CONTROL_BG_BLACK_CHAR, CONTROL_BG_RED_CHAR, CONTROL_BG_GREEN_CHAR,
     CONTROL_BG_YELLOW_CHAR, CONTROL_BG_BLUE_CHAR, CONTROL_BG_MAGENTA_CHAR,
     CONTROL_BG_CYAN_CHAR, CONTROL_BG_WHITE_CHAR},

    {CONTROL_FG_BLACK_CHAR, CONTROL_FG_RED_CHAR, CONTROL_FG_YELLOW_CHAR,
     CONTROL_FG_YELLOW_CHAR, CONTROL_FG_BLUE_CHAR, CONTROL_FG_MAGENTA_CHAR,
     CONTROL_FG_BLUE_CHAR, CONTROL_FG_BLACK_CHAR,

     CONTROL_FGB_BLACK_CHAR, CONTROL_FGB_RED_CHAR, CONTROL_FGB_YELLOW_CHAR,
     CONTROL_FGB_YELLOW_CHAR, CONTROL_FGB_BLUE_CHAR, CONTROL_FGB_MAGENTA_CHAR,
     CONTROL_FGB_BLUE_CHAR, CONTROL_FGB_BLACK_CHAR,

     CONTROL_BG_WHITE_CHAR, CONTROL_BG_RED_CHAR, CONTROL_BG_GREEN_CHAR,
     CONTROL_BG_YELLOW_CHAR, CONTROL_BG_BLUE_CHAR, CONTROL_BG_MAGENTA_CHAR,
     CONTROL_BG_CYAN_CHAR, CONTROL_BG_WHITE_CHAR},

    {CONTROL_FG_BLACK_CHAR, CONTROL_FG_MAGENTA_CHAR, CONTROL_FG_GREEN_CHAR,
     CONTROL_FG_YELLOW_CHAR, CONTROL_FG_BLUE_CHAR, CONTROL_FG_MAGENTA_CHAR,
     CONTROL_FG_CYAN_CHAR, CONTROL_FG_WHITE_CHAR,

     CONTROL_FGB_BLACK_CHAR, CONTROL_FGB_MAGENTA_CHAR, CONTROL_FGB_GREEN_CHAR,
     CONTROL_FGB_YELLOW_CHAR, CONTROL_FGB_BLUE_CHAR, CONTROL_FGB_MAGENTA_CHAR,
     CONTROL_FGB_CYAN_CHAR, CONTROL_FGB_WHITE_CHAR,

     CONTROL_BG_BLUE_CHAR, CONTROL_BG_RED_CHAR, CONTROL_BG_GREEN_CHAR,
     CONTROL_BG_YELLOW_CHAR, CONTROL_BG_BLUE_CHAR, CONTROL_BG_MAGENTA_CHAR,
     CONTROL_BG_CYAN_CHAR, CONTROL_BG_WHITE_CHAR}};

void (*control_code[5][256])(class cConHook *con, char **b, ubit8 code);

#define TELNET_SEND_GA(buf) sprintf(buf, "%c%c", IAC, GA)
#define TELNET_ECHO_OFF(buf) sprintf(buf, "%c%c%c", IAC, WILL, TELOPT_ECHO)
#define TELNET_ECHO_ON(buf) sprintf(buf, "%c%c%c", IAC, WONT, TELOPT_ECHO)

/* ----------------------------------------------------------------------- */

#define PROTOCOL_TRANSLATE(con, code, b)               \
    if (control_code[(con)->m_sSetup.emulation][code]) \
        (*control_code[(con)->m_sSetup.emulation][code])((con), (b), (code));

void protocol_translate(class cConHook *con, ubit8 code, char **b)
{
    if (code >= CONTROL_FG_BLACK_CHAR && code <= CONTROL_BG_WHITE_CHAR)
        code = default_colours[con->m_sSetup.colour_convert]
                              [code - CONTROL_FG_BLACK_CHAR];
    if (control_code[(con)->m_sSetup.emulation][code])
        (*control_code[(con)->m_sSetup.emulation][code])((con), (b), (code));

    //   PROTOCOL_TRANSLATE(con, code, b);
}

/* ======================================================================= */
/*                             2020 HTML->Telnet                           */
/* I don't even know what is really out there. Telnet of course. But       */
/* anyone using TTY or ANSI in 2020 ?? :o) Is telnet ANSI? I forgot...     */
/* ======================================================================= */

void Control_Echo_Off(class cConHook *con, char **b, ubit8 code)
{
    if (con->m_sSetup.telnet || (con->m_sSetup.emulation == TERM_TTY))
    {
        TELNET_ECHO_OFF(*b);
        TAIL(*b);
    }
    else if (con->m_sSetup.emulation == TERM_ANSI)
    {
        strcpy(*b, ANSI_CONCEALED);
        TAIL(*b);
    }
}

void Control_Echo_On(class cConHook *con, char **b, ubit8 code)
{
    if (con->m_sSetup.telnet || (con->m_sSetup.emulation == TERM_TTY))
    {
        TELNET_ECHO_ON(*b);
        TAIL(*b);
    }
    else if (con->m_sSetup.emulation == TERM_ANSI)
    {
        strcpy(*b, ANSI_RESET);
        TAIL(*b);
    }
}

/* ======================================================================= */
/*                             A N S I                                     */
/* ======================================================================= */

void Control_ANSI_Fg(class cConHook *con, char **dest, ubit8 code, int bBold)
{
    switch (code)
    {
    case 'r':
        if (bBold)
            Control_ANSI_Fg_Red(con, dest, 0);
        else
            Control_ANSI_Fgb_Red(con, dest, 0);
        break;

    case 'n':
        if (bBold)
            Control_ANSI_Fgb_Black(con, dest, 0);
        else
            Control_ANSI_Fg_Black(con, dest, 0);
        break;

    case 'g':
        if (bBold)
            Control_ANSI_Fgb_Green(con, dest, 0);
        else
            Control_ANSI_Fg_Green(con, dest, 0);
        break;

    case 'y':
        if (bBold)
            Control_ANSI_Fgb_Yellow(con, dest, 0);
        else
            Control_ANSI_Fg_Yellow(con, dest, 0);
        break;

    case 'b':
        if (bBold)
            Control_ANSI_Fgb_Blue(con, dest, 0);
        else
            Control_ANSI_Fg_Blue(con, dest, 0);
        break;

    case 'm':
        if (bBold)
            Control_ANSI_Fgb_Magenta(con, dest, 0);
        else
            Control_ANSI_Fg_Magenta(con, dest, 0);
        break;

    case 'c':
        if (bBold)
            Control_ANSI_Fgb_Cyan(con, dest, 0);
        else
            Control_ANSI_Fg_Cyan(con, dest, 0);
        break;

    case 'w':
        if (bBold)
            Control_ANSI_Fgb_White(con, dest, 0);
        else
            Control_ANSI_Fg_White(con, dest, 0);
        break;

    default:
        slog(LOG_ALL, 0, "Illegal fg color code %c", code);
        break;
    }

    if ((con->m_nBgColor >= (ubit8)CONTROL_FG_BLACK_CHAR) &&
        (con->m_nBgColor <= (ubit8)CONTROL_BG_WHITE_CHAR))
        PROTOCOL_TRANSLATE(con, con->m_nBgColor, dest);
}

void Control_ANSI_Bg(class cConHook *con, char **dest, ubit8 code)
{
    switch (code)
    {
    case 'r':
        Control_ANSI_Bg_Red(con, dest, 0);
        break;

    case 'n':
        Control_ANSI_Bg_Black(con, dest, 0);
        break;

    case 'g':
        Control_ANSI_Bg_Green(con, dest, 0);
        break;

    case 'y':
        Control_ANSI_Bg_Yellow(con, dest, 0);
        break;

    case 'b':
        Control_ANSI_Bg_Blue(con, dest, 0);
        break;

    case 'm':
        Control_ANSI_Bg_Magenta(con, dest, 0);
        break;

    case 'c':
        Control_ANSI_Bg_Cyan(con, dest, 0);
        break;

    case 'w':
        Control_ANSI_Bg_White(con, dest, 0);
        break;

    default:
        slog(LOG_ALL, 0, "Illegal bg color code %c", code);
        break;
    }
}

void Control_ANSI_Fg_Black(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_FG_BLACK);
    TAIL(*b);

    if ((con->m_nBgColor >= (ubit8)CONTROL_FG_BLACK_CHAR) &&
        (con->m_nBgColor <= (ubit8)CONTROL_BG_WHITE_CHAR))
        PROTOCOL_TRANSLATE(con, con->m_nBgColor, b);
}

void Control_ANSI_Fg_Red(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_FG_RED);
    TAIL(*b);

    if ((con->m_nBgColor >= (ubit8)CONTROL_FG_BLACK_CHAR) &&
        (con->m_nBgColor <= (ubit8)CONTROL_BG_WHITE_CHAR))
        PROTOCOL_TRANSLATE(con, con->m_nBgColor, b);
}

void Control_ANSI_Fg_Green(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_FG_GREEN);
    TAIL(*b);

    if ((con->m_nBgColor >= (ubit8)CONTROL_FG_BLACK_CHAR) &&
        (con->m_nBgColor <= (ubit8)CONTROL_BG_WHITE_CHAR))
        PROTOCOL_TRANSLATE(con, con->m_nBgColor, b);
}

void Control_ANSI_Fg_Yellow(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_FG_YELLOW);
    TAIL(*b);

    if ((con->m_nBgColor >= (ubit8)CONTROL_FG_BLACK_CHAR) &&
        (con->m_nBgColor <= (ubit8)CONTROL_BG_WHITE_CHAR))
        PROTOCOL_TRANSLATE(con, con->m_nBgColor, b);
}

void Control_ANSI_Fg_Blue(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_FG_BLUE);
    TAIL(*b);

    if ((con->m_nBgColor >= (ubit8)CONTROL_FG_BLACK_CHAR) &&
        (con->m_nBgColor <= (ubit8)CONTROL_BG_WHITE_CHAR))
        PROTOCOL_TRANSLATE(con, con->m_nBgColor, b);
}

void Control_ANSI_Fg_Magenta(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_FG_MAGENTA);
    TAIL(*b);

    if ((con->m_nBgColor >= (ubit8)CONTROL_FG_BLACK_CHAR) &&
        (con->m_nBgColor <= (ubit8)CONTROL_BG_WHITE_CHAR))
        PROTOCOL_TRANSLATE(con, con->m_nBgColor, b);
}

void Control_ANSI_Fg_Cyan(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_FG_CYAN);
    TAIL(*b);

    if ((con->m_nBgColor >= (ubit8)CONTROL_FG_BLACK_CHAR) &&
        (con->m_nBgColor <= (ubit8)CONTROL_BG_WHITE_CHAR))
        PROTOCOL_TRANSLATE(con, con->m_nBgColor, b);
}

void Control_ANSI_Fg_White(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_FG_WHITE);
    TAIL(*b);

    if ((con->m_nBgColor >= (ubit8)CONTROL_FG_BLACK_CHAR) &&
        (con->m_nBgColor <= (ubit8)CONTROL_BG_WHITE_CHAR))
        PROTOCOL_TRANSLATE(con, con->m_nBgColor, b);
}

void Control_ANSI_Fgb_Black(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_FGB_BLACK);
    TAIL(*b);
}

void Control_ANSI_Fgb_Red(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_FGB_RED);
    TAIL(*b);
}

void Control_ANSI_Fgb_Green(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_FGB_GREEN);
    TAIL(*b);
}

void Control_ANSI_Fgb_Yellow(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_FGB_YELLOW);
    TAIL(*b);
}

void Control_ANSI_Fgb_Blue(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_FGB_BLUE);
    TAIL(*b);
}

void Control_ANSI_Fgb_Magenta(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_FGB_MAGENTA);
    TAIL(*b);
}

void Control_ANSI_Fgb_Cyan(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_FGB_CYAN);
    TAIL(*b);
}

void Control_ANSI_Fgb_White(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_FGB_WHITE);
    TAIL(*b);
}

void Control_ANSI_Bg_Black(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_BG_BLACK);
    TAIL(*b);
    con->m_nBgColor = CONTROL_BG_BLACK_CHAR;
}

void Control_ANSI_Bg_Red(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_BG_RED);
    TAIL(*b);
    con->m_nBgColor = CONTROL_BG_RED_CHAR;
}

void Control_ANSI_Bg_Green(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_BG_GREEN);
    TAIL(*b);
    con->m_nBgColor = CONTROL_BG_GREEN_CHAR;
}

void Control_ANSI_Bg_Yellow(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_BG_YELLOW);
    TAIL(*b);
    con->m_nBgColor = CONTROL_BG_YELLOW_CHAR;
}

void Control_ANSI_Bg_Blue(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_BG_BLUE);
    TAIL(*b);
    con->m_nBgColor = CONTROL_BG_BLUE_CHAR;
}

void Control_ANSI_Bg_Magenta(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_BG_MAGENTA);
    TAIL(*b);
    con->m_nBgColor = CONTROL_BG_MAGENTA_CHAR;
}

void Control_ANSI_Bg_Cyan(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_BG_CYAN);
    TAIL(*b);
    con->m_nBgColor = CONTROL_BG_CYAN_CHAR;
}

void Control_ANSI_Bg_White(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_BG_WHITE);
    TAIL(*b);
    con->m_nBgColor = CONTROL_BG_WHITE_CHAR;
}

void Control_ANSI_Echo_Off(class cConHook *con, char **b, ubit8 code)
{
    if (con->m_sSetup.telnet)
    {
        TELNET_ECHO_OFF(*b);
        TAIL(*b);
    }

    strcpy(*b, ANSI_CONCEALED);
    TAIL(*b);
}

void Control_ANSI_Reset(class cConHook *con, char **b, ubit8 code)
{
    if (con->m_sSetup.telnet)
    {
        TELNET_ECHO_ON(*b);
        TAIL(*b);
    }

    strcpy(*b, ANSI_RESET);
    TAIL(*b);

    Control_ANSI_Fg_White(con, b, code);
}

void Control_ANSI_Home(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_HOME);
    TAIL(*b);
    con->m_nPromptLen = 0;
}

void Control_ANSI_Bold(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_BOLD);
    TAIL(*b);
}

void Control_ANSI_Reverse(class cConHook *con, char **b, ubit8 code)
{
    strcpy(*b, ANSI_REVERSE);
    TAIL(*b);
}

void Control_TTY_Echo_Off(class cConHook *con, char **b, ubit8 code)
{
    if (con->m_sSetup.telnet)
    {
        TELNET_ECHO_OFF(*b);
        TAIL(*b);
    }
}

void Control_TTY_Echo_On(class cConHook *con, char **b, ubit8 code)
{
    if (con->m_sSetup.telnet)
    {
        TELNET_ECHO_ON(*b);
        TAIL(*b);
    }
}

void Control_Copy(class cConHook *con, char **b, ubit8 code)
{
    **b = CONTROL_CHAR;
    (*b)++;
    **b = code;
    (*b)++;
}

void Control_Gobble_On(class cConHook *con, char **b, ubit8 code)
{
    con->m_bGobble = true;
}

void Control_Gobble_Off(class cConHook *con, char **b, ubit8 code)
{
    con->m_bGobble = false;
}

void control_color_change(class cConHook *con, char **b, ubit8 code)
{
    con->m_bColorChange = true;
}

void control_color_insert(class cConHook *con, char **b, ubit8 code)
{
    con->m_bColorInsert = true;
}

void control_color_create(class cConHook *con, char **b, ubit8 code)
{
    con->m_bColorCreate = true;
}

void control_color_remove(class cConHook *con, char **b, ubit8 code)
{
    con->m_bColorRemove = true;
}

void control_color_disp(class cConHook *con, char **b, ubit8 code)
{
    con->m_bColorDisp = true;
}

void control_color_end(class cConHook *con, char **b, ubit8 code)
{
    con->m_bColorCreate = false;
    con->m_bColorChange = false;
    con->m_bColorInsert = false;
    con->m_bColorDelete = false;
    con->m_bColorDisp = false;
    con->m_bColorRemove = false;
}

void translate_init(void)
{
    int i, j;

    for (j = 0; j < 4; j++)
        for (i = 0; i < 256; i++)
            control_code[j][i] = NULL;

    for (j = 0; j < 4; j++)
    {
        control_code[j][CONTROL_GOBBLE_ON_CHAR] = Control_Gobble_On;
        control_code[j][CONTROL_GOBBLE_OFF_CHAR] = Control_Gobble_Off;
    }

    for (j = 0; j < 4; j++)
    {
        control_code[j][CONTROL_COLOR_CHANGE_CHAR] = control_color_change;
        control_code[j][CONTROL_COLOR_INSERT_CHAR] = control_color_insert;
        control_code[j][CONTROL_COLOR_CREATE_CHAR] = control_color_create;
        control_code[j][CONTROL_COLOR_REMOVE_CHAR] = control_color_remove;
        control_code[j][CONTROL_COLOR_REMOVE_CHAR] = control_color_remove;
        control_code[j][CONTROL_COLOR_DISP_CHAR] = control_color_disp;
        control_code[j][CONTROL_COLOR_END_CHAR] = control_color_end;
    }

    control_code[TERM_TTY][CONTROL_ECHO_OFF_CHAR] = Control_TTY_Echo_Off;
    control_code[TERM_TTY][CONTROL_ECHO_ON_CHAR] = Control_TTY_Echo_On;
    control_code[TERM_TTY][CONTROL_RESET_CHAR] = Control_TTY_Echo_On;

    control_code[TERM_ANSI][CONTROL_ECHO_OFF_CHAR] = Control_ANSI_Echo_Off;
    control_code[TERM_ANSI][CONTROL_ECHO_ON_CHAR] = Control_ANSI_Reset;
    control_code[TERM_ANSI][CONTROL_RESET_CHAR] = Control_ANSI_Reset;
    control_code[TERM_ANSI][CONTROL_HOME_CHAR] = Control_ANSI_Home;

    control_code[TERM_ANSI][CONTROL_BOLD_CHAR] = Control_ANSI_Bold;
    control_code[TERM_ANSI][CONTROL_REVERSE_CHAR] = Control_ANSI_Reverse;

    control_code[TERM_ANSI][CONTROL_FG_BLACK_CHAR] = Control_ANSI_Fg_Black;
    control_code[TERM_ANSI][CONTROL_FG_RED_CHAR] = Control_ANSI_Fg_Red;
    control_code[TERM_ANSI][CONTROL_FG_GREEN_CHAR] = Control_ANSI_Fg_Green;
    control_code[TERM_ANSI][CONTROL_FG_YELLOW_CHAR] = Control_ANSI_Fg_Yellow;
    control_code[TERM_ANSI][CONTROL_FG_BLUE_CHAR] = Control_ANSI_Fg_Blue;
    control_code[TERM_ANSI][CONTROL_FG_MAGENTA_CHAR] = Control_ANSI_Fg_Magenta;
    control_code[TERM_ANSI][CONTROL_FG_CYAN_CHAR] = Control_ANSI_Fg_Cyan;
    control_code[TERM_ANSI][CONTROL_FG_WHITE_CHAR] = Control_ANSI_Fg_White;

    control_code[TERM_ANSI][CONTROL_FGB_BLACK_CHAR] = Control_ANSI_Fgb_Black;
    control_code[TERM_ANSI][CONTROL_FGB_RED_CHAR] = Control_ANSI_Fgb_Red;
    control_code[TERM_ANSI][CONTROL_FGB_GREEN_CHAR] = Control_ANSI_Fgb_Green;
    control_code[TERM_ANSI][CONTROL_FGB_YELLOW_CHAR] = Control_ANSI_Fgb_Yellow;
    control_code[TERM_ANSI][CONTROL_FGB_BLUE_CHAR] = Control_ANSI_Fgb_Blue;
    control_code[TERM_ANSI][CONTROL_FGB_MAGENTA_CHAR] = Control_ANSI_Fgb_Magenta;

    control_code[TERM_ANSI][CONTROL_FGB_CYAN_CHAR] = Control_ANSI_Fgb_Cyan;
    control_code[TERM_ANSI][CONTROL_FGB_WHITE_CHAR] = Control_ANSI_Fgb_White;

    control_code[TERM_ANSI][CONTROL_BG_BLACK_CHAR] = Control_ANSI_Bg_Black;
    control_code[TERM_ANSI][CONTROL_BG_RED_CHAR] = Control_ANSI_Bg_Red;
    control_code[TERM_ANSI][CONTROL_BG_GREEN_CHAR] = Control_ANSI_Bg_Green;
    control_code[TERM_ANSI][CONTROL_BG_YELLOW_CHAR] = Control_ANSI_Bg_Yellow;
    control_code[TERM_ANSI][CONTROL_BG_BLUE_CHAR] = Control_ANSI_Bg_Blue;
    control_code[TERM_ANSI][CONTROL_BG_MAGENTA_CHAR] = Control_ANSI_Bg_Magenta;
    control_code[TERM_ANSI][CONTROL_BG_CYAN_CHAR] = Control_ANSI_Bg_Cyan;
    control_code[TERM_ANSI][CONTROL_BG_WHITE_CHAR] = Control_ANSI_Bg_White;

    for (i = 0; i < 256; i++)
        control_code[TERM_INTERNAL][i] = Control_Copy;
}
