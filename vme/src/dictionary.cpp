/*
 $Author: All $
 $RCSfile: dictionary.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.4 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "textutil.h"
#include "interpreter.h"
#include "utility.h"
#include "db_file.h"
#include "trie.h"
#include "handler.h"
#include "queue.h"
#include "config.h"

#define MAX_ALIAS_LENGTH 10  /* Max length of alias */
#define MAX_ALIAS_COUNT 2000 /* Max room for string saved to file */
#define ALIAS_UPPER_BOUND 50 /* Max number of nodes in alias call-tree */
#define ALIAS_NAME 21        /* Length of extra string allocated for the  \
                              * name of dictionary owner + 2              \
                              * Unforgiveably hardcoded in a str_to_alias \
                              * call of sscanf()...                       \
                              */
struct alias_head
{
   ubit16 char_count;
   struct trie_type *trie;
   char owner[20];
};

struct alias_t
{
   char *key;
   char *value;
};

/* Procedure supplied to free_trie and del_trie */
void free_alias(void *data)
{
   struct alias_t *al = (struct alias_t *)data;

   FREE(al->key);
   FREE(al->value);
   FREE(al);
}

/* Word is here defined as an unbroken sequence of nonspaces */
static char *get_next_word(const char *argument, char *first_arg)
{
   /* Find first non blank */
   argument = skip_spaces(argument);

   /* Copy next word and make it lower case */
   for (; *argument && !isspace(*argument); argument++)
      *first_arg++ = tolower(*argument);

   *first_arg = '\0';

   return (char *)argument;
}

/* Setup owner-ship of dictionary. */
static void set_owner(class unit_data *obj, struct alias_head *ah, class unit_data *ch)
{
   char buf[256];

   strcpy(ah->owner, UNIT_NAME(ch));

   sprintf(buf, "On the ground lies %s's %s.", ah->owner, UNIT_NAME(obj));
   UNIT_OUT_DESCR(obj) = buf;

   sprintf(buf, "%s's %s", ah->owner, UNIT_NAME(obj));
   UNIT_TITLE(obj) = buf;
}

/* Allocate `exd', and erase old description */
/* MS2020 rewritten */
static class extra_descr_data *make_alias_extra(class unit_data *u)
{
   class extra_descr_data *exd;
   static const char *aliaslist[] = {"$alias", NULL};

   exd = UNIT_EXTRA(u).find_raw("$alias");

   if (exd)
      return exd;

   exd = new extra_descr_data(aliaslist, NULL);
   UNIT_EXTRA(u).add(exd);

   return exd;
}

/* Parse the alias, according to the &-rule sketched out in help-page */
static char *parse_alias(char *src, char *arg)
{
   static char buf[2 * MAX_INPUT_LENGTH + 2];
   char *argv[10], *cnew = buf;
   int argc, i = 0;

   for (argc = 1; argc < 10; argc++) /* store word-pointers in argv */
   {
      while (*arg && isspace(*arg))
         arg++;         /* skip spaces */
      argv[argc] = arg; /* record first non-space pos. */
      while (*arg && !isspace(*arg))
         arg++; /* skip non-spaces */
   }

   for (argc = 1; (*cnew = *src);) /* check src for `&' substitution */
   {
      if ((*src++ == '$') && (*src != '\\'))
      {
         if (isdigit(*src) && (i = *src - '0') > 0) /* Digit picks argument */
         {
            get_next_word(argv[i], cnew);
            src++;
         }
         else if (strchr(src, '$')) /* Current `&' isn't the last */
            get_next_word(argv[argc++], cnew);
         else /* It is the last, so it takes rest of argument */
            strcpy(cnew, argv[argc]);
         TAIL(cnew);
      }
      else
         cnew++;

      if (cnew - buf >= (sbit32)sizeof(buf) / 2 - 1)
         return NULL; /* we wrote too much, so abort */
   }

   return buf;
}

/*  Pars
the alias-substitution in s, using the arguments from arg, and
 *  looking up nested alias-calls in the alias-trie t.
 *  check_count makes sure that the user doesn't make nasty (!) computationally
 *  heavy (!!!) aliases that will bog the mud enormously.
 */
static int push_alias(char *s, char *arg, struct trie_type *t,
                      class unit_data *ch, bool first)
{
   char cmd[MAX_INPUT_LENGTH + 1], parsed[2 * MAX_INPUT_LENGTH + 2];
   char *par, *newarg, *c;
   struct alias_t *al;

   static ubit8 check_count = 0;

   if (first)
      check_count = 0;
   else if (check_count++ > ALIAS_UPPER_BOUND)
   {
      send_to_char("Alias was too deeply nested. Aborted.<br/>", ch);
      return -1;
   }

