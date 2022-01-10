
/*
 $Author: All $
 $RCSfile: config.cpp,v $
 $Date: 2004/09/18 19:52:56 $
 $Revision: 2.5 $
 */
#include "config.h"

#include "color.h"
#include "db.h"
#include "diku_exception.h"
#include "essential.h"
#include "files.h"
#include "str_parse.h"
#include "structs.h"
#include "textutil.h"
#include "utility.h"
#include "values.h"

#ifdef _WINDOWS
    #include <io.h>
    #include <winsock2.h>
#endif
#include <cstring>
#include <filesystem>

#include <boost/format.hpp>

CServerConfiguration g_cServerConfig;

auto CServerConfiguration::FromLAN(const char *pFromHost) const -> bool
{
    in_addr sTmp{};

#ifdef _WINDOWS
    if (inet_addr(pFromHost) == INADDR_NONE)
    {
        slog(LOG_ALL, 0, "Localhost invalid.");
        return false;
    }
#else
    if (inet_aton(pFromHost, &sTmp) == 0)
    {
        slog(LOG_ALL, 0, "Localhost [%s] invalid.", pFromHost);
        return false;
    }
#endif

    return ((m_sSubnetMask.s_addr & m_sLocalhost.s_addr) == (m_sSubnetMask.s_addr & sTmp.s_addr));
}

auto CServerConfiguration::ValidMplex(const sockaddr_in *isa) const -> bool
{
    return std::any_of(m_aMplexHosts.begin(),
                       m_aMplexHosts.end(),
                       [&isa](const in_addr &mplex) { return isa->sin_addr.s_addr == mplex.s_addr; });
}

