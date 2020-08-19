/*
 $Author: All $
 $RCSfile: system.h,v $
 $Date: 2004/03/20 06:13:22 $
 $Revision: 2.3 $
 */

#ifndef _MUD_SYSTEM_H
#define _MUD_SYSTEM_H
#ifdef _WINDOWS
#include <time.h>
#else
#include <sys/time.h>
#endif

#define MAX_HOSTNAME 256

void descriptor_close(class descriptor_data *d, int bSendClose = TRUE, int bReconnect = FALSE);
void MplexSendSetup(class descriptor_data *d);

int any_event(int port, struct multi_type *m);
int multi_any_connect(int port); /* test for event on socket port */
int multi_any_freaky(struct multi_type *m);
int multi_new(int mother, struct multi_type *m);

int any_input(int fd);  /* test for input         */
int any_output(int fd); /* test for output        */

int multi_process_input(struct multi_element *pm);

int write_to_descriptor(int desc, char *txt);
int read_from_descriptor(int desc, char *txt);
class descriptor_data *descriptor_new(class cMultiHook *pe);

#define SNOOP_PROMPT "snoop&gt; " /* probably not very nice to have here, but hey! */

#endif /* _MUD_SYSTEM_H */
