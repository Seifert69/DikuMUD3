/*
 $Author: All $
 $RCSfile: namelist.cpp,v $
 $Date: 2004/09/18 19:52:56 $
 $Revision: 2.6 $
 */

#include "namelist.h"

#include "db_file.h"
#include "essential.h"
#include "slog.h"
#include "textutil.h"

#include <cctype>
#include <cstring>

cNamelist::cNamelist()
{
    namelist = nullptr;
    length = 0;
}

void cNamelist::CopyList(const char **list)
{
    if (list)
    {
        for (int i = 0; list[i]; i++)
        {
            AppendName(list[i]);
        }
    }
}

void cNamelist::operator=(const char **list)
{
    if (list)
    {
        for (int i = 0; list[i]; i++)
        {
            AppendName(list[i]);
        }
    }
}

void cNamelist::operator=(cNamelist *cn)
{
    for (ubit32 i = 0; i < cn->Length(); i++)
    {
        AppendName(cn->Name(i));
    }
}

void cNamelist::CopyList(cNamelist *cn)
{
    for (ubit32 i = 0; i < cn->Length(); i++)
    {
        AppendName(cn->Name(i));
    }
}

// Serialize the namelist object into a byte buffer
void cNamelist::AppendBuffer(CByteBuffer *pBuf)
{
    pBuf->Append32(Length());
    for (ubit32 i = 0; i < Length(); i++)
    {
        pBuf->AppendString(Name(i));
    }
}

// De-serialize the namelist from a byte buffer
int cNamelist::ReadBuffer(CByteBuffer *pBuf, int unit_version)
{
    char *c = nullptr;

    if (unit_version < 65)
    {
        for (;;)
        {
            if (pBuf->SkipString(&c))
            {
                return 1;
            }

            if (*c)
            {
                AppendName(c);
            }
            else
            {
                break;
            }
        }

        return 0;
    }
    else
    {
        ubit32 len = 0;
        ubit32 i = 0;
        int corrupt = 0;

        len = pBuf->ReadU32(&corrupt);

        if (corrupt)
        {
            return 1;
        }

        for (i = 0; i < len; i++)
        {
            pBuf->SkipString(&c);
            AppendName(c);
        }
        return 0;
    }
}

void cNamelist::bread(ubit8 **b)
{
    int l = 0;
    char *c = nullptr;
    l = bread_ubit32(b);

    for (int i = 0; i < l; i++)
    {
        c = bread_str_skip(b);
        AppendName(c);
    }
}

void cNamelist::bwrite(ubit8 **b)
{
    bwrite_ubit32(b, Length());

    for (ubit32 i = 0; i < Length(); i++)
    {
        bwrite_string(b, Name(i));
    }
}

char *cNamelist::catnames()
{
    ubit32 i = 0;
    char *s = nullptr;
    ubit32 strsize = 0;
    strsize = (Length() * 3) + 3;
    for (i = 0; i < Length(); i++)
    {
        strsize = strsize + strlen(Name(i));
    }
    CREATE(s, char, strsize);
    strcpy(s, "{");
    if (Length() > 0)
    {
        for (i = 0; i < Length(); i++)
        {
            strcat(s, "\"");
            strcat(s, Name(i));
            strcat(s, "\"");
            if (i < Length() - 1)
            {
                strcat(s, ",");
            }
        }
    }
    strcat(s, "}");
    return s;
}

std::string cNamelist::json()
{
    std::string s;

    s = "\"namelist\": [";

    for (ubit32 i = 0; i < Length(); i++)
    {
        s.append(str_json_encode_quote(Name(i)));
        if (i < Length() - 1)
        {
            s.append(",");
        }
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
        {
            memmove(&namelist[idx], &namelist[idx + 1], sizeof(std::string *) * (length - idx - 1));
        }
        length--;
        if (length == 0)
        {
            Free();
        }
        else
        {
            RECREATE(namelist, std::string *, length);
        }
    }
}

void cNamelist::RemoveName(const char *name)
{
    ubit32 i = 0;
    ubit32 j = 0;

    for (i = 0; i < length; i++)
    {
        for (j = 0; namelist[i]->c_str()[j]; j++)
        {
            if (tolower(name[j]) != tolower(namelist[i]->c_str()[j]))
            {
                break;
            }
        }

        if (!namelist[i]->c_str()[j])
        {
            if (!name[j] || isaspace(name[j]))
            {
                Remove(i);
                return;
            }
        }
    }
}

void cNamelist::Substitute(ubit32 idx, const char *newname)
{
    if (length > idx)
    {
        delete namelist[idx];
        namelist[idx] = new std::string(newname ? newname : "");
    }
}

cNamelist::cNamelist(const char **list)
{
    namelist = nullptr;
    length = 0;

    CopyList(list);
}

