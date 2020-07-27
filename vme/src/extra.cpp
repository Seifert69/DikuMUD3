/*
 $Author: All $
 $RCSfile: extra.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.3 $
 */

#include "extra.h"
#include "utils.h"
#include "utility.h"
#include "textutil.h"

//
// MS2020. It seems like the old code would set unit's extra to NULL
// and create new extras by calling the NULL->add... That causes crashes.
// Because of how DIL works, I need the *next. And therefore I can't use
// std::list (I tried in vain) or anything similar. So I've followed the
// existing structure adding only a controlling class (extra_list).

extra_descr_data::extra_descr_data(void)
{
    next = NULL;
}

// MS2020: Super odd there was no constructor for adding data.
extra_descr_data::extra_descr_data(const char *name, const char *descr)
{
    const char *names[2] = {name, NULL};

    this->names = names;
    if (descr)
        this->descr = descr;
    this->next = NULL;
}

// MS2020: Super odd there was no constructor for adding data.
extra_descr_data::extra_descr_data(const char **names, const char *descr)
{
    this->names = names;
    if (descr)
        this->descr =  descr;
    this->next = NULL;
}

// MS2020: Super odd there was no constructor for adding data.
extra_descr_data::extra_descr_data(cNamelist names, const char *descr)
{
    this->names = names;
    if (descr)
        this->descr =  descr;
    this->next = NULL;
}

extra_descr_data::~extra_descr_data(void)
{
    this->next = NULL;
}


// SIGH. Due to DIL I need to make this function on the extra_descr_data
// when really I only wanted it on the list
class extra_descr_data *extra_descr_data::find_raw(const char *word)
{
    ubit32 i;
    class extra_descr_data *exd; 
    
    for (exd=this; exd; exd = exd->next)
    {
        if (exd->names.Length() < 1)
        {
            if (str_is_empty(word))
                return exd;
        }
        else
        {
            for (i = 0; i < exd->names.Length(); i++)
                if (str_ccmp(word, exd->names.Name(i)) == 0)
                    return exd;
        }
    }

    return NULL;
}

// Argh, just for DIL :-( *sob*
void rogue_push_front(class extra_descr_data **exlist, class extra_descr_data *newex)
{
    assert(exlist && newex);

    newex->next = (*exlist);
    (*exlist) = newex;
}

// Sob
void rogue_remove(class extra_descr_data **exlist, const char *name)
{
    class extra_descr_data *tex = (*exlist)->find_raw(name);

    if (tex)
    {
        // See if it's the head element
        if (*exlist  == tex)
        {
            *exlist = tex->next;
            tex->next = NULL;
            delete tex;
            return;
        }

        // Find exd in the list (not the first element)
        class extra_descr_data *pex;

        for (pex = *exlist; pex->next; pex = pex->next)
            if (pex->next == tex)
            {
                pex->next = tex->next;
                tex->next = NULL;
                delete tex;
                return;
            }


    }
}



// =============================================================
//               EXTRA LIST
// =============================================================

extra_list::extra_list(void)
{
    m_pList = NULL;
}

extra_list::~extra_list(void)
{
    freelist(m_pList);
    m_pList = NULL;
}

void extra_list::freelist(class extra_descr_data *ex)
{
    if (ex)
    {
        freelist(ex->next); // Recurse to delete reverse
        delete ex;
    }
}

int extra_list::isempty(void)
{
    return m_pList == NULL;
}

int extra_list::count(void)
{
    int i;
    class extra_descr_data *e;

    // Count no of elements in list
    for (e = this->m_pList, i = 0; e; e = e->next)
        i++;

    return i;
}

// Insert ex as the first element in front of the list
void extra_list::push_front(class extra_descr_data *ex)
{
    ex->next = m_pList;
    m_pList = ex;
}

// Insert ex as the last element in front of the list
void extra_list::push_tail(class extra_descr_data *ex)
{
    if (m_pList == NULL)
        push_front(ex);
    else
    {
        class extra_descr_data *p;

        for (p = m_pList; p->next; p = p->next)
            ;
        
        // p now points to the last element in the list
        p->next = ex;
    }
}


