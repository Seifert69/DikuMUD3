/*
 $Author: All $
 $RCSfile: translate.h,v $
 $Date: 2005/06/28 20:37:10 $
 $Revision: 2.1 $
 */
#ifndef _MUD_TRANSLATE_H
#define _MUD_TRANSLATE_H

void protocol_translate(class cConHook *con, ubit8 code, char **b);
void translate_init(void);

void Control_Echo_Off(class cConHook *con, char **b, ubit8 code);
void Control_Echo_On(class cConHook *con, char **b, ubit8 code);
void Control_ANSI_Bold(class cConHook *con, char **b, ubit8 code);

void Control_ANSI_Bg(class cConHook *con, char **b, ubit8 code);
void Control_ANSI_Fg(class cConHook *con, char **b, ubit8 code, int bBold);

void Control_ANSI_Fg_Black(class cConHook *con, char **b, ubit8 code);
void Control_ANSI_Fg_Red(class cConHook *con, char **b, ubit8 code);
void Control_ANSI_Fg_Green(class cConHook *con, char **b, ubit8 code);
void Control_ANSI_Fg_Yellow(class cConHook *con, char **b, ubit8 code);
void Control_ANSI_Fg_Blue(class cConHook *con, char **b, ubit8 code);
void Control_ANSI_Fg_Magenta(class cConHook *con, char **b, ubit8 code);
void Control_ANSI_Fg_Cyan(class cConHook *con, char **b, ubit8 code);
void Control_ANSI_Fg_White(class cConHook *con, char **b, ubit8 code);
void Control_ANSI_Fgb_Black(class cConHook *con, char **b, ubit8 code);
void Control_ANSI_Fgb_Red(class cConHook *con, char **b, ubit8 code);
void Control_ANSI_Fgb_Green(class cConHook *con, char **b, ubit8 code);
void Control_ANSI_Fgb_Yellow(class cConHook *con, char **b, ubit8 code);
void Control_ANSI_Fgb_Blue(class cConHook *con, char **b, ubit8 code);
void Control_ANSI_Fgb_Magenta(class cConHook *con, char **b, ubit8 code);
void Control_ANSI_Fgb_Cyan(class cConHook *con, char **b, ubit8 code);
void Control_ANSI_Fgb_White(class cConHook *con, char **b, ubit8 code);
void Control_ANSI_Bg_Black(class cConHook *con, char **b, ubit8 code);
void Control_ANSI_Bg_Red(class cConHook *con, char **b, ubit8 code);
void Control_ANSI_Bg_Green(class cConHook *con, char **b, ubit8 code);
void Control_ANSI_Bg_Yellow(class cConHook *con, char **b, ubit8 code);
void Control_ANSI_Bg_Blue(class cConHook *con, char **b, ubit8 code);
void Control_ANSI_Bg_Magenta(class cConHook *con, char **b, ubit8 code);
void Control_ANSI_Bg_Cyan(class cConHook *con, char **b, ubit8 code);
void Control_ANSI_Bg_White(class cConHook *con, char **b, ubit8 code);
void Control_ANSI_Echo_Off(class cConHook *con, char **b, ubit8 code);
/* Use these Super Colour defines, so that we can easilly change the
   default colours */

#define COLOUR_MENU CONTROL_FGB_WHITE
#define COLOUR_NORMAL CONTROL_FG_WHITE
#define COLOUR_COMM CONTROL_FGB_WHITE
#define COLOUR_MOB CONTROL_FG_CYAN
#define COLOUR_ATTN CONTROL_FGB_RED

#define CONTROL_CHAR '\x1B'

#define CONTROL_ECHO_OFF_CHAR 'A'
#define CONTROL_ECHO_ON_CHAR 'B'
#define CONTROL_BOLD_CHAR 'C'
#define CONTROL_REVERSE_CHAR 'D'
#define CONTROL_RESET_CHAR 'E'
#define CONTROL_DIM_CHAR 'F'
#define CONTROL_HOME_CHAR 'H'
#define CONTROL_GOBBLE_ON_CHAR 'I'
#define CONTROL_GOBBLE_OFF_CHAR 'J'
#define CONTROL_COLOR_CHANGE_CHAR 'K'
#define CONTROL_COLOR_INSERT_CHAR 'L'
#define CONTROL_COLOR_CREATE_CHAR 'M'
#define CONTROL_COLOR_REMOVE_CHAR 'N'
#define CONTROL_COLOR_DISP_CHAR 'O'
#define CONTROL_COLOR_END_CHAR 'P'

