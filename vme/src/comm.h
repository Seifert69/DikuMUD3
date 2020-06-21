/*
 $Author: All $
 $RCSfile: comm.h,v $
 $Date: 2004/03/20 06:13:21 $
 $Revision: 2.3 $
 */

#ifndef _MUD_COMM_H
#define _MUD_COMM_H

void page_string (class descriptor_data *d, const char *);

void send_to_outdoor (const char *messg);
void send_to_zone_outdoor (const class zone_type *z, const char *messg);
void send_to_descriptor (const char *messg, class descriptor_data *d);
void send_to_all (const char *messg);
void send_to_char (const char *messg, const class unit_data * ch);

/*  Please note that act() does NOT accept TRUE or FALSE as second argument
 *  anymore...
 */
void act (const char *str, int hide_invisible, const void *arg1,
          const void *arg2, const void *arg3, int type);
void sact(char *buf, const char *str, int show_type,
          const void *arg1, const void *arg2, const void *arg3, int type);
void cact (const char *str, int hide_invisible, const void *arg1,
           const void *arg2, const void *arg3, int type, const char *colortype);
void act_generate (char *buf, const char *str, int show_type,
                   const void *arg1, const void *arg2, const void *arg3,
                   int type, class unit_data * to, int bNewline = 1);

int process_output (class descriptor_data *t);
int process_input (class descriptor_data *t);

#define PROMPT_SENT   0
#define PROMPT_EXPECT 1
#define PROMPT_IGNORE 2

#endif /* _MUD_COMM_H */
