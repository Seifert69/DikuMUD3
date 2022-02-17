#pragma once

#include "essential.h"
#include "namelist.h"

#include <cstring>
#include <map>

class diltemplate;
class unit_data;
class zone_reset_cmd;
class file_index_type;

// Used for std::map in place of the old binary search
struct cmp_str
{
    bool operator()(char const *a, char const *b) const { return std::strcmp(a, b) < 0; }
};

/* A linked/sorted list of all the zones in the game */
class zone_type
{
    // TODO Once DIL is figured out - hopeful we can remove the need to give pointers to internal class members
    //      then all the *Ptr variant functions can go

    // TODO Add [[nodiscard]] to getters() once interface is stable
public:
    zone_type();
    ~zone_type();

    cNamelist &getCreators();

    const char *getName() const;
    char *getNamePtr();
    void setName(char *value);

    const char *getTitle() const;
    char **getTitlePtrPtr();
    void setTitle(char *value);

    const char *getNotes() const;
    char **getNotesPtrPtr();
    void setNotes(char *value);

    char **getHelpPtrPtr();
    void setHelp(char *value);

    const char *getFilename() const;
    char **getFilenamePtrPtr();
    void setFilename(char *value);

    // TODO stop exposing the external container
    const std::map<const char *, file_index_type *, cmp_str> &cgetFileIndexMap() const;
    std::map<const char *, file_index_type *, cmp_str> &getFileIndexMap();

    const zone_reset_cmd *cgetZoneResetCommands() const;
    zone_reset_cmd *getZoneResetCommands();
    void setZoneResetCommands(zone_reset_cmd *value);

    // TODO stop exposing the external container
    const std::map<const char *, diltemplate *, cmp_str> &cgetDILTemplate() const;
    std::map<const char *, diltemplate *, cmp_str> &getDILTemplate();

    void setZoneNumber(ubit16 value);

    ubit16 getNumOfFileIndexes() const;
    void incrementNumOfFileIndexes();
    void setNumOfFileIndexes(ubit16 value);

    ubit16 getZoneResetTime() const;
    void setZoneResetTime(ubit16 value);
    ubit16 *getZoneResetTimePtr();

    ubit16 getNumOfRooms() const;
    void incrementNumOfRooms();
    void setNumOfRooms(ubit16 value);

    ubit16 getNumOfObjects() const;
    void incrementNumOfObjects();

    ubit16 getNumOfNPCs() const;
    void incrementNumOfNPCs();

    ubit8 getResetMode() const;
    ubit8 *getResetModePtr();
    void setResetMode(ubit8 value);

    void incrementNumOfDILTemplates();
    void setNumOfDILTemplates(ubit16 value);

    ubit8 getAccessLevel() const;
    void setAccessLevel(ubit8 value);

    ubit8 getLevelRequiredToLoadItems() const;
    ubit8 *getLevelRequiredToLoadItemsPtr();
    void setLevelRequiredToLoadItems(ubit8 value);

    ubit8 getPayOnly() const;
    ubit8 *getPayOnlyPtr();
    void setPayOnly(ubit8 value);

    const char *getDILFilePath() const;
    void setDILFilePath(char *value);

    class Weather
    {
        // TODO this should be extracted to global namespace and the functions from weather.cpp moved here too
    public:
        int getPressure() const { return pressure; }
        int getChange() const { return change; }
        int getSky() const { return sky; }
        int getBase() const { return base; }

        void setBase(int value) { base = value; }
        void setChange(int value) { change = value; }
        void incrementChangeBy(int value) { change += value; }
        void decrementChangeBy(int value) { change -= value; }
        void setPressure(int value) { pressure = value; }
        void incrementPressureBy(int value) { pressure += value; }
        void setSky(int value) { sky = value; }

    private:
        int pressure{0}; // How is the pressure ( Mb )
        int change{0};   // How fast and what way does it change.
        int sky{0};      // How is the sky.
        int base{0};     // What is the basis pressure like?
    };
    const Weather &cgetWeather() const;
    Weather &getWeather();

private:
    cNamelist creators;                                        // List of creators of zone
    char *name{nullptr};                                       // Unique in list
    char *title{nullptr};                                      // A nice looking zone title
    char *notes{nullptr};                                      // Creator notes to zone
    char *help{nullptr};                                       // User-Help to zone
    char *filename{nullptr};                                   // The filename of this file
    unit_data *rooms{nullptr};                                 // unit pointer to the base rooms, used in vmc really
    unit_data *objects{nullptr};                               // unit pointer to the base objects, used in vmc really
    unit_data *npcs{nullptr};                                  // unit pointer to the base npcs, used in vmc really
    std::map<const char *, file_index_type *, cmp_str> mmp_fi; //
    zone_reset_cmd *zri{nullptr};                              // List of Zone reset commands
    std::map<const char *, diltemplate *, cmp_str> mmp_tmpl;   //
    ubit8 **spmatrix{nullptr};                                 // Shortest Path Matrix
    ubit16 zone_no{0};                                         // Zone index counter (spmatrix)
    ubit16 no_of_fi{0};                                        // Number of fi's in the list
    ubit16 zone_time{0};                                       // How often to reset zone
    ubit16 no_rooms{0};                                        // The number of rooms
    ubit16 no_objs{0};                                         //
    ubit16 no_npcs{0};                                         //
    ubit8 reset_mode{0};                                       // when/how to reset zone
    ubit16 no_tmpl{0};                                         // number of DIL templates
    ubit8 access{0};                                           // Access Level 0 = highest (root)
    ubit8 loadlevel{0};                                        // Level required to load items
    ubit8 payonly{0};                                          // TRUE when only 4 paying players
    char *dilfilepath{nullptr};                                //
    Weather weather;                                           //
};
