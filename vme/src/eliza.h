#pragma once

#include "essential.h"
#include "interpreter.h"

#include <cstddef>

/// buf in oracle(), where preprocess_string() rewrites a command argument (shorter than MAX_INPUT_LENGTH) in place.
/// It pads punctuation ("!" becomes " ! ") and grows words ("id " becomes "i would "); the worst repeated text
/// found ("!id") grows 10/3 times, so this allows 4 times.
constexpr size_t ORACLE_BUF_SIZE = 4 * MAX_INPUT_LENGTH;

void delayed_action(void *p1, void *p2);
void eliza_boot();
void preprocess_string(char *str, const char *own_name);
int oracle(spec_arg *sarg);