void cNamelist::Free()
{
    ubit32 i = 0;

    for (i = 0; i < length; i++)
    {
        delete namelist[i];
        namelist[i] = nullptr;
    }

    if (namelist)
        FREE(namelist);

    namelist = nullptr;
    length = 0;
}

cNamelist::~cNamelist()
{
    Free();
}

cNamelist *cNamelist::Duplicate()
{
    cNamelist *pNl = new cNamelist;
    ubit32 i = 0;

    for (i = 0; i < length; i++)
    {
        pNl->AppendName(namelist[i]->c_str());
    }

    return pNl;
}

// Find a match for 'name' in this namelist.
// Will match full name only.
// Case insensitive.
// Then it will return a pointer to "Boar" because there was
// a match on the full word fallow.
//
// If name is nullptr function returns nullptr.
// If name is empty function returns nullptr (I believe).
const char *cNamelist::IsNameRaw(const char *name)
{
    return std::as_const(*this).IsNameRaw(name);
}

// Find a precise match for 'name' in this namelist.
// Will match exact name only.
// Case insensitive.
//
// If name is nullptr function returns nullptr.
// If name is empty "" function returns nullptr (I believe).
// If a match returns a pointer to the end of name for the match.
//
const char *cNamelist::IsNameRaw(const char *name) const
{
    if (name == nullptr)
        return nullptr;

    for (ubit32 i = 0; i < length; i++)
    {
        ubit32 j;

        for (j = 0; namelist[i]->c_str()[j]; j++)
        {
            if (tolower(name[j]) != tolower(namelist[i]->c_str()[j]))
            {
                break;
            }
        }

        if (namelist[i]->c_str()[j] == 0)
        {
            if (!name[j] || isaspace(name[j]))
            {
                // Skip trailing spaces
                while (name[j] == ' ')
                {
                    j++;
                }
                return name + j;
            }
        }
    }

    return nullptr;
}


// See IsNameRaw.
// Will match non-full words, so
//   {"fallow deer", "deer"}.IsNameRawAbbrev("de guard")
// will return " guard" because "de" matches the beginning of "deer"
//
const char *cNamelist::IsNameRawAbbrev(const char *name)
{
    return std::as_const(*this).IsNameRawAbbrev(name);
}

// See IsNameRaw.
// Will match non-full words, so
//   {"fallow deer", "deer"}.IsNameRawAbbrev("de guard")
// will return "guard" because "de" matches the beginning of "deer"
// Note it is raw so string must be pre-formatted.
//
const char *cNamelist::IsNameRawAbbrev(const char *name) const
{
    if (name == nullptr)
        return nullptr;

    for (ubit32 i = 0; i < length; i++)
    {
        ubit32 j;

        for (j = 0; name[j]; j++)
        {
            if (tolower(name[j]) != tolower(namelist[i]->c_str()[j]))
            {
                break;
            }
        }

        if ((j > 0) && ((name[j] == 0) || isspace(name[j])))
        {
            // Skip trailing spaces
            while (name[j] == ' ')
            {
                j++;
            }

            return name + j;
        }
    }

    return nullptr;
}

/* Returns -1 if no name matches, or 0.. for the index in the namelist */
const int cNamelist::IsNameRawIdx(const char *name)
{
    if (name == nullptr)
        return -1;

    for (ubit32 i = 0; i < length; i++)
    {
        ubit32 j;

        for (j = 0; namelist[i]->c_str()[j]; j++)
        {
            if (tolower(name[j]) != tolower(namelist[i]->c_str()[j]))
            {
                break;
            }
        }

        if (namelist[i]->c_str()[j] == 0)
        {
            if (!name[j] || isaspace(name[j]))
            {
                return i;
            }
        }
    }

    return -1;
}

// Finds "name" in this namelist.
// Will skip any prepending spaces, and will skip double spaces in name.
// If you want a precise string match only, use IsNameRaw()
// Retuns nullptr if no match.
// If a namelist entry has double spaces or initial spacing, a match will be impossible.
// Otherwise returns a pointer to name, in the location where the match ends.
// Will skip any spaces at the end of the returned pointer.
//
const char *cNamelist::IsName(const char *name)
{
    if (name == nullptr)
        return nullptr;

    while (*name == ' ')
        name++;

    for (ubit32 i = 0; i < length; i++)
    {
        int s = 0;
        ubit32 j;

        for (j = 0; namelist[i]->c_str()[j]; j++)
        {
            if (tolower(name[j+s]) != tolower(namelist[i]->c_str()[j]))
            {
                break;
            }

            while (name[j+s] == ' ' && name[j+s+1] == ' ')
            {
                s++;
            }
       }

        if (namelist[i]->c_str()[j] == 0)
        {
            if (!name[j+s] || isaspace(name[j+s]))
            {
                // Skip trailing spaces
                while (name[j+s] == ' ')
                {
                    j++;
                }
                return name + j +s;
            }
        }
    }

    return nullptr;
}