// Unlink exd from the list and delete the object and return the next object
// Always returns the list object (why?)
void extra_list::erase(class extra_descr_data *exd)
{
    assert(m_pList && exd);

    // See if it's the head element
    if (m_pList == exd)
    {
        m_pList = exd->next;
        exd->next = NULL;
        delete exd;

        return;
    }

    // FInd exd in the list (not the first element)
    class extra_descr_data *pex;

    for (pex = m_pList; pex->next; pex = pex->next)
        if (pex->next == exd)
        {
            pex->next = exd->next;
            exd->next = NULL;
            delete exd;
            return;
        }

    /* This should not be possible */

    error(HERE, "Remove extra descr got unmatching list and exd.\n");
    return;
}

// adds ex (not a copy of ex, so be sure it is allocated before calling)
class extra_descr_data *extra_list::add(class extra_descr_data *ex)
{
    push_front(ex);

    return ex;
}


/*                                                            */
/* Adds an extra description just before the one given (this) */
/*                                                            */
class extra_descr_data *extra_list::add(const char **names, const char *descr)
{
    class extra_descr_data *new_ex;

    new_ex = new class extra_descr_data(names, descr);
    push_front(new_ex);

    return new_ex;
}


class extra_descr_data *extra_list::add(cNamelist cn, const char *descr)
{
    class extra_descr_data *new_ex;

    new_ex = new class extra_descr_data(cn, descr);
    push_front(new_ex);

    return new_ex;
}

class extra_descr_data *extra_list::add(const char *name, const char *descr)
{
    class extra_descr_data *new_ex;

    new_ex = new class extra_descr_data(name, descr);
    push_front(new_ex);

    return new_ex;
}

void extra_list::remove(const char *name)
{
    class extra_descr_data *tex = find_raw(name);

    if (tex)
        this->erase(tex);
}

void extra_list::AppendBuffer(CByteBuffer *pBuf)
{
    class extra_descr_data *e;

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
class extra_descr_data *extra_list::find_raw(const char *word)
{
    if (this->m_pList)
        return this->m_pList->find_raw(word);
    else
        return NULL;
}


// From the elist object, make a copy of the list
void extra_list::copyfrom(class extra_list &listToBeCopied)
{
    class extra_descr_data *new_ex;

    assert(this->m_pList == NULL);
    
    class extra_descr_data *exd;
    for (exd = listToBeCopied.m_pList; exd; exd = exd->next)
    {
        new_ex = new class extra_descr_data(exd->names, exd->descr.c_str());
        this->add(new_ex);
    }
}

/* ===================================================================== */
/*   The following should really be in the unit_data class, but we dont  */
/*   have it yet...                                                      */

/*  We don't want people to be able to see $-prefixed extras
 *  so check for that...
 */
class extra_descr_data *unit_find_extra(const char *word, class unit_data *unit)
{
    word = skip_spaces(word);

    if (unit && *word != '$')
    {
        class extra_descr_data *i;

        word = skip_spaces(word);

        for (i = UNIT_EXTRA(unit).m_pList; i; i = i->next)
        {
            if (i->names.Name())
            {
                if (i->names.Name(0)[0] == '$')
                    continue;

                if (i->names.IsName(word))
                    return i;
            }
            else if (UNIT_NAMES(unit).IsName(word))
                return i;
        }
    }

    return NULL;
}

class extra_descr_data *char_unit_find_extra(class unit_data *ch,
                     class unit_data **target,
                     char *word, class unit_data *list)
{
    class extra_descr_data *exd = NULL;

    if (!list)
        return NULL;

    if (IS_ROOM(list))
    {
        if (CHAR_CAN_SEE(ch, list) && (exd = unit_find_extra(word, list)))
        {
            if (target)
                *target = list;
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
                *target = list;
            return exd;
        }
    }

    if (target)
        *target = NULL;

    return NULL;
}

const char *unit_find_extra_string(class unit_data *ch,
                                   char *word, class unit_data *list)
{
    class extra_descr_data *exd = char_unit_find_extra(ch, NULL, word, list);

    if (exd)
        return exd->descr.c_str();

    return NULL;
}
