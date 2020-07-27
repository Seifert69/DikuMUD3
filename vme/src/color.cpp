/*
 $Author: All $
 $RCSfile: color.cpp,v $
 $Date: 2003/10/09 01:12:32 $
 $Revision: 2.3 $
 */
#include <assert.h>
#include <ctype.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "color.h"
#include "protocol.h"


color_type::color_type (void)
{
    next = NULL;
    color = NULL;
    keyword = NULL;
    key_size = 0;
    color_size = 0;
    count = 0;
}

color_type::color_type (char *key, char *c)
{
    int len = 0;
    next = NULL;
    len = strlen (c);
    //color = new char[len + 1];

    CREATE(color, char, len+1);
    if (!color)
    {
        slog (LOG_ALL, 0, "Error in allocating color");
        assert (color);
    }
    strcpy (color, c);

    len = strlen (key);
    //keyword = new char[len + 1];
    CREATE(keyword, char, len + 1);
    if (!keyword)
    {
        slog (LOG_ALL, 0, "Error in allocating keyword");
        assert (keyword);
    }
    strcpy (keyword, key);
}

color_type::~color_type (void)
{
    color_type *l = this->next;

    next = NULL;
    if (keyword)
        FREE(keyword);
    if (color)
        FREE(color);
    color = NULL;
    keyword = NULL;
    if (l)
        delete l;

}

void
color_type::count_plus (char *key, char *c)
{
    key_size += (strlen (key) + 1);
    color_size += (strlen (c) + 1);
    count++;
}

void
color_type::count_minus (char *key, char *c)
{
    key_size -= (strlen (key) + 1);
    color_size -= (strlen (c) + 1);
    count--;
}

char *
color_type::insert (char *key, char *c)
{
    color_type *l = this->next, *b = this;
    color_type *temp;
    if ((!key) || (!c))
        return (NULL);
    temp = new color_type(key, c);
    if (!temp)
        return (NULL);
    while (l)
    {
        if (strcmp (l->keyword, key) >= 0)
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
        count_plus (key, c);
    }
    else if (strcmp (key, l->keyword) == 0)
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
        count_plus (key, c);
    }
    char *c_return;
    //c_return = new char[strlen (key) + strlen (c) + 1];
    CREATE(c_return, char, strlen (key) + strlen (c) + 1);
    strcpy (c_return, c);
    strcat (c_return, key);
    return (c_return);
}

void
color_type::change (char *combo)
{

    char *key, *tok, *ret;

    key = strtok (combo, ":");
    if (!key)
    {
        return;
    }
    tok = strtok (0, ":");
    if (!tok)
    {
        return;
    }
    ret = change (key, tok);
    delete ret;

}

void
color_type::insert (char *combo)
{

    char *key, *tok, *ret;

    key = strtok (combo, ":");
    if (!key)
    {
        return;
    }
    tok = strtok (0, ":");
    if (!tok)
    {
        return;
    }
    ret = insert (key, tok);
    delete ret;

}

char *
color_type::change (char *key, char *c)
{
    color_type *l = this->next;
    char *temp = 0;
    //temp = new char[strlen (c) + 1];
    CREATE(temp, char, strlen (c) + 1);
    
    if (!temp)
    {
        slog (LOG_ALL, 0, "Error in allocating change color.");
        assert (temp);
    }

    while (l)
    {
        if (strncmp (l->keyword, key, strlen (key)) == 0)
        {
            color_size -= (strlen (l->color) + 1);
            delete (l->color);
            strcpy (temp, c);
            l->color = temp;
            color_size += (strlen (temp) + 1);
            char *c_return;
            //c_return = new char[strlen (l->keyword) + strlen (l->color) + 1];
            CREATE(c_return, char, strlen (l->keyword) + strlen (l->color) + 1);
            
            strcpy (c_return, l->color);
            strcat (c_return, l->keyword);
            return (c_return);
        }
        else
            l = l->next;
    }

    return (NULL);
}

char *
color_type::get (char *key)
{
    color_type *l = this->next;

    if (!key)
        return (NULL);
    while (l)
    {
        if (strncmp (key, l->keyword, strlen (key)) == 0)
            break;
        l = l->next;
    }

    if (l)
        return (l->color);
    else
        return (NULL);
}

char *
color_type::get (char *key, char *full_key)
{
    color_type *l = this->next;

    if (!key)
        return (NULL);
    while (l)
    {
        if (strncmp (key, l->keyword, strlen (key)) == 0)
            break;
        l = l->next;
    }

    if (l)
    {
        strcpy (full_key, l->keyword);
        return (l->color);
    }
    else
        return (NULL);
}

