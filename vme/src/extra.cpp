/*
 $Author: All $
 $RCSfile: extra.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.3 $
 */

#include "extra.h"

#include "error.h"
#include "textutil.h"
#include "utils.h"

//
// MS2020. It seems like the old code would set unit's extra to NULL
// and create new extras by calling the NULL->add... That causes crashes.
// Because of how DIL works, I need the *next. And therefore I can't use
// std::list (I tried in vain) or anything similar. So I've followed the
// existing structure adding only a controlling class (extra_list).

extra_descr_data::extra_descr_data()
{
    next = nullptr;
}

// MS2020: Super odd there was no constructor for adding data.
extra_descr_data::extra_descr_data(const char *name, const char *descr)
{
    const char *names[2] = {name, nullptr};

    this->names = names;
    if (descr)
    {
        this->descr = descr;
    }
    this->next = nullptr;
}

// MS2020: Super odd there was no constructor for adding data.
extra_descr_data::extra_descr_data(const char **names, const char *descr)
{
    this->names = names;
    if (descr)
    {
        this->descr = descr;
    }
    this->next = nullptr;
}

// MS2020: Super odd there was no constructor for adding data.
extra_descr_data::extra_descr_data(cNamelist names, const char *descr)
{
    this->names = names;
    if (descr)
    {
        this->descr = descr;
    }
    this->next = nullptr;
}

extra_descr_data::~extra_descr_data()
{
    this->next = nullptr;
}

// SIGH. Due to DIL I need to make this function on the extra_descr_data
// when really I only wanted it on the list
extra_descr_data *extra_descr_data::find_raw(const char *word)
{
    ubit32 i = 0;
    extra_descr_data *exd = nullptr;

    for (exd = this; exd; exd = exd->next)
    {
        if (exd->names.Length() < 1)
        {
            if (str_is_empty(word))
            {
                return exd;
            }
        }
        else
        {
            for (i = 0; i < exd->names.Length(); i++)
            {
                if (str_ccmp(word, exd->names.Name(i)) == 0)
                {
                    return exd;
                }
            }
        }
    }

    return nullptr;
}

// Argh, just for DIL :-( *sob*
void rogue_push_front(extra_descr_data **exlist, extra_descr_data *newex)
{
    assert(exlist && newex);

    newex->next = (*exlist);
    (*exlist) = newex;
}

// Sob
void rogue_remove(extra_descr_data **exlist, const char *name)
{
    extra_descr_data *tex = (*exlist)->find_raw(name);

    if (tex)
    {
        // See if it's the head element
        if (*exlist == tex)
        {
            *exlist = tex->next;
            tex->next = nullptr;
            delete tex;
            return;
        }

        // Find exd in the list (not the first element)
        extra_descr_data *pex = nullptr;

        for (pex = *exlist; pex->next; pex = pex->next)
        {
            if (pex->next == tex)
            {
                pex->next = tex->next;
                tex->next = nullptr;
                delete tex;
                return;
            }
        }
    }
}

// =============================================================
//               EXTRA LIST
// =============================================================

extra_list::extra_list()
{
    m_pList = nullptr;
}

extra_list::~extra_list()
{
    freelist(m_pList);
    m_pList = nullptr;
}

void extra_list::freelist(extra_descr_data *ex)
{
    if (ex)
    {
        freelist(ex->next); // Recurse to delete reverse
        delete ex;
    }
}

int extra_list::isempty()
{
    return m_pList == nullptr;
}

int extra_list::count()
{
    int i = 0;
    extra_descr_data *e = nullptr;

    // Count no of elements in list
    for (e = this->m_pList, i = 0; e; e = e->next)
    {
        i++;
    }

    return i;
}

std::string extra_list::json()
{
    std::string s;

    s = "\"extralist\": [ ";

    extra_descr_data *e = nullptr;

    // Count no of elements in list
    for (e = this->m_pList; e; e = e->next)
    {
        s.append("{");
        s.append(e->names.json());
        s.append(",");
        s.append(str_json("descr", e->descr));
        s.append(",");
        s.append(e->vals.json());
        s.append("}");

        if (e->next)
        {
            s.append(",\n");
        }
    }

    s.append(" ]");

    return s;
}

// Insert ex as the first element in front of the list
void extra_list::push_front(extra_descr_data *ex)
{
    ex->next = m_pList;
    m_pList = ex;
}

// Insert ex as the last element in front of the list
void extra_list::push_tail(extra_descr_data *ex)
{
    if (m_pList == nullptr)
    {
        push_front(ex);
    }
    else
    {
        extra_descr_data *p = nullptr;

        for (p = m_pList; p->next; p = p->next)
        {
            ;
        }

        // p now points to the last element in the list
        p->next = ex;
    }
}

