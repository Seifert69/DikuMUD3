/*
 $Author: tperry $
 $RCSfile: str_parse.h,v $
 $Date: 2001/04/29 03:46:07 $
 $Revision: 2.1 $
 */
#pragma once

char *parse_match(const char *pData, const char *pMatch);
char *parse_name(char **pData);
int *parse_numlist(char **pData, int *count);
int parse_num(char **pData, int *pNum);
char **parse_namelist(char **pData);
int parse_match_num(const char **pData, const char *pMatch, int *pResult);
int *parse_match_numlist(const char **pData, const char *pMatch, int *count);
char *parse_match_name(const char **pData, const char *pMatch);
char **parse_match_namelist(const char **pData, const char *pMatch);
