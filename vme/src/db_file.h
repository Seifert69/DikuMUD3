/*
 $Author: All $
 $RCSfile: db_file.h,v $
 $Date: 2004/09/21 08:45:46 $
 $Revision: 2.4 $
 */

#ifndef _MUD_DB_FILE_H
#define _MUD_DB_FILE_H

#include "structs.h"
#include "bytestring.h"
#include "dil.h"

extern CByteBuffer g_FileBuffer; /* Defined in db_file.c */
extern int g_nCorrupt;           /*          "           */

void *bread_dil(CByteBuffer *pBuf, class unit_data *, ubit8 version,
                class unit_fptr *fptr, int stspec = TRUE);

struct diltemplate *bread_diltemplate(CByteBuffer *pBuf, int version);
int bread_extra(CByteBuffer *pBuf, class extra_list &cExtra, int unit_version);

int bread_affect(CByteBuffer *pBuf, class unit_data *u, ubit8 nVersion);

class unit_fptr *bread_func(CByteBuffer *pBuf, ubit8 version,
                             class unit_data *owner, int stspec = TRUE);

void bread_block(FILE *datafile, long file_pos, int length, void *buffer);

void bwrite_affect(CByteBuffer *pBuf, class unit_affected_type *af,
                   ubit8 version);
void bwrite_func(CByteBuffer *pBuf, class unit_fptr *fptr);
void bwrite_block(FILE *f, int length, void *buffer);
void bwrite_dil(CByteBuffer *pBuf, class dilprg *prg);
void bwrite_diltemplate(CByteBuffer *pBuf, struct diltemplate *tmpl);

void write_unit(FILE *f, class unit_data *u, char *fname);
void write_diltemplate(FILE *f, struct diltemplate *tmpl);
int write_unit_string(CByteBuffer *pBuf, class unit_data *u);

#endif /* _MUD_DB_FILE_H */
