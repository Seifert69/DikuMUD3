/*
 $Author: All $
 $RCSfile: extra.h,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.3 $
 */
#pragma once

#include "essential.h"
#include "intlist.h"
#include "namelist.h"

#include <string>

class extra_descr_data;
class unit_data;

class extra_list
{
public:
    extra_list();
    ~extra_list();

    void AppendBuffer(CByteBuffer *pBuf);
    extra_descr_data *find_raw(const char *word);
    void erase(extra_descr_data *exd);
    void remove(const char *name);
    void copyfrom(extra_list &listToBeCopied);
    int isempty();
    int count();
    std::string json();

    void push_front(extra_descr_data *ex);
    void push_tail(extra_descr_data *ex);

    extra_descr_data *add(extra_descr_data *ex);
    extra_descr_data *add(const char *name, const char *descr);
    extra_descr_data *add(const char **names, const char *descr);
    extra_descr_data *add(cNamelist cn, const char *descr);

    // I couldn't use std:list because dilfld needs the next pointer
    // and I can't get to the container only one pointer in DIL.
    extra_descr_data *m_pList;

private:
    void freelist(extra_descr_data *);
};

class extra_descr_data
{
public:
    extra_descr_data();
    extra_descr_data(const char *name, const char *descr);
    extra_descr_data(const char **names, const char *descr);
    extra_descr_data(cNamelist names, const char *descr);
    ~extra_descr_data();

    extra_descr_data *find_raw(const char *word);

    cintlist vals;
    cNamelist names;        /* Keyword in look/examine          */
    std::string descr;      /* What to see                      */
    extra_descr_data *next; /* Next in list                     */
};

const char *unit_find_extra_string(unit_data *ch, char *word, unit_data *list);

extra_descr_data *char_unit_find_extra(unit_data *ch, unit_data **target, char *word, unit_data *list);

extra_descr_data *unit_find_extra(const char *word, unit_data *unit);
void rogue_push_front(extra_descr_data **exlist, extra_descr_data *newex);
void rogue_remove(extra_descr_data **exlist, const char *name);