void CServerConfiguration::Boot(const std::string &srvcfg)
{
    char Buf[2 * MAX_STRING_LENGTH]{};
    char *c{};
    int i{};

    slog(LOG_OFF, 0, "Booting server.");

    if (!file_exists(srvcfg))
    {
        slog(LOG_ALL, 0, "Could not find server configuration file. %s", srvcfg.c_str());
        throw diku_exception{FPFL} << "Could not find server configuration file. [" << srvcfg << "]";
    }

    config_file_to_string(srvcfg, Buf, sizeof(Buf));

    c = Buf;

    m_mudname = parse_match_name((const char **)&c, "mudname", "No Mud server name in server.cfg");
    if (m_mudname.length() > 49)
    {
        m_mudname.resize(50);
    }
    slog(LOG_ALL, 0, "The Mud Name is %s.", m_mudname.c_str());

    m_pImmortName = parse_match_name((const char **)&c, "immortal_name", "immortal");
    if (m_pImmortName.length() > PC_MAX_NAME)
    {
        m_pImmortName.resize(PC_MAX_NAME);
    }

    m_libdir = parse_match_name((const char **)&c, "libdir", "../lib/");
    checkDirectoryExists("lib", m_libdir);

    m_etcdir = parse_match_name((const char **)&c, "etcdir", "../etc/");
    checkDirectoryExists("etc", m_etcdir);

    m_logdir = parse_match_name((const char **)&c, "logdir", "../log/");
    checkDirectoryExists("log", m_logdir);

    m_zondir = parse_match_name((const char **)&c, "zondir", "../zone/");
    checkDirectoryExists("zone", m_zondir);

    m_plydir = parse_match_name((const char **)&c, "plydir", "../lib/ply/");
    checkDirectoryExists("player", m_plydir);

    m_dilfiledir = parse_match_name((const char **)&c, "dil_file_dir", "../lib/file/");
    checkDirectoryExists("dil", m_dilfiledir);

    if (parse_match_num((const char **)&c, "Port", &i))
    {
        m_nMotherPort = i;
    }

    if (!is_in(m_nMotherPort, 2000, 8000))
    {
        slog(LOG_ALL, 0, "Mother port not in [2000..8000].");
        throw diku_exception(FPFL) << "Mother port " << m_nMotherPort << " not in [2000..8000].";
    }

    if (parse_match_num((const char **)&c, "auto_reboot_hour", &i))
    {
        m_hReboot = i;
    }

    if (!((m_hReboot >= 0) && (m_hReboot <= 24)))
    {
        m_hReboot = 0;
        slog(LOG_ALL, 0, "Auto Reboot Set to an invaild hour, disabling.");
    }

    if (parse_match_num((const char **)&c, "Rent", &i))
    {
        m_nRentModifier = i;
    }

    if (!is_in(m_nRentModifier, 0, 100))
    {
        slog(LOG_ALL, 0, "Rent modifier not in [0..100].");
        throw diku_exception(FPFL) << "Rent modifier not in [0..100].";
    }

    if (parse_match_num((const char **)&c, "BOB", &i))
    {
        m_bBOB = (i != 0);
    }

    if (!is_in(i, 0, 1))
    {
        slog(LOG_ALL, 0, "BOB not 0 or 1");
        throw diku_exception(FPFL) << "BOB not 0 or 1";
    }

    if (parse_match_num((const char **)&c, "Alias Shout", &i))
    {
        m_bAliasShout = (i != 0);
    }

    if (!is_in(i, 0, 1))
    {
        slog(LOG_ALL, 0, "Alias Shout not 0 or 1");
        throw diku_exception(FPFL) << "Alias Shout not 0 or 1";
    }

    if (parse_match_num((const char **)&c, "No Specials", &i))
    {
        m_bNoSpecials = i;
    }

    if (!is_in(m_bNoSpecials, 0, 1))
    {
        slog(LOG_ALL, 0, "Specials not 0 or 1");
        throw diku_exception(FPFL) << "Specials not 0 or 1";
    }

    if (parse_match_num((const char **)&c, "Lawful", &i))
    {
        m_bLawful = (i != 0);
    }

    if (!is_in(i, 0, 1))
    {
        slog(LOG_ALL, 0, "Lawful not 0 or 1");
        throw diku_exception(FPFL) << "Lawful not 0 or 1";
    }

    if (parse_match_num((const char **)&c, "BBS", &i))
    {
        m_bBBS = (i != 0);
    }

    if (!is_in(i, 0, 1))
    {
        slog(LOG_ALL, 0, "BBS not 0 or 1");
        throw diku_exception(FPFL) << "BBS not 0 or 1";
    }

    if (parse_match_num((const char **)&c, "Accounting", &i))
    {
        m_bAccounting = (i != 0);
    }

    if (!is_in(i, 0, 1))
    {
        slog(LOG_ALL, 0, "Accounting not 0 or 1");
        throw diku_exception(FPFL) << "Accounting not 0 or 1";
    }

    if (parse_match_num((const char **)&c, "Shout", &i))
    {
        m_nShout = i;
    }

    if (!is_in(m_nShout, 0, 255))
    {
        slog(LOG_ALL, 0, "Shout level not [0..255]");
        throw diku_exception(FPFL) << "Shout level not [0..255]";
    }

    auto subnet = parse_match_name((const char **)&c, "subnetmask", "255.255.255.255");
    m_sSubnetMask = stringToIPAddress(subnet, "SubnetMask [%s] invalid.");

    auto localhost = parse_match_name((const char **)&c, "localhost", "127.0.0.1");
    m_sLocalhost = stringToIPAddress(localhost, "Localhost [%s] invalid.");

    auto ppNames = parse_match_namelist((const char **)&c, "mplex hosts");

    if (ppNames.empty())
    {
        slog(LOG_ALL, 0, "Mplex hosts must be specified.");
        throw diku_exception(FPFL) << "Mplex hosts must be specified.";
    }

    if (ppNames.size() > MAX_MPLEX_HOSTS)
    {
        slog(LOG_ALL, 0, "10 is maximum number of mplex hosts.");
        throw diku_exception(FPFL) << "10 is maximum number of mplex hosts.";
    }

    for (size_t idx = 0; idx < MAX_MPLEX_HOSTS; ++idx)
    {
        if (idx < ppNames.size())
        {
            m_aMplexHosts[idx] = stringToIPAddress(ppNames[idx], "Mplex host [%s] invalid IP.");
        }
        else
        {
            m_aMplexHosts[idx] = m_aMplexHosts[idx - 1];
        }
    }

    m_promptstr = parse_match_name((const char **)&c, "promptstr", "");

    slog(LOG_OFF, 0, "Reading info and configuration files.");

    slog(LOG_OFF, 0, "Reading in etc / colors.");
    touch_file(getFileInEtcDir(COLOR_FILE));
    char *tmp2 = nullptr;
    tmp2 = read_info_file(getFileInEtcDir(COLOR_FILE), tmp2);
    m_pColor = tmp2;
    FREE(tmp2);
    color.create(m_pColor);

    slog(LOG_OFF, 0, "Reading in etc / logo.");
    touch_file(getFileInEtcDir(LOGO_FILE));

    char *tmp = nullptr;
    tmp = read_info_file(getFileInEtcDir(LOGO_FILE), tmp);
    m_pLogo = tmp;
    FREE(tmp);
}

auto CServerConfiguration::getMotherPort() const -> int
{
    return m_nMotherPort;
}

auto CServerConfiguration::getRentModifier() const -> int
{
    return m_nRentModifier;
}

auto CServerConfiguration::isAccounting() const -> bool
{
    return m_bAccounting;
}

auto CServerConfiguration::isAliasShout() const -> bool
{
    return m_bAliasShout;
}

auto CServerConfiguration::isBBS() const -> bool
{
    return m_bBBS;
}

auto CServerConfiguration::isLawful() const -> bool
{
    return m_bLawful;
}

auto CServerConfiguration::isNoSpecials() const -> bool
{
    return m_bNoSpecials;
}

auto CServerConfiguration::isBOB() const -> bool
{
    return m_bBOB;
}

auto CServerConfiguration::getShout() const -> int
{
    return m_nShout;
}

