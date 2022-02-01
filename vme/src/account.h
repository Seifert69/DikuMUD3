/*
 $Author: tperry $
 $RCSfile: account.h,v $
 $Date: 2001/04/29 03:46:05 $
 $Revision: 2.1 $
 */
#pragma once

#include "essential.h"
#include "interpreter.h"
#include "structs.h"

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
    CAccountConfig();

    void Boot();

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

    flatrate_type m_flatrate[MAX_FLATRATE];
};

void account_cclog(unit_data *ch, int amount);
void account_closed(unit_data *ch);
void account_defaults(unit_data *pc);
void account_flatrate_change(unit_data *god, unit_data *whom, sbit32 days);
void account_global_stat(unit_data *ch);
void account_insert(unit_data *god, unit_data *whom, ubit32 amount);
int account_is_closed(unit_data *ch);
int account_is_overdue(const unit_data *ch);
void account_local_stat(const unit_data *ch, unit_data *u);
void account_overdue(const unit_data *ch);
void account_paypoint(unit_data *ch);
void account_subtract(unit_data *pc, time_t from, time_t to);
void account_withdraw(unit_data *god, unit_data *whom, ubit32 amount);
void do_account(unit_data *, char *, const command_info *);

extern CAccountConfig g_cAccountConfig;