   do
   {
      /* Cut off at semi-colons, but remember position for next iteration */
      if ((c = strchr(s, ';')))
         *c = '\0';

      /* Parse the alias substitution */
      if (!(par = parse_alias(s, arg)))
      {
         send_to_char("Overflow while parsing alias. Aborted.<br/>", ch);
         return -1;
      }

      /* Try to parse result of parsing (!) */
      strcpy(parsed, par);
      newarg = (char *)get_next_word(parsed, cmd);

      if ((al = (struct alias_t *)search_trie(cmd, t)))
      {
         if (push_alias(al->value, newarg, t, ch, FALSE) < 0)
            return -1;
      }
      else /* not a new alias, so it's probably a command */
         CHAR_DESCRIPTOR(ch)->qInput.Append(new cQueueElem(par));

      if (c) /* Restore the semi-colon */
         *c = ';';

      s = c + 1;
   } while (c);

   return 0;
}

/* Merely prints a formatted alias-definition out to the char */
static void alias_to_char(struct alias_t *al, class unit_data *ch)
{
   char buf[2 * MAX_INPUT_LENGTH + 2];

   sprintf(buf, " %-*s%s", MAX_ALIAS_LENGTH + 5, al->key, al->value);
   act("$2t", A_ALWAYS, ch, buf, cActParameter(), TO_CHAR);
}

/*  Prints all defined aliases in `t' alphabetically to char by
 *  recursively walking the trie
 */
static int print_alias(struct trie_type *t, class unit_data *ch)
{
   struct trie_type *t2;
   struct alias_t *al;
   int i, count = 0;

   for (i = 0; t && i < t->size; i++)
   {
      t2 = t->nexts[i].t;

      if ((al = (struct alias_t *)t2->data))
      {
         alias_to_char(al, ch);
         count++;
      }
      count += print_alias(t2, ch);
   }
   return count;
}

/*  Add the new alias `key' to `ah'.
 *  single says if we plan to add only one alias or several at `boottime'.
 *  If false, it is assumed the user sorts the trie himself
 */
static int add_alias(struct alias_head *ah, char *key, char *val, bool single)
{
   struct alias_t *al = NULL, *tmp_al;

   /* There is already an alias for key - Delete it */
   if (single && ah->trie &&
       (al = (struct alias_t *)search_trie(key, ah->trie)))
   {
      FREE(al->value);
      al->value = str_dup(val);
      return 0;
   }

   CREATE(tmp_al, struct alias_t, 1);
   tmp_al->key = str_dup(key);
   tmp_al->value = str_dup(val);
   ah->char_count += (strlen(key) + strlen(val) + 2);

   ah->trie = add_trienode(key, ah->trie);
   if (single)
      qsort_triedata(ah->trie);
   set_triedata(key, ah->trie, tmp_al, TRUE);

   return 1;
}

/* Delete an alias, using the free_alias procedure */
static bool del_alias(struct alias_head *ah, char *key)
{
   struct alias_t *al = NULL;

   if (ah->trie && (al = (struct alias_t *)search_trie(key, ah->trie)))
   {
      ah->char_count -= (strlen(al->key) + strlen(al->value) + 2);

      del_trie(key, &ah->trie, free_alias);

      return TRUE;
   }

   return FALSE;
}

/*  Parse the alias to see if `val' calls something that will eventually call
 *  `key', or if it contains state-changing commands.
 *  check_count makes sure that the user doesn't make nasty (!) computationally
 *  heavy (!!!) aliases that will bog the mud enormously.
 */
static ubit8 circle_alias(char *key, char *val, struct trie_type *t, bool first)
{
   char *tmp, *sc, comm[MAX_INPUT_LENGTH + 2];
   struct alias_t *tmp_al = NULL;
   ubit8 res = 0;
   static ubit8 check_count = 0;

   if (first)
      check_count = 0;
   else if (check_count++ > ALIAS_UPPER_BOUND)
      return 3;

   tmp = val;

   do
   {
      if ((sc = strchr(tmp, ';')))
         *sc = '\0';

      get_next_word(tmp, comm);

      if (sc)
         *sc = ';';

      if (!str_ccmp(comm, key))
         return 1;
      else if (first && (is_abbrev(comm, "alias") || is_abbrev(comm, "unalias")))
         return 2;
      else if (first && *comm == '!')
         return 4;
      else if (first && is_abbrev(comm, "shout"))
         return 5;

      if (t && (tmp_al = (struct alias_t *)search_trie(comm, t)) && (res = circle_alias(key, tmp_al->value, t, FALSE)))
         return res;

      tmp = sc + 1;
   } while (sc);

   return 0;
}

