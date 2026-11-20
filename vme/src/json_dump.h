#pragma once

#include <string>

/**
 * vme -j [zone]: after the boot (every zone loaded and reset once, before the first tick), writes
 * the world, or one zone, as JSON lines to dump_<zone>.jsonl (dump_world.jsonl without a zone).
 * Returns the process exit code. boot_db() has checked that the zone exists.
 */
int dump_json(const std::string &zone);
