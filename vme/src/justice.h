/*
 $Author: tperry $
 $RCSfile: justice.h,v $
 $Date: 2001/04/29 03:46:06 $
 $Revision: 2.1 $
 */
#pragma once

#include "essential.h"
#include "interpreter.h"
#include "structs.h"

#define CRIME_LIFE 12   /* Remember crime for (5 mins * CRIME_LIFE)    */
#define CRIME_NONPRO 8  /* Person will be non-protected                */
#define CRIME_OUTLAW 32 /* Person is wanted                            */
#define CRIME_REWARD 64 /* Person will be wanted dead for reward.      */

void offend_legal_state(unit_data *ch, unit_data *victim);
void log_crime(unit_data *criminal, unit_data *victim, ubit8 crime_type, int active = TRUE);
int new_crime_serial_no();
int reward_give(spec_arg *sarg);
