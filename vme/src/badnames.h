/*
 $Author: All $
 $RCSfile: badnames.h,v $
 $Date: 2004/01/09 20:34:23 $
 $Revision: 2.2 $
 */
#pragma once

class badnames_list
{
private:
    char *name;
    badnames_list *next;

public:
    badnames_list();
    badnames_list(char *n);
    ~badnames_list();

    char *insert(char *n);
    char *in(char *n);
    char *equal(char *n);
    void create(char *input_str);
};
