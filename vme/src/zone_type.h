#pragma once

#include "essential.h"
#include "file_index_type.h"
#include "namelist.h"
#include "weather.h"

#include <cstring>
#include <map>
#include <memory>

struct diltemplate;
class unit_data;
class zone_reset_cmd;

/**
 * A linked/sorted list of all the zones in the game
 */
class zone_type
{
    using FileIndexMap = std::map<std::string, std::unique_ptr<file_index_type>>;
    using DILTemplateMap = std::map<std::string, std::unique_ptr<diltemplate>>;

    class PtrPtrType
    {
    public:
        PtrPtrType() = default;
        explicit PtrPtrType(std::string value)
            : m_value(std::move(value))
        {
        }
        PtrPtrType &operator=(std::string value)
        {
            m_value = std::move(value);
            return *this;
        }
        [[nodiscard]] const std::string &getValue() const { return m_value; }
        char **getValuePtr()
        {
            m_value_ptr = m_value.data();
            return &m_value_ptr;
        }

    private:
        std::string m_value;
        char *m_value_ptr{nullptr};
    };

public:
    zone_type(std::string name, std::string filename);
    ~zone_type();

    cNamelist &getCreators();

    [[nodiscard]] const std::string &getName() const;
    char *getNamePtr();

    [[nodiscard]] const std::string &getTitle() const;
    char **getTitlePtrPtr();
    void setTitle(std::string value);

    [[nodiscard]] const std::string &getNotes() const;
    char **getNotesPtrPtr();
    void setNotes(std::string value);

    char **getHelpPtrPtr();
    void setHelp(std::string value);

    [[nodiscard]] const std::string &getFilename() const;
    char **getFilenamePtrPtr();

    /**
     * Extracted from extra_stat_zone()
     * @param search_type Unit_data type
     * @return Formatted message
     */
    [[nodiscard]] std::string getExtraStatZoneMessage(int search_type) const;
    void insertFileIndex(std::unique_ptr<file_index_type> &&value);
    /**
     * Extracted from find_file_index()
     * @param name File index name
     * @return file_index_type or nullptr
     */
    file_index_type *findFileIndex(const std::string &name);

    [[nodiscard]] const zone_reset_cmd *cgetZoneResetCommands() const;
    zone_reset_cmd *getZoneResetCommands();
    void setZoneResetCommands(zone_reset_cmd *value);

    [[nodiscard]] size_t getNumOfFileIndexes() const;

    [[nodiscard]] ubit16 getZoneResetTime() const;
    void setZoneResetTime(ubit16 value);
    ubit16 *getZoneResetTimePtr();

    [[nodiscard]] ubit16 getNumOfRooms() const;
    void incrementNumOfRooms();
    void setNumOfRooms(ubit16 value);

    [[nodiscard]] ubit16 getNumOfObjects() const;
    void incrementNumOfObjects();

    [[nodiscard]] ubit16 getNumOfNPCs() const;
    void incrementNumOfNPCs();

    [[nodiscard]] ubit8 getResetMode() const;
    ubit8 *getResetModePtr();
    void setResetMode(ubit8 value);

    [[nodiscard]] ubit8 getAccessLevel() const;
    void setAccessLevel(ubit8 value);

    [[nodiscard]] ubit8 getLevelRequiredToLoadItems() const;
    ubit8 *getLevelRequiredToLoadItemsPtr();
    void setLevelRequiredToLoadItems(ubit8 value);

    [[nodiscard]] ubit8 getPayOnly() const;
    ubit8 *getPayOnlyPtr();
    void setPayOnly(ubit8 value);

    [[nodiscard]] const std::optional<std::string> &getDILFilePath() const;
    void setDILFilePath(std::string value);

    [[nodiscard]] const Weather &cgetWeather() const;
    [[nodiscard]] Weather &getWeather();

    /**
     * Extracted from read_all_rooms()
     */
    void readAllUnitRooms();

    /**
     * Extracted from dilfld case DILF_ROOMS:
     * @return nullptr or the found unit_data
     */
    unit_data *findFirstRoom();

    /**
     * Extract from dilfld case DILF_NPCS:
     * @return nullptr or the found unit_data
     */
    unit_data *findFirstNPC();

    /**
     * Extract from dilfld case DILF_OBJS:
     * @return nullptr or the found unit_data
     */
    unit_data *findFirstObj();

    /**
     * Extracted from assign_player_file_index()
     * Finds an existing file index for a player or if not creates a new one
     * @param name Filename of the player file
     * @return A file_index_type
     */
    file_index_type *findOrCreateFileIndex(const char *name);

    /**
     * Extracted from zone_update_no_in_zone()
     */
    void updateNumberInZone();

    /**
     * Extracted from stat_dil()
     * @return std::string Formatted message
     */
    [[nodiscard]] std::string getStatDIL() const;

    /**
     * Extracted from stat_global_dil()
     * @param nCount
     * @param instructionSum Running total of all instructions
     * @return std::string Formatted message
     */
    [[nodiscard]] std::string getStatGlobalDIL(ubit32 nCount, ubit64 &instructionSum) const;

    /**
     * Extracted from resolve_templates()
     */
    void resolveZoneTemplates();

    /**
     *
     * @param value Template to insert
     */
    void insertDILTemplate(std::unique_ptr<diltemplate> &&value);

    /**
     * Extracted from find_dil_index()
     * Find template by name
     * @param name
     * @return template or nullptr
     */
    diltemplate *findDILTemplate(const std::string &name);

private:
    unit_data *findFirstUnitOfType(int type);

    cNamelist m_creators;                     ///< List of creators of zone
    std::string m_name;                       ///< Unique in list
    PtrPtrType m_title;                       ///< A nice looking zone title
    PtrPtrType m_notes;                       ///< Creator notes to zone
    PtrPtrType m_help;                        ///< User-Help to zone
    PtrPtrType m_filename;                    ///< The filename of this file
    unit_data *m_rooms{nullptr};              ///< unit pointer to the base rooms, used in vmc really
    unit_data *m_objects{nullptr};            ///< unit pointer to the base objects, used in vmc really
    unit_data *m_npcs{nullptr};               ///< unit pointer to the base npcs, used in vmc really
    FileIndexMap m_mmp_fi;                    ///<
    zone_reset_cmd *m_zri{nullptr};           ///< List of Zone reset commands
    DILTemplateMap m_mmp_tmpl;                ///<
    ubit8 **m_spmatrix{nullptr};              ///< Shortest Path Matrix
    ubit16 m_zone_time{0};                    ///< How often to reset zone
    ubit16 m_no_rooms{0};                     ///< The number of rooms
    ubit16 m_no_objs{0};                      ///<
    ubit16 m_no_npcs{0};                      ///<
    ubit8 m_reset_mode{0};                    ///< when/how to reset zone
    ubit8 m_access{255};                      ///< Access Level 0 = highest (root)
    ubit8 m_loadlevel{0};                     ///< Level required to load items
    ubit8 m_payonly{0};                       ///< TRUE when only 4 paying players
    std::optional<std::string> m_dilfilepath; ///<
    Weather m_weather;                        ///<
};
