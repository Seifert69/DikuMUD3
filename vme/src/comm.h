/*
 $Author: All $
 $RCSfile: comm.h,v $
 $Date: 2004/03/20 06:13:21 $
 $Revision: 2.3 $
 */
#pragma once

#include "dil.h"
#include "structs.h"

class cActParameter
{
public:
    cActParameter();
    cActParameter(const unit_data *u);
    cActParameter(const int *i);
    cActParameter(const char *p);
    cActParameter(const dilval *v);
    int isNull();

    const unit_data *m_u;
    const int *m_i;
    const char *m_p;
};

void page_string(descriptor_data *d, const char *);
void page_string(descriptor_data *d, const std::string &);

void send_to_outdoor(const char *messg);
void send_to_zone_outdoor(const zone_type *z, const char *messg);
void send_to_descriptor(const char *messg, descriptor_data *d);
void send_to_descriptor(const std::string &messg, descriptor_data *d);
void send_to_all(const char *messg);
void send_to_all(const std::string &messg);
void send_to_char(const char *messg, const unit_data *ch);
void send_to_char(const std::string &messg, const unit_data *ch);

/*  Please note that act() does NOT accept TRUE or FALSE as second argument
 *  anymore...
 */
void act(const char *str, int hide_invisible, cActParameter arg1, cActParameter arg2, cActParameter arg3, int type);
void sact(char *buf, const char *str, int show_type, cActParameter arg1, cActParameter arg2, cActParameter arg3, int type);
void cact(const char *str, int hide_invisible, cActParameter arg1, cActParameter arg2, cActParameter arg3, int type, const char *colortype);
void act_generate(char *buf,
                  const char *str,
                  int show_type,
                  cActParameter arg1,
                  cActParameter arg2,
                  cActParameter arg3,
                  int type,
                  const unit_data *to,
                  int bNewline = 1);

int process_output(descriptor_data *t);
int process_input(descriptor_data *t);

#define PROMPT_SENT 0
#define PROMPT_EXPECT 1
#define PROMPT_IGNORE 2
