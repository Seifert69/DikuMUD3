/*
 $Author: All $
 $RCSfile: color.h,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.4 $
 */

#ifndef _MUD_COLOR_H
#define _MUD_COLOR_H

#include <string>
#include "essential.h"
#include "values.h"

class color_type
{
private:
  int count;
  long key_size;   //how big are all the key's together for maloc purposes
  long color_size; //how big are all the color's together for maloc purposes
  char *keyword;
  char *color;
  color_type *next;
  color_type *current;

  void count_plus(char *key, char *c);
  void count_minus(char *key, char *c);

public:
  color_type(void);
  color_type(char *key, char *c);
  ~color_type(void);

  char *insert(char *key, char *c);
  void insert(char *combo);
  std::string change(char *key, char *c);
  void change(char *combo);
  const char *get(const char *key);
  const char *get(const char *key, char *full_key);
  int remove(char *key);
  void remove_all(void);
  void create(char *input_str);
  char *key_string(void);
  char *key_string(color_type &dft);
  char *save_string(void);
};

int is_forground(char *cstr);
int is_background(char *cstr);

#endif /* _MUD_COLOR_H */