/*  Is `key' a legal new alias or not?
 *  This should all be linear, except for the recursion check,
 *  which has a fixed upper bound.
 */
static bool alias_is_ok(struct alias_head *ah, char *key, char *val,
                        class unit_data *ch)
{
   char *tmp;
   struct alias_t *al = NULL;
   int count;

   if (strlen(key) > MAX_ALIAS_LENGTH)
   {
      count = MAX_ALIAS_LENGTH;

      act("Aliasname too long. Max $2d chars.",
         A_ALWAYS, ch, &count, cActParameter(), TO_CHAR);
      return FALSE;
   }

   count = strlen(key) + strlen(val) + ah->char_count + 3;
   if (ah->trie && (al = (struct alias_t *)search_trie(key, ah->trie)))
      count -= strlen(al->value);

   if (count > MAX_ALIAS_COUNT)
   {
      send_to_char("You will have to delete another alias first.<br/>", ch);
      return FALSE;
   }

   if (is_abbrev(key, "alias") || is_abbrev(key, "unalias"))
   {
      send_to_char("Alas, but that is a reserved name.<br/>", ch);
      return FALSE;
   }

   for (count = 0, tmp = val; *tmp; tmp++)
      switch (*tmp)
      {
      case '~':
         send_to_char("Sorry, `~' is used as a control character.<br/>", ch);
         return FALSE;

      case ';':
         if (*skip_spaces(tmp + 1) == ';')
         {
            send_to_char("Semicolon error in alias.<br/>", ch);
            return FALSE;
         }
         break;

      case '$':
         if (!isdigit(tmp[1]) && ++count > 9)
         {
            send_to_char("Sorry, `$' can't be used more than 9 times.<br/>",
                         ch);
            return FALSE;
         }
      }

   switch (circle_alias(key, val, ah->trie, TRUE))
   {
   case 1:
      send_to_char("Aliases are not allowed to call in circle.<br/>", ch);
      return FALSE;
   case 2:
      send_to_char("Aliases can't be defined or erased within an alias.<br/>",
                   ch);
      return FALSE;
   case 3:
      send_to_char("You're not allowed to nest aliases that deep.<br/>", ch);
      return FALSE;
   case 4:
      send_to_char("You can't use '!' from within an alias.<br/>", ch);
      return FALSE;
   case 5:
      if (!g_cServerConfig.m_bAliasShout)
      {
         send_to_char("Using `shout' in an alias is not very nice...<br/>",
                      ch);
         return FALSE;
      }
   }

   return TRUE;
}

/*  This walks the trie recursively to note down the aliases in the buffer
 *  pointed to by bufp
 */
static void rec_alias_to_str(struct trie_type *t, char **bufp)
{
   int i;
   struct alias_t *al;

   if (t)
   {
      if ((al = (struct alias_t *)t->data))
      {
         sprintf(*bufp, "%s %s~", al->key, al->value);
         TAIL(*bufp);
      }

      for (i = 0; i < t->size; i++)
         rec_alias_to_str(t->nexts[i].t, bufp);
   }
}

/* Convert a trie to a string.  Needed for saving the aliases */
static char *alias_to_str(struct alias_head *ah)
{
   static char buf[MAX_ALIAS_COUNT + ALIAS_NAME + 2 + 500];
   char *bufp = buf;

   sprintf(bufp, "%s~ ", ah->owner); /* Note down owner of dictionary */
   TAIL(bufp);

   rec_alias_to_str(ah->trie, &bufp);

   *bufp = '\0';

   return buf;
}

/* Take the above constructed string and turn it into an alias-set. */
static struct alias_head *str_to_alias(const char *str)
{
   struct alias_head *ah;
   char cmd[MAX_ALIAS_LENGTH + 1], *tmp, *tilde;

   CREATE(ah, struct alias_head, 1);
   ah->char_count = 0;
   ah->trie = NULL;
   strcpy(ah->owner, "none");

   if (str)
   {
      /*  This takes the first word of the alias-string.
         *  If it's tilde-terminated, it's the name of the owner.
         *  Otherwise it's an old-style dictionary, which will be throw into
         *  space, where it will die like a miserable pig.
         */

      if (sscanf(str, "%19s", ah->owner) == 1 && ah->owner[strlen(ah->owner) - 1] == '~')
      {
         str += strlen(ah->owner);
         str = skip_spaces(str);

         ah->owner[strlen(ah->owner) - 1] = '\0'; /* Cut of tilde */
      }
      else
         sprintf(ah->owner, "old_hat");

      while (*str)
      {
         char t = '~';
         tilde = (char *)strchr(str, t); /* Find first tilde in string */
         *tilde = '\0';
         tmp = get_next_word(str, cmd); /* Get alias-name */

         /* Add alias, but don't sort the trie yet */
         add_alias(ah, cmd, skip_spaces(tmp), FALSE);
         *tilde = '~';
         str = tilde + 1;
      }

      if (ah->trie)
         qsort_triedata(ah->trie);
   }

