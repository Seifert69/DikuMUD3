#pragma once

#include <string>

int pipeMUD_read(std::string &str);
void namedpipe_setup();
void pipeMUD_dispatch(std::string str);
void pipeMUD_write(const char *str);
