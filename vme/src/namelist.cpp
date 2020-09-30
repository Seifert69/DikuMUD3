/*
 $Author: All $
 $RCSfile: namelist.cpp,v $
 $Date: 2004/09/18 19:52:56 $
 $Revision: 2.6 $
 */

#include <ctype.h>
#include <string.h>

#include "namelist.h"
#include "essential.h"
#include "textutil.h"
#include "db_file.h"

cNamelist::cNamelist (void)
{
    namelist = NULL;
    length = 0;
}


void cNamelist::CopyList (const char **list)
{
    if (list)
        for (int i = 0; list[i]; i++)
            AppendName(list[i]);
}

void cNamelist::operator = (const char **list) {
    if (list)
        for (int i = 0; list[i]; i++)
            AppendName(list[i]);
}

void
cNamelist::operator = (class cNamelist *cn) {
    for (ubit32 i = 0; i < cn->Length (); i++)
        AppendName(cn->Name(i));
}

void cNamelist::CopyList (class cNamelist *cn)
{
    for (ubit32 i = 0; i < cn->Length (); i++)
        AppendName (cn->Name (i));
}


// Serialize the namelist object into a byte buffer
void cNamelist::AppendBuffer (CByteBuffer * pBuf)
{
    pBuf->Append32(Length());
    for (ubit32 i = 0; i < Length(); i++)
        pBuf->AppendString(Name (i));
}

// De-serialize the namelist from a byte buffer
int cNamelist::ReadBuffer (CByteBuffer * pBuf, int unit_version)
{
    char *c;

    if (unit_version < 65)
    {
        for (;;)
        {
            if (pBuf->SkipString (&c))
                return 1;

            if (*c)
                AppendName (c);
            else
                break;
        }

        return 0;
    }
    else
    {
        ubit32 len, i;
        if (pBuf->Read32 (&len))
            return 1;

        for (i = 0; i < len; i++)
        {
            pBuf->SkipString(&c);
            AppendName(c);
        }
        return 0;
    }

}


void cNamelist::bread (ubit8 ** b)
{
    int l;
    char *c;
    l=bread_ubit32(b);

    for (int i=0; i<l; i++)
    {
        c = bread_str_skip (b);
        AppendName (c);
    }
}


void cNamelist::bwrite(ubit8 ** b)
{
    bwrite_ubit32(b, Length());

    for (ubit32 i=0; i<Length(); i++)
        bwrite_string (b, Name(i));
}


char *cNamelist::catnames()
{
    ubit32 i;
    char *s;
    ubit32 strsize;
    strsize = (Length () * 3) + 3;
    for (i = 0; i < Length (); i++)
        strsize = strsize + strlen (Name (i));
    CREATE (s, char, strsize);
    strcpy (s, "{");
    if (Length () > 0)
    {
        for (i = 0; i < Length (); i++)
        {
            strcat (s, "\"");
            strcat (s, Name (i));
            strcat (s, "\"");
            if (i < Length () - 1)
                strcat (s, ",");
        }
    }
    strcat (s, "}");
    return s;
}

std::string cNamelist::json(void)
{
    string s;

    s = "\"namelist\": [";

    for (ubit32 i = 0; i < Length(); i++)
    {
        s.append(str_json_encode_quote(Name(i)));
        if (i < Length () - 1)
            s.append(",");
    }

    s.append("]");

    return s;
}

void cNamelist::Remove(ubit32 idx)
{
    if (length > idx)
    {
        delete namelist[idx];
        if (idx != length - 1)
            memmove (&namelist[idx], &namelist[idx + 1],
                     sizeof(string *) * (length - idx - 1));
        length--;
        if (length == 0)
        {
            Free();
        }
        else
        {
            RECREATE(namelist, string *, length);
        }
    }
}

void cNamelist::RemoveName (const char *name)
{
    register ubit32 i, j;

    for (i = 0; i < length; i++)
    {
        for (j = 0; namelist[i]->c_str ()[j]; j++)
            if (tolower (name[j]) != tolower (namelist[i]->c_str ()[j]))
                break;

        if (!namelist[i]->c_str ()[j])
            if (!name[j] || isaspace (name[j]))
            {
                Remove (i);
                return;
            }
    }
}

void cNamelist::Substitute (ubit32 idx, const char *newname)
{
    if (length > idx)
    {
        delete namelist[idx];
        namelist[idx] = new string (newname?newname:"");
    }
}

cNamelist::cNamelist (const char **list)
{
    namelist = NULL;
    length = 0;

    CopyList (list);
}


void cNamelist::Free (void)
{
    ubit32 i;

    for (i = 0; i < length; i++)
    {
        delete namelist[i];
        namelist[i] = NULL;
    }

    if (namelist)
        FREE (namelist);

    namelist = NULL;
    length = 0;
}


cNamelist::~cNamelist (void)
{
    Free ();
}


