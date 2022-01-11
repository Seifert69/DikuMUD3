/*
 $Author: tperry $
 $RCSfile: ttydef.h,v $
 $Date: 2001/04/29 03:46:07 $
 $Revision: 2.1 $
 */
#pragma once

#define BACKSPACE1_CHAR '\b' /* The two standard BACKSPACE chars */
#define BACKSPACE2_CHAR '\177'

#define ANSI_DELETE "^[\177"  /* Literal! */
#define ANSI_BACKSPACE "^[[D" /* Literal! */

#define ANSI_CURSOR_UP "\033[1A"
#define ANSI_CURSOR_DOWN "\033[1B"
#define ANSI_CURSOR_FORWARD "\033[1C"
#define ANSI_CURSOR_BACK "\033[1D"
#define ANSI_ERASE_LINE "\033[K"

#define ANSI_RESET "\033[0m"
#define ANSI_BOLD "\033[1m"
#define ANSI_REVERSE "\033[7m"
#define ANSI_CONCEALED "\033[8m"
#define ANSI_HOME "\033[2J"

#define ANSI_FG_BLACK "\033[0;30m"
#define ANSI_FG_RED "\033[0;31m"
#define ANSI_FG_GREEN "\033[0;32m"
#define ANSI_FG_YELLOW "\033[0;33m"
#define ANSI_FG_BLUE "\033[0;34m"
#define ANSI_FG_MAGENTA "\033[35;40m"
#define ANSI_FG_CYAN "\033[0;36m"
#define ANSI_FG_WHITE "\033[0;37m"

#define ANSI_FGB_BLACK "\033[1;30m"
#define ANSI_FGB_RED "\033[1;31m"
#define ANSI_FGB_GREEN "\033[1;32m"
#define ANSI_FGB_YELLOW "\033[1;33m"
#define ANSI_FGB_BLUE "\033[1;34m"
#define ANSI_FGB_MAGENTA "\033[1;35m"
#define ANSI_FGB_CYAN "\033[1;36m"
#define ANSI_FGB_WHITE "\033[1;37m"

#define ANSI_BG_BLACK "\033[40m"
#define ANSI_BG_RED "\033[41m"
#define ANSI_BG_GREEN "\033[42m"
#define ANSI_BG_YELLOW "\033[43m"
#define ANSI_BG_BLUE "\033[44m"
#define ANSI_BG_MAGENTA "\033[45m"
#define ANSI_BG_CYAN "\033[46m"
#define ANSI_BG_WHITE "\033[47m"
