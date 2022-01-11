/*
 $Author: tperry $
 $RCSfile: account.h,v $
 $Date: 2001/04/29 03:46:05 $
 $Revision: 2.1 $
 */
#pragma once

#include "essential.h"

#define MAX_FLATRATE 2

struct flatrate_type
{
    char *pMessage;
    ubit16 days;
    ubit32 price;
};

class CAccountConfig
{
public:
    CAccountConfig(void);

    void Boot(void);

    char *m_pCoinName;
    char *m_pOverdueMessage;
    char *m_pClosedMessage;
    char *m_pPaypointMessage;

    int m_nMaxCharge;
    int m_nMinCharge;
    int m_nFreeFromLevel;
    int m_nAccountFree;
    int m_nAccountLimit;
    int m_nHourlyRate;
    int m_bCreditCard;

    struct flatrate_type m_flatrate[MAX_FLATRATE];
};

void account_cclog(class unit_data *ch, int amount);
void account_closed(class unit_data *ch);
void account_defaults(class unit_data *pc);
void account_flatrate_change(class unit_data *god, class unit_data *whom, sbit32 days);
void account_global_stat(class unit_data *ch);
void account_insert(class unit_data *god, class unit_data *whom, ubit32 amount);
int account_is_closed(class unit_data *ch);
int account_is_overdue(const class unit_data *ch);
void account_local_stat(const class unit_data *ch, class unit_data *u);
void account_overdue(const class unit_data *ch);
void account_paypoint(class unit_data *ch);
void account_subtract(class unit_data *pc, time_t from, time_t to);
void account_withdraw(class unit_data *god, class unit_data *whom, ubit32 amount);
void do_account(class unit_data *, char *, const struct command_info *);

extern class CAccountConfig g_cAccountConfig;