cNamelist *cNamelist::Duplicate(void)
{
    cNamelist *pNl = new cNamelist;
    ubit32 i = 0;

    for (i = 0; i < length; i++)
        pNl->AppendName ((char *) namelist[i]->c_str ());

    return pNl;
}


const char *cNamelist::IsNameRaw (const char *name)
{
    register ubit32 i, j;

    for (i = 0; i < length; i++)
    {
        if (namelist)
        {
            for (j = 0; namelist[i]->c_str ()[j]; j++)
                /*fuck look at thes isnameraw functions */
                if (tolower (name[j]) != tolower (namelist[i]->c_str ()[j]))
                    break;

            if (!namelist[i]->c_str ()[j])
                if (!name[j] || isaspace (name[j]))
                    return ((char *) name) + j;
        }
    }

    return NULL;
}


const char *cNamelist::IsName (const char *name)
{
    char buf[MAX_STRING_LENGTH];

    name = skip_spaces (name);

    if (!name)
        strcpy (buf, "");
    else
    {
        strcpy (buf, name);
        str_remspc (buf);
    }
    return IsNameRaw (buf);
}


//see if name is in the name list names some where and return name if it is
const char *cNamelist::StrStrRaw (const char *name)
{
    register ubit32 i;

    for (i = 0; i < length; i++)
    {
        if (namelist)
        {
            if (str_cstr(namelist[i]->c_str(),name))
                return name;
        }
    }

    return NULL;
}


// Clean up name and see if it exists in the namelist. If so return name (uncleaned)
const char *cNamelist::StrStr(const char *name)
{
    char buf[MAX_STRING_LENGTH];

    name = skip_spaces(name);

    if (!name)
        strcpy (buf, "");
    else
    {
        strcpy (buf, name);
        str_remspc (buf);
    }

    if (StrStrRaw(buf))
        return name;
    else
        return NULL;
}


/* Returns -1 if no name matches, or 0.. for the index in the namelist */
const int cNamelist::IsNameRawIdx (const char *name)
{
    register ubit32 i, j;

    for (i = 0; i < length; i++)
    {
        for (j = 0; namelist[i]->c_str ()[j]; j++)
            if (tolower (name[j]) != tolower (namelist[i]->c_str ()[j]))
                break;

        if (!namelist[i]->c_str ()[j])
            if (!name[j] || isaspace (name[j]))
                return i;
    }

    return -1;
}

/* As IsNameRawIdx */
const int cNamelist::IsNameIdx (const char *name)
{

    if (name)
        return IsNameRawIdx (name);
    else
        return IsNameRawIdx ("");
}


const char *cNamelist::Name(ubit32 idx)
{
    const char *test;

    if (!namelist)
        return NULL;

    if (idx < length)
    {
        test = namelist[idx]->c_str ();
    }
    else
        test = NULL;
    return (test);
}


string *cNamelist::InstanceName (ubit32 idx)
{
    if (idx < length)
        return namelist[idx];

    return NULL;
}


void cNamelist::dAppendName (const char *name)
{
    length++;

    if (name) assert((*name == 0) || (*name != 0)); // Ensure valid string (debug)

    if (namelist == NULL)
    {
        CREATE (namelist, string *, length);
    }
    else
    {
        RECREATE (namelist, string *, length);
    }
    namelist[length - 1] = new string (name?name:"");
}


void cNamelist::AppendName (const char *name)
{
    length++;

    if (name) assert((*name == 0) || (*name != 0)); // Ensure valid string (debug)

    if (namelist == NULL)
    {
        CREATE (namelist, string *, length);
    }
    else
    {
        RECREATE (namelist, string *, length);
    }
    namelist[length - 1] = new string (name?name:"");

}


void cNamelist::PrependName (const char *name)
{
    length++;

    if (name) assert((*name == 0) || (*name != 0)); // Ensure valid string (debug)

    if (namelist == NULL)
    {
        CREATE (namelist, string *, length);
    }
    else
    {
        RECREATE (namelist, string *, length);
    }
    if (length > 1)
        memmove (&namelist[1], &namelist[0], sizeof (string *) * (length - 1));

    namelist[0] = new string (name?name:"");
}

void
cNamelist::InsertName (const char *name, ubit32 loc)
{
    ubit32 nadd, x, olen;
    olen = length;

    if (name) assert((*name == 0) || (*name != 0)); // Ensure valid string (debug)

    if (loc < length)
    {
        length++;
        nadd = 1;
    }
    else
    {
        nadd = loc + 1 - length;
        length = (loc + 1);
    }

    if (namelist == NULL)
    {
        CREATE (namelist, string *, length);
    }
    else
    {
        RECREATE (namelist, string *, length);
    }
    if (olen != loc)
    {
        if (length > 1)
            if (nadd == 1)
                memmove (&namelist[loc + 1], &namelist[loc],
                         sizeof (string *) * (olen - loc));
    }
    namelist[loc] = new string (name?name:"");
    if (nadd > 1)
        for (x = length - nadd; x < (length - 1); x++)
            namelist[x] = new string ("");
}
