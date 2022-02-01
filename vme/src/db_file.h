/*
 $Author: All $
 $RCSfile: db_file.h,v $
 $Date: 2004/09/21 08:45:46 $
 $Revision: 2.4 $
 */
#pragma once

#include "bytestring.h"
#include "dil.h"
#include "structs.h"

void *bread_dil(CByteBuffer *pBuf, unit_data *, ubit8 version, unit_fptr *fptr, int stspec = TRUE);

diltemplate *bread_diltemplate(CByteBuffer *pBuf, int version);
int bread_extra(CByteBuffer *pBuf, extra_list &cExtra, int unit_version);

unit_fptr *bread_func(CByteBuffer *pBuf, ubit8 version, unit_data *owner, int stspec = TRUE);

void bread_block(FILE *datafile, long file_pos, int length, void *buffer);

void bwrite_affect(CByteBuffer *pBuf, unit_affected_type *af, ubit8 version);
void bwrite_func(CByteBuffer *pBuf, unit_fptr *fptr);
void bwrite_block(FILE *f, int length, void *buffer);
void bwrite_dil(CByteBuffer *pBuf, dilprg *prg);
void bwrite_diltemplate(CByteBuffer *pBuf, diltemplate *tmpl);

void write_unit(FILE *f, unit_data *u, char *fname);
void write_diltemplate(FILE *f, diltemplate *tmpl);
int write_unit_string(CByteBuffer *pBuf, unit_data *u);

extern CByteBuffer g_FileBuffer; /* Defined in db_file.c */
extern int g_nCorrupt;           /*          "           */
