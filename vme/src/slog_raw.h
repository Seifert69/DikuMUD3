#pragma once

#include "essential.h"

#include <string>

void slog_raw(log_level level, ubit8 wizinv_level, const std::string &msg);

/**
 * While one exists, log lines from its thread are counted instead of written.
 * For reads a player can repeat that would log the same lines each time, such
 * as "when" reading a corrupt player file: the log stops the server when it
 * grows too big.
 */
class cQuietLog
{
public:
    cQuietLog();
    ~cQuietLog();
    cQuietLog(const cQuietLog &) = delete;
    cQuietLog &operator=(const cQuietLog &) = delete;

    /// How many lines weren't written since this was made
    unsigned Count() const;

private:
    unsigned m_start;
};
