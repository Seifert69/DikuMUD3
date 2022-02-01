/*
 $Author: All $
 $RCSfile: files.h,v $
 $Date: 2003/11/29 03:31:16 $
 $Revision: 2.2 $
 */
#pragma once

#include "bytestring.h"

#include <string>

#define FCACHE_MAX 100

int store_name_test(char *name);
void fstrcpy(CByteBuffer *pBuf, FILE *f);
char *fread_line_commented(FILE *fl, char *buf, int max);
long fsize(FILE *f);
void touch_file(const char *name);
void touch_file(const std::string &name);
char *fread_string(FILE *fl);
char *fread_string_copy(FILE *fl, char *buf, int max);
int file_to_string(const char *name, char *buf, int max_len);
int file_to_string(const std::string &name, char *buf, int max_len);
int config_file_to_string(const char *name, char *buf, int max_len);
int config_file_to_string(const std::string &name, char *buf, int max_len);
FILE *fopen_cache(const char *name, const char *mode);
FILE *fopen_cache(const std::string &name, const char *mode);
void fclose_cache();
ubit1 file_exists(const char *name);
ubit1 file_exists(const std::string &name);
int load_string(char *filename, char **file_str);
int save_string(char *filename, char **file_str, char *opp);