int
color_type::remove (char *key)
{
    color_type *temp = this, *l = this->next;
    if (!l)
    {
        return (0);
        //If blank list return nothing to remove
    }

    while (l)
    {
        if (strcmp (key, l->keyword) == 0)
            break;
        else
        {
            temp = l;
            l = temp->next;
        }
    }

    if (!l)
        return (0);

    temp->next = l->next;
    l->next = NULL;
    count_minus (l->keyword, l->color);
    delete l;
    return (1);
}

void color_type::remove_all ()
{
    color_type *l = this->next;

    next = NULL;

    if (keyword)
        delete keyword;
    if (color)
        delete color;
    color = NULL;
    keyword = NULL;
    if (l)
        delete l;

    key_size = 0;
    color_size = 0;
    count = 0;
}


void color_type::create (char *input_temp)
{
    char *delstr;
    char *tok, *key;
    char *input_str;

    if ((!input_temp) || (input_temp[0] == '\0'))
        return;


    //input_str = new char[strlen (input_temp) + 1];
    CREATE(input_str, char, strlen (input_temp) + 1);

    delstr = input_str;
    if (!input_str)
        return;

    while (*input_temp != '\0')
    {
        if ((*input_temp == '\r') || (*input_temp == ' '))
        {
            input_temp++;
            continue;
        }
        *input_str = *input_temp;
        input_str++;
        input_temp++;
    }

    *input_str = '\0';
    input_str = delstr;
    key = strtok (input_str, ":");

    if (!key)
    {
        FREE(delstr);
        //delete delstr;
        return;
    }
    tok = strtok (0, ":");
    if (!tok)
    {
        FREE(delstr);
        //delete delstr;
        return;
    }

    insert (key, tok);

    while ((key = strtok (0, ":")) != NULL)
    {

        tok = strtok (0, ":");
        if (!tok)
            break;

        insert (key, tok);
    }

    FREE(delstr);
    //delete delstr;
}

char *
color_type::key_string (void)
{
    long i = 0;
    int r = 0;
    i = (50 * count) + key_size + color_size;
    //char *buff = new char[i];
    char *buff;
    
    CREATE(buff, char, i);

    color_type *l = this->next;
    if (!buff)
    {
        slog (LOG_ALL, 0, "Error allocating key string for colors.");
        assert (buff);
    }

    buff[0] = '\0';
    if (!l)
    {
        return (NULL);
    }
    r = 0;

    while (l)
    {
        if (r < 3)
        {
            strcat (buff, l->color);
            strcat (buff, l->keyword);
            strcat (buff, getcolor ("default"));
            i = strlen (l->keyword);
            while (i < 25)
            {
                strcat (buff, " ");
                i++;
            }
            r++;
        }
        if (r == 3)
        {
            strcat (buff, "<br/>");
            r = 0;
        }
        l = l->next;
    }

    strcat (buff, "<br/>");
    return (buff);
}

char *
color_type::key_string (color_type & dft)
{
    char *t = this->save_string ();
    char *d = dft.save_string ();	//ptr to create string
    color_type temp;
    int i = 0;
    char *f;
    if (d)
        i += strlen (d) + 1;
    if (t)
        i += strlen (t) + 2;
    //f = new char[i];
    CREATE(f, char, i);

    if (!f)
    {
        slog (LOG_ALL, 0,
              "Problem with the 'final' pointer when doing save_string in key_string");
        assert (f);
    }
    f[0] = '\0';

    if (d)
    {
        strcat (f, d);
        delete d;
    }
    if (t)
    {
        strcat (f, ":");
        strcat (f, t);
        delete t;
    }

    temp.create (f);
    delete f;

    f = temp.key_string ();
    char *ftemp;
    //ftemp = new char[strlen (f) + 1];
    CREATE(ftemp, char, strlen (f) + 1);
    strcpy (ftemp, f);
    if (f)
        delete f;
    return (ftemp);
}


char *
color_type::save_string (void)
{

    color_type *l = this->next;

    if (!l)
    {
        return (NULL);
    }

    //char *buff = new char[key_size + color_size + (count * 2)];
    char *buff;
    CREATE(buff, char, key_size + color_size + (count * 2));

    if (!buff)
    {
        slog (LOG_ALL, 0, "Error allocating save string for colors.");
        assert (buff);
    }
    buff[0] = '\0';


    while (l)
    {
        strcat (buff, l->keyword);
        strcat (buff, ":");
        strcat (buff, l->color);
        strcat (buff, ":");
        l = l->next;
    }

    buff[(strlen (buff))] = '\0';
    return (buff);
}

char *getcolor (const char *colorstr)
{
    static char cstr[1024];
    sprintf (cstr, "<div class='%s'>", colorstr);
    return cstr;
}
