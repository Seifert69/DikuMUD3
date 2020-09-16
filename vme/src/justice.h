/*
 $Author: tperry $
 $RCSfile: justice.h,v $
 $Date: 2001/04/29 03:46:06 $
 $Revision: 2.1 $
 */

#ifndef _MUD_JUSTICE_H
#define _MUD_JUSTICE_H

#define CRIME_LIFE   12		/* Remember crime for (5 mins * CRIME_LIFE)    */

#define CRIME_NONPRO  8		/* Person will be non-protected                */
#define CRIME_OUTLAW 32		/* Person is wanted                            */
#define CRIME_REWARD 64		/* Person will be wanted dead for reward.      */

/* STRUCTS ---------------------------------------------------------------*/

void offend_legal_state (class unit_data * ch, class unit_data * victim);

void log_crime (class unit_data * criminal, class unit_data * victim,
                ubit8 crime_type, int active = TRUE);
void call_guards(class unit_data *guard);
void set_reward_char (class unit_data * ch, int crimes);

#endif /* _MUD_JUSTICE_H */