   return ah;
}

/* ********** The Alias Commands ********** */

static void cmd_alias(class unit_data *ch, char *arg, struct alias_head *alias_h)
{
   char comm[MAX_INPUT_LENGTH + 1];
   struct alias_t *al = NULL;

   if (str_is_empty(arg))
   {
      /* No argument lists the defined aliases */
      if (print_alias(alias_h->trie, ch) == 0)
      {
         send_to_char("No aliases defined.<br/>", ch);
         if (alias_h->char_count > 0)
            slog(LOG_BRIEF, 0, "%s: No aliases, but %d in char_count.",
                 __FILE__, alias_h->char_count);
      }

      return;
   }

   /* Cut off first argument */
   arg = get_next_word(arg, comm);

   if (str_is_empty(arg))
   {
      /* No further arguments lists this alias, if defined */
      if (alias_h->trie == NULL || (al =
                                        (struct alias_t *)search_trie(comm, alias_h->trie)) == NULL)
         act("No alias defined for `$2t'.", A_ALWAYS, ch, comm, cActParameter(), TO_CHAR);
      else
         alias_to_char(al, ch);

      return;
   }

   /* There's a new alias to re/define */
   arg = skip_spaces(arg);

   if (alias_is_ok(alias_h, comm, arg, ch))
   {
      int status = add_alias(alias_h, comm, arg, TRUE);

      act("Alias for `$2t' $3t.", A_ALWAYS, ch, comm, status ? "added" : "changed", TO_CHAR);
   }
}

static void cmd_unalias(class unit_data *ch, char *arg, struct alias_head *alias_h)
{
   if (str_is_empty(arg))
      act("Unalias what?", A_ALWAYS, ch, cActParameter(), cActParameter(), TO_CHAR);
   else
   {
      char comm[MAX_INPUT_LENGTH + 1];

      /* unalias cycles through all its arguments and undefines them */
      do
      {
         arg = get_next_word(arg, comm);

         act(del_alias(alias_h, comm) ? "Alias `$2t' deleted." : "No alias defined for `$2t'.",
             A_ALWAYS, ch, comm, cActParameter(), TO_CHAR);
      } while (!str_is_empty(arg));
   }
}

static void cmd_claim(class unit_data *ch, char *arg, class unit_data *obj,
                      struct alias_head *alias_h)
{
   char buf[MAX_INPUT_LENGTH + 1];

   one_argument(arg, buf);

   if (str_is_empty(buf) || !UNIT_NAMES(obj).IsName(buf))
      act("You can only claim $2n.", A_ALWAYS, ch, obj, cActParameter(), TO_CHAR);
   else
   {
      act("You claim $2n as your property.", A_ALWAYS, ch, obj, cActParameter(), TO_CHAR);
      set_owner(obj, alias_h, ch);
   }
}

/* ********** The Dictionary Specproc ********** */

