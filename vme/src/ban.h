/*
 $Author: All $
 $RCSfile: ban.h,v $
 $Date: 2004/03/20 06:13:21 $
 $Revision: 2.2 $
 */

#ifndef _MUD_BAN_H
#define _MUD_BAN_H

char site_banned(char *cur_site);
void show_ban_text(char *site, class descriptor_data *d);

#define NO_BAN '0'
#define BAN_NEW_CHARS 'n' /* if(until == 0) ban is forever, otherwise      */
#define BAN_TOTAL 't'     /* it will be lifted when time(0) passes "until" */

const std::string BAN_FILE{"ban_list"};

#endif /* _MUD_BAN_H */
