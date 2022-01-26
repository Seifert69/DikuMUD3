/*
 $Author: All $
 $RCSfile: config.h,v $
 $Date: 2004/01/09 20:34:23 $
 $Revision: 2.4 $
 */
#pragma once

#ifdef _WINDOWS
    #include <winsock.h>
#else
    #include <arpa/inet.h>
#endif
#include "color.h"
#include "common.h"
#include "fight.h"

#include <map>
#include <vector>

class CServerConfiguration
{
public:
    CServerConfiguration() = default;
    CServerConfiguration(const CServerConfiguration &) = delete;
    CServerConfiguration(CServerConfiguration &&) = delete;
    CServerConfiguration &operator=(const CServerConfiguration &) = delete;
    CServerConfiguration &operator=(CServerConfiguration &&) = delete;
    ~CServerConfiguration() = default;

    static constexpr auto MAX_MPLEX_HOSTS = 10U;

    void Boot(const std::string &srvcfg);

    [[nodiscard]] bool FromLAN(const char *pFromHost) const;
    [[nodiscard]] bool ValidMplex(const sockaddr_in *isa) const;

    [[nodiscard]] bool isAccounting() const;
    [[nodiscard]] bool isAliasShout() const;
    [[nodiscard]] bool isBBS() const;
    [[nodiscard]] bool isBOB() const;
    [[nodiscard]] bool isLawful() const;
    [[nodiscard]] bool isNoSpecials() const;

    [[nodiscard]] const std::string &getColorString() const;
    [[nodiscard]] const color_type &getColorType() const;
    [[nodiscard]] const std::string &getDILFileDir() const;
    [[nodiscard]] const std::string &getEtcDir() const;
    [[nodiscard]] const std::string &getImmortalName() const;
    [[nodiscard]] const std::string &getLibDir() const;
    [[nodiscard]] const in_addr &getLocalhost() const;
    [[nodiscard]] const std::string &getLogDir() const;
    [[nodiscard]] const std::string &getLogo() const;
    [[nodiscard]] int getMotherPort() const;
    [[nodiscard]] const std::vector<in_addr> &getMplexHosts() const;
    [[nodiscard]] const in_addr &getMplexHost(size_t index);
    [[nodiscard]] const std::string &getMudName() const;
    [[nodiscard]] const std::string &getPlyDir() const;
    [[nodiscard]] const std::string &getPromptString() const;
    [[nodiscard]] int getReboot() const;
    [[nodiscard]] int getRentModifier() const;
    [[nodiscard]] int getShout() const;
    [[nodiscard]] const in_addr &getSubnetMask() const;
    [[nodiscard]] const std::string &getZoneDir() const;

    void enableAccounting();
    void disableAccounting();
    /**
     * Generates a string of the path with the filename appended
     *
     * @param filename Filename to append to path
     * @return Returns Path with filename appended
     * @{
     */
    [[nodiscard]] std::string getFileInEtcDir(const std::string &filename) const;
    [[nodiscard]] std::string getFileInLibDir(const std::string &filename) const;
    [[nodiscard]] std::string getFileInLogDir(const std::string &filename) const;
    /** }@ */

    /**
     * Same as above, but designed to catch nullptr's
     * @param filename Filename to append to path
     * @return Returns Path with filename appended
     * @throw diku_exception
     * @{
     */
    [[nodiscard]] std::string getFileInEtcDir(const char *filename) const;
    [[nodiscard]] std::string getFileInLibDir(const char *filename) const;
    [[nodiscard]] std::string getFileInLogDir(const char *filename) const;
    /** }@ */
private:
    static std::string parse_match_name(const char **pData, const char *pMatch, std::string default_value);
    static std::vector<std::string> parse_match_namelist(const char **pData, const char *pMatch);
    /**
     * Convert IP address in string to in_addr
     * @param ip_address
     * @param error_msg
     * @return Converted IP address
     * @throw diku_exception on failure to convert
     */
    static in_addr stringToIPAddress(const std::string &ip_address, const std::string &error_msg);
    /**
     * Checks to see if directory exists
     * @param name Name to appear in the exception texts eg 'lib'
     * @param directory Name of directory1
     * @throw diku_exception
     */
    static void checkDirectoryExists(const std::string &name, const std::string &directory);

    int m_nMotherPort{4999};                             // TCP port number
    int m_nRentModifier{10};                             //
    bool m_bAccounting{false};                           //
    bool m_bAliasShout{true};                            //
    bool m_bBBS{false};                                  //
    bool m_bLawful{false};                               // Unused apart from unit_tests so far
    bool m_bNoSpecials{false};                           //
    bool m_bBOB{false};                                  //
    int m_nShout{1};                                     // Unused apart from unit_tests so far
    int m_hReboot{0};                                    // Hour 0-24 to reboot server on
    color_type color{};                                  //
    in_addr m_sSubnetMask{};                             // Unused apart from unit_tests so far
    in_addr m_sLocalhost{};                              // Unused apart from unit_tests so far
    std::vector<in_addr> m_aMplexHosts{MAX_MPLEX_HOSTS}; // Unused apart from unit_tests so far
    std::string m_promptstr{};                           // Unused apart from unit_tests so far
    std::string m_libdir{};                              // The path to lib directory
    std::string m_plydir{};                              // The path to player directory
    std::string m_etcdir{};                              // The path to etc directory
    std::string m_logdir{};                              // The path to log directory
    std::string m_zondir{};                              // The path to zone directory
    std::string m_dilfiledir{};                          // The path to dil file director
    std::string m_mudname{};                             // The mud name
    std::string m_pLogo{};                               // Intro screen
    std::string m_pColor{};                              //
    std::string m_pImmortName{};                         // Name of the Immortal of the mud
};

extern CServerConfiguration g_cServerConfig;