static int local_dictionary(struct spec_arg *sarg)
{
   char *pcomm = NULL, *cmd_array[256];
   ubit16 i;
   struct alias_t *al = NULL;
   struct alias_head *alias_h;
   class extra_descr_data *exd;

   /* specproc initialization */
   if ((alias_h = (struct alias_head *)sarg->fptr->data) == NULL)
   {
      exd = UNIT_EXTRA(sarg->owner).find_raw("$alias");
      sarg->fptr->data = str_to_alias(exd ? exd->descr.c_str() : NULL);
      alias_h = (struct alias_head *)sarg->fptr->data;
   }

   /* clean up if destroyed */
   if (sarg->cmd->no == CMD_AUTO_EXTRACT)
   {
      if (alias_h->trie)
         free_trie(alias_h->trie, free_alias);
      FREE(alias_h);
      sarg->fptr->data = NULL;
      return SFR_BLOCK;
   }

   /* create a snapshot of alias-set if saved */
   else if (sarg->cmd->no == CMD_AUTO_SAVE)
   {
      char *tmp;

      exd = make_alias_extra(sarg->owner);

      tmp = alias_to_str(alias_h);
      assert(strlen(tmp) < MAX_ALIAS_COUNT + ALIAS_NAME + 2 + 500);
      exd->descr = tmp;

      return SFR_SHARE;
   }

   /* Not an applicaple command */
   if (!sarg->activator || !IS_CHAR(sarg->activator) ||
       !CHAR_DESCRIPTOR(sarg->activator) ||
       sarg->activator != UNIT_IN(sarg->owner))
      return SFR_SHARE;

   /* Check for the dictionary-commands */

   /* The alias command */
   if (is_command(sarg->cmd, "alias"))
   {
      /* This check is needed too, so people can't wreak havoc with another
         * person's dictionary...
         */
      if (str_ccmp(UNIT_NAME(sarg->activator), alias_h->owner))
         act("You can't use the alias command before you type `claim $3N'.",
             A_ALWAYS, sarg->activator, cActParameter(), sarg->owner, TO_CHAR);
      else
         cmd_alias(sarg->activator, (char *)sarg->arg, alias_h);

      return SFR_BLOCK;
   }
   /* The unalias command */
   else if (is_command(sarg->cmd, "unalias"))
   {
      cmd_unalias(sarg->activator, (char *)sarg->arg, alias_h);
      return SFR_BLOCK;
   }
   /* The claim command is used to `accept' the current dictionary */
   else if (is_command(sarg->cmd, "claim"))
   {
      cmd_claim(sarg->activator, (char *)sarg->arg, sarg->owner, alias_h);
      return SFR_BLOCK;
   }

   /*  Have to get the typed command from descriptor.last_cmd,
     *  as the user might have typed `in', which the interpreter expands
     *  to `inventory' in the cmd->cmd_str.
     */
   if (alias_h->trie == NULL ||
       (al =
            (struct alias_t *)
                search_trie(CHAR_DESCRIPTOR(sarg->activator)->last_cmd,
                            alias_h->trie)) == NULL)
   {
      return SFR_SHARE;
   }

   /* We now have an alias to parse... */

   /* Check if this dictionary belongs to user */
   if (str_ccmp(UNIT_NAME(sarg->activator), alias_h->owner))
   {
      act("You can't use the alias `$2t' before you type `claim $3N'.",
          A_ALWAYS, sarg->activator, sarg->cmd->cmd_str, sarg->owner, TO_CHAR);
      return SFR_BLOCK;
   }

   sarg->arg = skip_spaces(sarg->arg);

   /*  In the unlikely (?) event that one or more commands sneak in on the
     *  command queue before we get to to process, we have to empty it...
     */
   for (i = 0; !CHAR_DESCRIPTOR(sarg->activator)->qInput.IsEmpty();)
      if (i < 256) /* 256 should certainly be enough, but check anyway */
      {
         class cQueueElem *qe =
             CHAR_DESCRIPTOR(sarg->activator)->qInput.GetHead();
         cmd_array[i++] = (char *)qe->Data();
         qe->SetNull();
         delete qe;
      }

   /* Now do the alias trickery */
   if (push_alias(al->value, (char *)sarg->arg,
                  alias_h->trie, sarg->activator, TRUE) == 0)
   {
      /* Execute first of `derived' commands to avoid silly `command lag'
         * This is very ugly
         */
      if (!CHAR_DESCRIPTOR(sarg->activator)->qInput.IsEmpty())
      {
         class cQueueElem *qe =
             CHAR_DESCRIPTOR(sarg->activator)->qInput.GetHead();
         pcomm = (char *)qe->Data();
         qe->SetNull();
         delete qe;
      }
   }

   /* Put popped commands back on queue and clean up */
   while (i--)
   {
      CHAR_DESCRIPTOR(sarg->activator)->qInput.Append(new cQueueElem(cmd_array[i]));
   }

   /* Ok, now we can safely call command interpreter. */
   if (pcomm)
   {
      if (*(skip_spaces(pcomm)) == '!')
         send_to_char("Whoops, you just called an alias with '!' in it!<br/>",
                      sarg->activator);
      else
         command_interpreter(sarg->activator, pcomm);
      FREE(pcomm);
   }

   return SFR_BLOCK;
}

int dictionary(struct spec_arg *sarg)
{
   /* Recursive calls can occur via 'at':

       alias mail at hansen mail paper papi

       (since mail is in the "at" command!).
       So Gnort's routine wont be able to catch it. I'll make a double
       check here for no more than 100 nests. */

   static int recursive_calls = 0;
   int n = SFR_BLOCK;

   recursive_calls++;

   if (recursive_calls > 100)
      send_to_char("Alias is recursive, it has been terminated!<br/>",
                   sarg->activator);
   else
      n = local_dictionary(sarg);

   recursive_calls--;

   return n;
}
