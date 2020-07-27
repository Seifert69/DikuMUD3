/*
 $Author: All $
 $RCSfile: badnames.cpp,v $
 $Date: 2004/01/09 20:34:23 $
 $Revision: 2.2 $
 */
#include "badnames.h"
#include <ctype.h>
#include "structs.h"
#include "essential.h"
#include "string.h"
badnames_list::badnames_list (void)
{
    next = NULL;
    name = NULL;
}

badnames_list::badnames_list (char *n)
{
    int len = 0;
    next = NULL;
    len = strlen (n);
    //name = new char[len + 1];
    CREATE(name, char, len + 1);

    if (!name)
    {
        slog (LOG_ALL, 0, "Error in allocating name");
        assert (name);
    }
    strcpy (name, n);
}

badnames_list::~badnames_list (void)
{
    badnames_list *l = this->next;

    next = NULL;
    if (name)
    {
        FREE(name);
        name = NULL;
    }
    if (l)
        delete l;
}

char *badnames_list::insert (char *n)
{
    badnames_list *l = this->next, *b = this;
    badnames_list *temp;
    if (!n)
        return (NULL);
    temp = new badnames_list(n);

    if (!temp)
        return (NULL);

    while (l)
    {
        if (strcmp (l->name, n) >= 0)
            break;
        else
        {
            b = l;
            l = l->next;
        }
    }

    if (!l)
    {
        b->next = temp;
    }
    else if (strcmp (n, l->name) == 0)
    {
        temp->next = l->next;
        b->next = temp;
        l->next = NULL;
        delete l;
    }
    else
    {
        temp->next = b->next;
        b->next = temp;
    }
    return (name);
}

char *
badnames_list::equal (char *n)
{
    badnames_list *l = this->next;

    if (!n)
        return (NULL);
    while (l)
    {
        if (strcasecmp (n, l->name) == 0)
            break;
        l = l->next;
    }
    if (l)
        return (l->name);
    else
        return (NULL);
}


char *
badnames_list::in (char *n)
{
    char tname[PC_MAX_NAME];
    int i=0;
    tname[0]=0;

    badnames_list *l = this->next;

    if (!n)
        return (NULL);
    strcpy(tname,n);
    while (tname[i]!=0)
    {
        tname[i]=tolower(tname[i]);
        i++;
    }

    while (l)
    {
        if (strstr (tname, l->name) )
            break;
        l = l->next;
    }
    if (l)
        return (l->name);
    else
        return (NULL);
}

void
badnames_list::create (char *input_temp)
{
    char *n;

    if (!input_temp)
        return;
    n = strtok (input_temp, " ");
    if (!n)
        return;
    insert (n);

    while ((n = strtok (0, " ")) != NULL)
        insert (n);
}
