/*
 $Author: All $
 $RCSfile: extra.h,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.3 $
 */

#ifndef _MUD_EXTRA_H
#define _MUD_EXTRA_H

#include "essential.h"
#include "namelist.h"
#include "intlist.h"
#include <string>

class extra_list
{
   public:
    extra_list(void);
    ~extra_list(void);

    void AppendBuffer(CByteBuffer *pBuf);
    class extra_descr_data *find_raw(const char *word);
    void erase(class extra_descr_data *exd);
    void remove(const char *name);
    void copyfrom(class extra_list &listToBeCopied);
    int isempty(void);
    int count(void);
    std::string json(void);

    void push_front(class extra_descr_data *ex);
    void push_tail(class extra_descr_data *ex);

    class extra_descr_data *add(class extra_descr_data *ex);
    class extra_descr_data *add(const char *name, const char *descr);
    class extra_descr_data *add(const char **names, const char *descr);
    class extra_descr_data *add(cNamelist cn, const char *descr);

    // I couldn't use std:list because dilfld needs the next pointer
    // and I can't get to the container only one pointer in DIL.
    class extra_descr_data *m_pList;

   private:
    void freelist(class extra_descr_data *);
};


class extra_descr_data
{
public:
    extra_descr_data (void);
    extra_descr_data(const char *name, const char *descr);
    extra_descr_data(const char **names, const char *descr);
    extra_descr_data(cNamelist names, const char *descr);
    ~extra_descr_data (void);

    class extra_descr_data *find_raw(const char *word);

    class cintlist vals;
    class cNamelist names;	/* Keyword in look/examine          */
    string descr;	/* What to see                      */
    class extra_descr_data *next;	/* Next in list                     */
};


const char *unit_find_extra_string (class unit_data * ch,
                                    char *word, class unit_data * list);

class extra_descr_data *char_unit_find_extra (class unit_data * ch,
            class unit_data ** target,
            char *word,
            class unit_data * list);

class extra_descr_data *unit_find_extra (const char *word,
            class unit_data * unit);
void rogue_push_front(class extra_descr_data **exlist, class extra_descr_data *newex);
void rogue_remove(class extra_descr_data **exlist, const char *name);


#endif /* _MUD_STRUCTS_H */