/* As IsName but for IsNameRawIdx */
const int cNamelist::IsNameIdx(const char *name)
{
    if (name == nullptr)
        return -1;

    while (*name == ' ')
        name++;

    for (ubit32 i = 0; i < length; i++)
    {
        ubit32 j;
        int s = 0;

        for (j = 0; namelist[i]->c_str()[j]; j++)
        {
            if (tolower(name[j+s]) != tolower(namelist[i]->c_str()[j]))
            {
                break;
            }

            while (name[j+s] == ' ' && name[j+s+1] == ' ')
            {
                s++;
            }
       }

        if (namelist[i]->c_str()[j] == 0)
        {
            if (!name[j+s] || isaspace(name[j+s]))
            {
                return i;
            }
        }
    }

    return -1;
}


// see if name is in the name list names some where and return name if it is
const char *cNamelist::StrStrRaw(const char *name)
{
    ubit32 i = 0;

    if (name == nullptr)
        return nullptr;

    for (i = 0; i < length; i++)
    {
        if (namelist)
        {
            if (str_cstr(namelist[i]->c_str(), name))
            {
                return name;
            }
        }
    }

    return nullptr;
}

// Clean up name and see if it exists in the namelist. If so return name (uncleaned)
const char *cNamelist::StrStr(const char *name)
{
    char buf[MAX_STRING_LENGTH];

    if (name == nullptr)
        return nullptr;

    name = skip_spaces(name);

    if (!name)
    {
        strcpy(buf, "");
    }
    else
    {
        strcpy(buf, name);
        str_remspc(buf);
    }

    if (StrStrRaw(buf))
    {
        return name;
    }
    else
    {
        return nullptr;
    }
}

const char *cNamelist::Name(ubit32 idx) const
{
    const char *test = nullptr;

    if (!namelist)
    {
        return nullptr;
    }

    if (idx < length)
    {
        test = namelist[idx]->c_str();
    }
    else
    {
        test = nullptr;
    }
    return (test);
}

std::string *cNamelist::InstanceName(ubit32 idx)
{
    if (idx < length)
    {
        return namelist[idx];
    }

    return nullptr;
}

// Trims preceeding blanks, trailing blanks, double spaces ' ' in the string
// Does not trim newlines, etc.
//
void cNamelist::AppendNameTrim(const char *name)
{
    length++;

    if (namelist == nullptr)
    {
        CREATE(namelist, std::string *, length);
    }
    else
    {
        RECREATE(namelist, std::string *, length);
    }

    const char *c = name;

    while (isspace(*c))
        c++;

    std::string *s = new std::string(c);

    str_remspc(s->data());
    strip_trailing_blanks(s->data());

    /*if (strcmp(s->c_str(), name) != 0)
    {
        slog(LOG_ALL, 0, "Trimmed AppendName string [%s]", name);
    }*/

    namelist[length - 1] = s;
}

void cNamelist::AppendName(const char *name)
{
    length++;

    if (name)
    {
        assert((*name == 0) || (*name != 0)); // Ensure valid string (debug)
    }

    if (namelist == nullptr)
    {
        CREATE(namelist, std::string *, length);
    }
    else
    {
        RECREATE(namelist, std::string *, length);
    }
    namelist[length - 1] = new std::string(name ? name : "");
}

void cNamelist::PrependName(const char *name)
{
    length++;

    if (name)
    {
        assert((*name == 0) || (*name != 0)); // Ensure valid string (debug)
    }

    if (namelist == nullptr)
    {
        CREATE(namelist, std::string *, length);
    }
    else
    {
        RECREATE(namelist, std::string *, length);
    }
    if (length > 1)
    {
        memmove(&namelist[1], &namelist[0], sizeof(std::string *) * (length - 1));
    }

    namelist[0] = new std::string(name ? name : "");
}

void cNamelist::InsertName(const char *name, ubit32 loc)
{
    ubit32 nadd = 0;
    ubit32 x = 0;
    ubit32 olen = 0;
    olen = length;

    if (name)
    {
        assert((*name == 0) || (*name != 0)); // Ensure valid string (debug)
    }

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

    if (namelist == nullptr)
    {
        CREATE(namelist, std::string *, length);
    }
    else
    {
        RECREATE(namelist, std::string *, length);
    }
    if (olen != loc)
    {
        if (length > 1)
        {
            if (nadd == 1)
            {
                memmove(&namelist[loc + 1], &namelist[loc], sizeof(std::string *) * (olen - loc));
            }
        }
    }
    namelist[loc] = new std::string(name ? name : "");
    if (nadd > 1)
    {
        for (x = length - nadd; x < (length - 1); x++)
        {
            namelist[x] = new std::string("");
        }
    }
}