// Unlink exd from the list and delete the object and return the next object
// Always returns the list object (why?)
void extra_list::erase(extra_descr_data *exd)
{
    assert(m_pList && exd);

    // See if it's the head element
    if (m_pList == exd)
    {
        m_pList = exd->next;
        exd->next = nullptr;
        delete exd;

        return;
    }

    // FInd exd in the list (not the first element)
    extra_descr_data *pex = nullptr;

    for (pex = m_pList; pex->next; pex = pex->next)
    {
        if (pex->next == exd)
        {
            pex->next = exd->next;
            exd->next = nullptr;
            delete exd;
            return;
        }
    }

    /* This should not be possible */

    error(HERE, "Remove extra descr got unmatching list and exd.\n");
    return;
}

// adds ex (not a copy of ex, so be sure it is allocated before calling)
extra_descr_data *extra_list::add(extra_descr_data *ex)
{
    push_front(ex);

    return ex;
}

/*                                                            */
/* Adds an extra description just before the one given (this) */
/*                                                            */
extra_descr_data *extra_list::add(const char **names, const char *descr)
{
    extra_descr_data *new_ex = nullptr;

    new_ex = new extra_descr_data(names, descr);
    push_front(new_ex);

    return new_ex;
}

extra_descr_data *extra_list::add(cNamelist cn, const char *descr)
{
    extra_descr_data *new_ex = nullptr;

    new_ex = new extra_descr_data(cn, descr);
    push_front(new_ex);

    return new_ex;
}

extra_descr_data *extra_list::add(const char *name, const char *descr)
{
    extra_descr_data *new_ex = nullptr;

    new_ex = new extra_descr_data(name, descr);
    push_front(new_ex);

    return new_ex;
}

void extra_list::remove(const char *name)
{
    extra_descr_data *tex = find_raw(name);

    if (tex)
    {
        this->erase(tex);
    }
}

void extra_list::AppendBuffer(CByteBuffer *pBuf)
{
    extra_descr_data *e = nullptr;

    assert(this);

    // MS2020 had to rewrite, was a mess :)
    pBuf->Append32(this->count()); // Store number of extra_descr elements

    /* While description is non null, keep writing */
    for (e = this->m_pList; e; e = e->next)
    {
        pBuf->AppendString(e->descr.c_str());
        e->names.AppendBuffer(pBuf);
        e->vals.AppendBuffer(pBuf);
    }
}

/*                                                            */
/* Only matches on complete letter by letter match!           */
/* The empty word "" or NULL matches an empty namelist.       */
/*                                                            */
extra_descr_data *extra_list::find_raw(const char *word)
{
    if (this->m_pList)
    {
        return this->m_pList->find_raw(word);
    }
    else
    {
        return nullptr;
    }
}

// From the elist object, make a copy of the list
void extra_list::copyfrom(extra_list &listToBeCopied)
{
    extra_descr_data *new_ex = nullptr;

    assert(this->m_pList == nullptr);

    extra_descr_data *exd = nullptr;
    for (exd = listToBeCopied.m_pList; exd; exd = exd->next)
    {
        new_ex = new extra_descr_data(exd->names, exd->descr.c_str());
        this->add(new_ex);
    }
}

/* ===================================================================== */
/*   The following should really be in the unit_data class, but we dont  */
/*   have it yet...                                                      */

/*  We don't want people to be able to see $-prefixed extras
 *  so check for that...
 */
extra_descr_data *unit_find_extra(const char *word, unit_data *unit)
{
    word = skip_spaces(word);

    if (unit && *word != '$')
    {
        extra_descr_data *i = nullptr;

        word = skip_spaces(word);

        for (i = UNIT_EXTRA(unit).m_pList; i; i = i->next)
        {
            if (i->names.Name())
            {
                if (i->names.Name(0)[0] == '$')
                {
                    continue;
                }

                if (i->names.IsName(word))
                {
                    return i;
                }
            }
            else if (UNIT_NAMES(unit).IsName(word))
            {
                return i;
            }
        }
    }

    return nullptr;
}

extra_descr_data *char_unit_find_extra(unit_data *ch, unit_data **target, char *word, unit_data *list)
{
    extra_descr_data *exd = nullptr;

    if (!list)
    {
        return nullptr;
    }

    if (IS_ROOM(list))
    {
        if (CHAR_CAN_SEE(ch, list) && (exd = unit_find_extra(word, list)))
        {
            if (target)
            {
                *target = list;
            }
            return exd;
        }
        else
        {
            list = list->inside;
        }
    }

    for (; list; list = list->next)
    {
        if (CHAR_CAN_SEE(ch, list) && (exd = unit_find_extra(word, list)))
        {
            if (target)
            {
                *target = list;
            }
            return exd;
        }
    }

    if (target)
    {
        *target = nullptr;
    }

    return nullptr;
}

const char *unit_find_extra_string(unit_data *ch, char *word, unit_data *list)
{
    extra_descr_data *exd = char_unit_find_extra(ch, nullptr, word, list);

    if (exd)
    {
        return exd->descr.c_str();
    }

    return nullptr;
}
