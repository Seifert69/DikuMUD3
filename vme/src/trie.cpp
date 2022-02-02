/*
 $Author: tperry $
 $RCSfile: trie.cpp,v $
 $Date: 2001/04/29 03:46:07 $
 $Revision: 2.1 $
 */

#include "trie.h"

#include "slog.h"
#include "structs.h"

#include <cstdlib>
#include <cstring>

static int trie_size = 0, trie_nodes = 0;

int trie_src_cmp(const void *keyval, const void *datum)
{
    return (((char *)keyval)[0] - ((trie_entry *)datum)->c);
}

int trie_sort_cmp(const void *keyval, const void *datum)
{
    return (((trie_entry *)keyval)->c - ((trie_entry *)datum)->c);
}

trie_entry *triebindex(char c, trie_type *t)
{
    if (t->nexts)
    {
        return (trie_entry *)bsearch(&c, t->nexts, t->size, sizeof(trie_entry), trie_src_cmp);
    }
    else
    {
        return nullptr;
    }
}

void qsort_triedata(trie_type *t)
{
    int i = 0;

    qsort(t->nexts, t->size, sizeof(trie_entry), trie_sort_cmp);

    for (i = 0; i < t->size; i++)
    {
        qsort_triedata(t->nexts[i].t);
    }
}

int trie_index(char c, trie_type *t)
{
    int i = 0;

    /* See if the char already exists in array */
    for (i = 0; i < t->size; i++)
    {
        if (t->nexts[i].c == c)
        {
            break;
        }
    }

    return i < t->size ? i : -1;
}

trie_type *add_trienode(const char *s, trie_type *t)
{
    int i = 0;

    /* If no node is given, create one */
    if (t == nullptr)
    {
        trie_size += sizeof(trie_type);
        trie_nodes++;

        CREATE(t, trie_type, 1);
        t->data = nullptr;
        t->nexts = nullptr;
        t->size = 0;
    }

    if (*s)
    {
        i = trie_index(*s, t);

        if (i == -1)
        {
            trie_size += sizeof(trie_entry);

            if (t->size == 0)
            {
                CREATE(t->nexts, trie_entry, 1);
            }
            else
            {
                RECREATE(t->nexts, trie_entry, t->size + 1);
            }
            t->size++;

            t->nexts[t->size - 1].c = *s;
            t->nexts[t->size - 1].t = add_trienode(s + 1, nullptr);
        }
        else
        {
            add_trienode(s + 1, t->nexts[i].t);
        }
    }

    return t;
}

void set_triedata(const char *s, trie_type *t, void *p, ubit1 nonabbrev)
{
    int i = 0;

    while (*s && t)
    {
        if (t->data == nullptr && !nonabbrev)
        {
            t->data = p;
        }

        i = trie_index(*s, t);

        if (i == -1)
        {
            slog(LOG_ALL, 0, "Bad, unlikely thing in set_triedata!");
            return; /* This is bad! */
        }
        else
        {
            t = t->nexts[i].t;
            s++;
        }
    }

    if (t && t->data == nullptr)
    {
        t->data = p;
    }
}

/* It runs in nothing less than O(|s|), returns the data pointer */
void *search_trie(const char *s, trie_type *t)
{
    trie_entry *te = nullptr;

    for (; *s && t; t = te->t, ++s)
    {
        if ((te = triebindex(*s, t)) == nullptr)
        { /* O(1) since always < 256 */
            return nullptr;
        }
    }

    return t->data;
}

/*  The following two procedures work by being supplied another procedure
 *  which takes care of erasing the data at the nodes.
 *  Use at own risk! :)
 */

/* Free an entire trie */
void free_trie(trie_type *t, void (*free_data)(void *))
{
    ubit8 i = 0;

    /* If there's any data at this node, let the supplied method free it */
    if (t->data)
    {
        (*free_data)(t->data);
    }

    /* Subtract size of free'ed info */
    trie_size -= (t->size * sizeof(trie_entry) + sizeof(trie_type));
    --trie_nodes;

    /* Walk through node-array, and call recursively */
    for (i = 0; i < t->size; i++)
    {
        if (t->nexts[i].t)
        {
            free_trie(t->nexts[i].t, free_data);
        }
    }

    /* Clean up the last bits */
    if (t->size > 0)
        FREE(t->nexts);

    FREE(t);
}

/*  The deletion of trie-entries is ONLY possible if they're created in
 *  nonabbrev mode!
 *  The boolean returned is entirely for internal usage.
 *
 *  Overcommented due to non-trivialism :-)
 */
ubit1 del_trie(char *s, trie_type **t, void (*free_data)(void *))
{
    /* Any more string of keyword to delete? */
    if (*s)
    { /* Yep, let's find next chars index */
        int i = trie_index(*s, *t);

        /* Found one, did deletion of rest of keyword delete node below? */
        if (i >= 0 && del_trie(s + 1, &((*t)->nexts[i].t), free_data))
        {
            if ((*t)->size == 1) /* Yes.  Are we alone at this node? */
            {                    /* Yep, delete and confirm */
                trie_size -= (sizeof(trie_entry) + sizeof(trie_type));
                trie_nodes--;

                FREE((*t)->nexts);
                FREE(*t);
                *t = nullptr;
                return TRUE;
            }
            else /* No, so we have to clean up carefully */
            {
                trie_size -= sizeof(trie_entry);

                (*t)->size--;
                for (; i < (*t)->size; i++)
                {
                    (*t)->nexts[i] = (*t)->nexts[i + 1];
                }
                RECREATE((*t)->nexts, trie_entry, (*t)->size);
            }
        }
    } /* No more string.  Is there data at this node? */
    else if ((*t)->data)
    { /* Yep, delete */
        (*free_data)((*t)->data);
        (*t)->data = nullptr;

        if ((*t)->size == 0) /* Is this a leaf? */
        {                    /* Yep, delete it, and confirm */
            trie_nodes--;
            trie_size -= sizeof(trie_type);
            FREE(*t);
            *t = nullptr;
            return TRUE;
        }
    }
    /* We didn't get to delete a node (sulk) */
    return FALSE;
}

/*
 * This is how a build trie ought to look
 * Refer to interpreter.c for an example in use.

struct trie_type *build_trie(char *strs[])
{
  struct trie_type *trie = NULL;
  int i;

  for (i = 0; *strs[i]; i++)
    trie = add_trienode(strs[i], trie);

  qsort_triedata(trie);

  return trie;
}

*/
