/*
 $Author: All $
 $RCSfile: textutil.h,v $
 $Date: 2003/11/29 03:31:16 $
 $Revision: 2.3 $
 */

#ifndef _MUD_TEXTUTIL_H
#define _MUD_TEXTUTIL_H

#include <string>
#include <cstring>

#define isaspace(c) ((c) == ' ')

extern const char *fillwords[];

int str_escape_format(const char *str, char *dest, int destlen, int format = TRUE);
char *str_escape_format(const char *src, int formatting = TRUE);
void str_correct_utf8(std::string &src);
char *html_encode_utf8(const char *src);
std::string scriptwrap(const char *str);
void str_cescape_format(const char *src, char *dest);
void str_cescape_transform(char *src);

char *catnames(char *s, char **names);
void str_insert(char *str, char *itxt);
void str_substitute(const char *old, const char *newstr, char *str);
void str_substitute(const std::string &search, const std::string &replace, std::string &subject);
void str_rem(char *s, char c);
void str_blank_punct(char *s);
void str_remspc(register char *s);
void str_chraround(char *str, char c);

const char *is_name_raw(const char *arg, char const *const *names); // MS2020 const char *names[])
char *is_name(char *arg, char const *const *names);     // arg will get double spaces zapped
//const char *is_name_raw(const char *arg, const char *names[]);
//const char *is_name(const char *arg, const char *names[]);

int str_lower(char *s);
int str_upper(char *s);
int str_lower(const char *s, char *d, int nBufSize);

char *spc(int n);
char *str_repeatchar(int n, char c);

char *str_line(const char *, char *);
#ifdef DOS
#define itoa(n) itoa_dos(n)
char *itoa_dos(int n);
#else
char *itoa(int n);
#endif
char *ltoa(long n);
int str_ccmp(const char *s, const char *d);
int str_nccmp(const char *s, const char *d, int n);
char *str_dup(const char *source);
char *str_str(const char *cs, const char *ct);
const char *str_cstr(const char *cs, const char *ct);
ubit1 str_is_empty(const char *arg);
ubit1 str_is_number(const char *str);
ubit1 next_word_is_number(const char *str);
char *one_argument(const char *argument, char *first_arg);
int search_block_abbrevs(const char *oarg, const char **list,
                         const char **end);
int search_block(const char *arg, const char **list, ubit1 exact);
int search_block_length(const char *arg, int length,
                        const char **list, ubit1 exact);
int fill_word(const char *argument);
ubit1 is_abbrev(const char *arg1, const char *arg2);
char *str_cc(const char *s1, const char *s2);
char **add_name(const char *name, char **namelist);

char **create_namelist(void);

void free_namelist(char **list);
char *str_ccmp_next_word(const char *buf, const char *next_word);

char *str_next_word_copy(const char *argument, char *first_arg);
char *str_next_word(const char *argument, char *first_arg);
void split_fi_ref(const char *str, char *zone, char *name);
char *skip_spaces(const char *);
char *skip_blanks(const char *string);
void strip_trailing_spaces(char *str);
void strip_trailing_blanks(char *str);

const char *divcolor(const char *colorstr);
int substHTMLTagClass(const char *pOldTag, const char *pAttr, const char *pNewVal, char *pNewTag, int nTagMax);
int getHTMLValue(const char *name, const char *p, char *pTag, int nTagMax);
const char *getHTMLTag(const char *p, char *pTag, int nTagMax);


std::string str_json(const char *lbl, ubit64 nInt);
std::string str_json(const char *lbl, const char *str);
std::string str_json(const char *lbl, const std::string &str);
std::string str_jsonencode(const char *str);
std::string str_json_encode_quote(const char *str);

int pwdcompare(const char *p1, const char *p2, int nMax);

#endif /* _MUD_TEXTUTIL_H */