/* Must run in sequence +1 from fg black */

#define CONTROL_FG_BLACK_CHAR 'a'
#define CONTROL_FG_RED_CHAR 'b'
#define CONTROL_FG_GREEN_CHAR 'c'
#define CONTROL_FG_YELLOW_CHAR 'd'
#define CONTROL_FG_BLUE_CHAR 'e'
#define CONTROL_FG_MAGENTA_CHAR 'f'
#define CONTROL_FG_CYAN_CHAR 'g'
#define CONTROL_FG_WHITE_CHAR 'h'

#define CONTROL_FGB_BLACK_CHAR 'i'
#define CONTROL_FGB_RED_CHAR 'j'
#define CONTROL_FGB_GREEN_CHAR 'k'
#define CONTROL_FGB_YELLOW_CHAR 'l'
#define CONTROL_FGB_BLUE_CHAR 'm'
#define CONTROL_FGB_MAGENTA_CHAR 'n'
#define CONTROL_FGB_CYAN_CHAR 'o'
#define CONTROL_FGB_WHITE_CHAR 'p'

#define CONTROL_BG_BLACK_CHAR 'q'
#define CONTROL_BG_RED_CHAR 'r'
#define CONTROL_BG_GREEN_CHAR 's'
#define CONTROL_BG_YELLOW_CHAR 't'
#define CONTROL_BG_BLUE_CHAR 'u'
#define CONTROL_BG_MAGENTA_CHAR 'v'
#define CONTROL_BG_CYAN_CHAR 'w'
#define CONTROL_BG_WHITE_CHAR 'x'

#define CONTROL_ECHO_OFF "\x1B" \
                         "A"
#define CONTROL_ECHO_ON "\x1B" \
                        "B"

#define CONTROL_BOLD "\x1B" \
                     "C"
#define CONTROL_sREVERSE "\x1B" \
                         "D"

#define CONTROL_RESET "\x1B" \
                      "E"
#define CONTROL_DIM "\x1B" \
                    "F"
#define CONTROL_HOME "\x1B" \
                     "H"
#define CONTROL_GOBBLE_ON "\x1B" \
                          "I"
#define CONTROL_GOBBLE_OFF "\x1B" \
                           "J"
#define CONTROL_COLOR_CHANGE "\x1B" \
                             "K"
#define CONTROL_COLOR_INSERT "\x1B" \
                             "L"
#define CONTROL_COLOR_CREATE "\x1B" \
                             "M"
#define CONTROL_COLOR_REMOVE "\x1B" \
                             "N"
#define CONTROL_COLOR_DISP "\x1B" \
                           "O"
#define CONTROL_COLOR_END "\x1B" \
                          "P"

#define CONTROL_FG_BLACK "\x1B" \
                         "a"
#define CONTROL_FG_RED "\x1B" \
                       "b"
#define CONTROL_FG_GREEN "\x1B" \
                         "c"
#define CONTROL_FG_YELLOW "\x1B" \
                          "d"
#define CONTROL_FG_BLUE "\x1B" \
                        "e"
#define CONTROL_FG_MAGENTA "\x1B" \
                           "f"
#define CONTROL_FG_CYAN "\x1B" \
                        "g"
#define CONTROL_FG_WHITE "\x1B" \
                         "h"

#define CONTROL_FGB_BLACK "\x1B" \
                          "i"
#define CONTROL_FGB_RED "\x1B" \
                        "j"
#define CONTROL_FGB_GREEN "\x1B" \
                          "k"
#define CONTROL_FGB_YELLOW "\x1B" \
                           "l"
#define CONTROL_FGB_BLUE "\x1B" \
                         "m"
#define CONTROL_FGB_MAGENTA "\x1B" \
                            "n"
#define CONTROL_FGB_CYAN "\x1B" \
                         "o"
#define CONTROL_FGB_WHITE "\x1B" \
                          "p"

#define CONTROL_BG_BLACK "\x1B" \
                         "q"
#define CONTROL_BG_RED "\x1B" \
                       "r"
#define CONTROL_BG_GREEN "\x1B" \
                         "s"
#define CONTROL_BG_YELLOW "\x1B" \
                          "t"
#define CONTROL_BG_BLUE "\x1B" \
                        "u"
#define CONTROL_BG_MAGENTA "\x1B" \
                           "v"
#define CONTROL_BG_CYAN "\x1B" \
                        "w"
#define CONTROL_BG_WHITE "\x1B" \
                         "x"

#endif