auto CServerConfiguration::getReboot() const -> int
{
    return m_hReboot;
}

auto CServerConfiguration::getColorType() const -> const color_type &
{
    return color;
}

auto CServerConfiguration::getSubnetMask() const -> const in_addr &
{
    return m_sSubnetMask;
}

auto CServerConfiguration::getLocalhost() const -> const in_addr &
{
    return m_sLocalhost;
}

auto CServerConfiguration::getPromptString() const -> const std::string &
{
    return m_promptstr;
}

auto CServerConfiguration::getLibDir() const -> const std::string &
{
    return m_libdir;
}

auto CServerConfiguration::getFileInLibDir(const std::string &filename) const -> std::string
{
    return {m_libdir + filename};
}

auto CServerConfiguration::getFileInLibDir(const char *filename) const -> std::string
{
    if (!filename)
    {
        throw diku_exception(FPFL) << "Null pointer passed";
    }
    return getFileInLibDir(std::string{filename});
}

void CServerConfiguration::checkDirectoryExists(const std::string &name, const std::string &directory)
{
    if (!(std::filesystem::exists(directory) && std::filesystem::is_directory(directory)))
    {
        slog(LOG_ALL, 0, "The %s directory %s does not exist.", name.c_str(), directory.c_str());
        throw diku_exception(FPFL) << boost::format("The %s directory %s does not exist.") % name % directory;
    }
    slog(LOG_ALL, 0, "The %s directory is %s.", name.c_str(), directory.c_str());
}

auto CServerConfiguration::getPlyDir() const -> const std::string &
{
    return m_plydir;
}

auto CServerConfiguration::getEtcDir() const -> const std::string &
{
    return m_etcdir;
}

auto CServerConfiguration::getFileInEtcDir(const std::string &filename) const -> std::string
{
    return {m_etcdir + filename};
}

auto CServerConfiguration::getFileInEtcDir(const char *filename) const -> std::string
{
    if (!filename)
    {
        throw diku_exception(FPFL) << "Null pointer passed";
    }
    return getFileInEtcDir(std::string{filename});
}

auto CServerConfiguration::getLogDir() const -> const std::string &
{
    return m_logdir;
}

auto CServerConfiguration::getFileInLogDir(const std::string &filename) const -> std::string
{
    return {m_logdir + filename};
}

auto CServerConfiguration::getFileInLogDir(const char *filename) const -> std::string
{
    if (!filename)
    {
        throw diku_exception(FPFL) << "Null pointer passed";
    }
    return getFileInLogDir(std::string{filename});
}

auto CServerConfiguration::getZoneDir() const -> const std::string &
{
    return m_zondir;
}

auto CServerConfiguration::getDILFileDir() const -> const std::string &
{
    return m_dilfiledir;
}

auto CServerConfiguration::getMudName() const -> const std::string &
{
    return m_mudname;
}

auto CServerConfiguration::getLogo() const -> const std::string &
{
    return m_pLogo;
}

auto CServerConfiguration::getColorString() const -> const std::string &
{
    return m_pColor;
}

auto CServerConfiguration::getImmortalName() const -> const std::string &
{
    return m_pImmortName;
}

auto CServerConfiguration::parse_match_name(const char **pData, const char *pMatch, std::string default_value) -> std::string
{
    auto *match = ::parse_match_name(pData, pMatch);
    if (match == nullptr)
    {
        return default_value;
    }
    std::string retval{match};
    FREE(match);
    return retval;
}

auto CServerConfiguration::parse_match_namelist(const char **pData, const char *pMatch) -> std::vector<std::string>
{
    auto *ppNames = ::parse_match_namelist(pData, pMatch);
    std::vector<std::string> retval;

    if (ppNames == nullptr)
    {
        return retval;
    }

    int l = 0;
    while (ppNames[l])
    {
        retval.emplace_back(std::string{ppNames[l]});
        l++;
    }
    free_namelist(ppNames);

    return retval;
}

auto CServerConfiguration::getMplexHosts() const -> const std::vector<in_addr> &
{
    return m_aMplexHosts;
}

auto CServerConfiguration::getMplexHost(size_t index) -> const in_addr &
{
    return m_aMplexHosts[index];
}

auto CServerConfiguration::stringToIPAddress(const std::string &ip_address, const std::string &error_msg) -> in_addr
{
    in_addr retval{};
#ifdef _WINDOWS
    retval.S_un.S_addr = inet_addr(ip_address.c_str());
    if (retval.S_un.S_addr == INADDR_NONE)
    {
        slog(LOG_ALL, 0, error_msg.c_str(), ip_address.c_str());
        throw diku_exception(FPFL) << boost::format(error_msg) % ip_address;
    }
#else
    if (inet_aton(ip_address.c_str(), &retval) == 0)
    {
        slog(LOG_ALL, 0, error_msg.c_str(), ip_address.c_str());
        throw diku_exception(FPFL) << boost::format(error_msg) % ip_address;
    }
#endif

    return retval;
}
