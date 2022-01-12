#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace
{
inline std::string get_compiled_hash_defines()
{
    std::vector<std::string> hash_defines;
#ifdef DMSERVER
    hash_defines.emplace_back("DMSERVER");
#endif
#ifdef POSIX
    hash_defines.emplace_back("POSIX");
#endif
#ifdef DEBUG_HISTORY
    hash_defines.emplace_back("DEBUG_HISTORY");
#endif
#ifdef VMC
    hash_defines.emplace_back("VMC");
#endif
#ifdef VMC_SRC
    hash_defines.emplace_back("VMC_SRC");
#endif
#ifdef LINUX
    hash_defines.emplace_back("LINUX");
#endif
#ifdef UNIT_TYPE_DEBUG
    hash_defines.emplace_back("UNIT_TYPE_DEBUG");
#endif
#ifdef SUSPEKT
    hash_defines.emplace_back("SUSPEKT");
#endif
#ifdef MUD_DEBUG
    hash_defines.emplace_back("MUD_DEBUG");
#endif
#ifdef _WINDOWS
    hash_defines.emplace_back("_WINDOWS");
#endif
#ifdef MPLEX_COMPILE
    hash_defines.emplace_back("MPLEX_COMPILE");
#endif
#ifdef MEMORY_DEBUG
    hash_defines.emplace_back("MEMORY_DEBUG");
#endif

    std::ostringstream strm;
    std::string space;
    for (auto &define : hash_defines)
    {
        strm << space << "-D" << define;
        space = " ";
    }
    return strm.str();
}
} // namespace
