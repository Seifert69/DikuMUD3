/*
 $Author: tperry $
 $RCSfile: trie.h,v $
 $Date: 2001/04/29 03:46:07 $
 $Revision: 2.1 $
 */

#ifndef _MUD_TRIE_H
#define _MUD_TRIE_H

struct trie_entry
{
    char c;
    struct trie_type *t;
};

struct trie_type
{
    void *data;
    ubit8 size;
    struct trie_entry *nexts;
};


void *search_trie (const char *s, struct trie_type *t);
void set_triedata (const char *s, struct trie_type *t, void *p, ubit1 verbose);
struct trie_type *add_trienode (const char *s, struct trie_type *t);
void free_trie (struct trie_type *t, void (*free_data) (void *));
ubit1 del_trie (char *s, struct trie_type **t, void (*)(void *));
void qsort_triedata (struct trie_type *t);

#endif /* _MUD_TRIE_H */
