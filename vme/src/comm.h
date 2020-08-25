/*
 $Author: All $
 $RCSfile: comm.h,v $
 $Date: 2004/03/20 06:13:21 $
 $Revision: 2.3 $
 */

#ifndef _MUD_COMM_H
#define _MUD_COMM_H

#include "dil.h"

class cActParameter
{
public:
   cActParameter(void);
   cActParameter(const class unit_data *u);
   cActParameter(const int *i);
   cActParameter(const char *p);
   cActParameter(const dilval *v);
   int isNull(void);

   const class unit_data *m_u;
   const int *m_i;
   const char *m_p;
};

void page_string(class descriptor_data *d, const char *);

void send_to_outdoor(const char *messg);
void send_to_zone_outdoor(const class zone_type *z, const char *messg);
void send_to_descriptor(const char *messg, class descriptor_data *d);
void send_to_all(const char *messg);
void send_to_char(const char *messg, const class unit_data *ch);

/*  Please note that act() does NOT accept TRUE or FALSE as second argument
 *  anymore...
 */
void act(const char *str, int hide_invisible, cActParameter arg1,
         cActParameter arg2, cActParameter arg3, int type);
void sact(char *buf, const char *str, int show_type,
          cActParameter arg1, cActParameter arg2, cActParameter arg3, int type);
void cact(const char *str, int hide_invisible, cActParameter arg1,
          cActParameter arg2, cActParameter arg3, int type, const char *colortype);
void act_generate(char *buf, const char *str, int show_type,
                  cActParameter arg1, cActParameter arg2, cActParameter arg3,
                  int type, const class unit_data *to, int bNewline = 1);

int process_output(class descriptor_data *t);
int process_input(class descriptor_data *t);

#define PROMPT_SENT 0
#define PROMPT_EXPECT 1
#define PROMPT_IGNORE 2

#endif /* _MUD_COMM_H */
