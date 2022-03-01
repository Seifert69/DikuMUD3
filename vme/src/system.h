/*
 $Author: All $
 $RCSfile: system.h,v $
 $Date: 2004/03/20 06:13:22 $
 $Revision: 2.3 $
 */
#pragma once

#include "essential.h"
#include "structs.h"

#define MAX_HOSTNAME 256
#define SNOOP_PROMPT "snoop&gt; " /* probably not very nice to have here, but hey! */

void descriptor_close(descriptor_data *d, int bSendClose = TRUE, int bReconnect = FALSE);
void MplexSendSetup(descriptor_data *d);
descriptor_data *descriptor_new(cMultiHook *pe);
void system_memory(unit_data *ch);
void init_char(unit_data *ch);

extern int g_no_connections;
extern int g_max_no_connections;
