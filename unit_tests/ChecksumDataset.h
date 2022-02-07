#pragma once

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#include <filesystem>
#include <fstream>
#include <map>
#include <set>
#include <vector>

#include <boost/test/data/monomorphic.hpp>

struct FilePair
{
    std::filesystem::path filename;
    std::string sha512_checksum;
};

struct FileInfo
{
    std::string name;                   // Test set descriptive name
    FilePair source_file;               // Source filename and hash
    std::vector<FilePair> output_files; // Array output filenames and hashes
};

/**
 * Reads a json configuration file and creates a dataset of filenames and
 * checksums for boost::unit_test. The test run can alter the dataset
 * eg. remove non-existent files, update hash changed files and then
 * on destruction it will update the json configuration
 */
class ChecksumDataset
{
    using Container = std::map<std::filesystem::path, FileInfo>;

public:
    using value_type = Container::value_type;
    using iterator = Container::iterator;
    using size_type = Container::size_type;

    iterator begin() { return m_files.begin(); }
    iterator end() { return m_files.end(); }
    size_type size() { return m_files.size(); }

    explicit ChecksumDataset(const std::string &json_filename);
    ~ChecksumDataset();

    /**
     * File doesn't exist remove it from dataset before saving json
     * @param it Source file to remove
     */
    iterator RemoveSourceFile(iterator it);

    /**
     * Checksums have been updated - save the updated json on destruction
     */
    void SetChanged();

    /**
     * An error occured - cancel saving updated json
     */
    void CancelChanged();

    /**
     * Removes the project path and the relative path from a filename
     * eg.
     * "/home/user/code/DikuMUD3/vme/zone/udgaard.zon"
     * becomes
     * "udgaard.zon"
     * @param filename Filename to shorten
     * @return Filename with path stripped
     */
    std::string StripFullPath(const std::filesystem::path &filename);

private:
    [[nodiscard]] FilePair extract_file_pair_from_json(const rapidjson::Value &value) const;
    void load_json();
    void parser_header();
    void parse_new_file_check_section();
    void parse_files_section();
    void process_new_files();
    Container create_list_new_source_files();
    void write_json();

    static const std::filesystem::path m_project_root;   // Full path to source code project root - set from CMakeLists
    bool m_changed{false};                               // Has a boost test case updated a hash/fileset
    std::filesystem::path m_json_filename;               // Full path filename to json config file
    rapidjson::Document m_json;                          // Parsed JSON config file
    std::string m_name;                                  // Descriptive name of config file
    std::filesystem::path m_relative_path;               // Path from project root to data files eg vme/zones
    std::set<std::filesystem::path> m_skip;              // Files to skip over when searching for new sources eg color.dat
    std::filesystem::path m_source_extension;            // Extension of a source file eg .zon
    std::set<std::filesystem::path> m_output_extensions; // Extensions of compiled files eg .data, .reset
    Container m_files;                                   // Container of all files and hashes from config + new files
};
