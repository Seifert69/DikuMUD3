#include "checksum_dataset.h"

#include <iostream>

checksum_dataset::checksum_dataset(const std::string &json_filename)
{
    std::ifstream in(json_filename);
    if (!in.good())
    {
        throw std::logic_error("Failed to open file: " + json_filename);
    }
    rapidjson::IStreamWrapper inw(in);
    rapidjson::Document doc;
    doc.ParseStream(inw);

    for (auto &file_set : doc.GetArray())
    {
        auto relative_path = file_set["relative_path"].Get<std::string>();
        auto error_fmt_str = file_set["error_fmt_str"].Get<std::string>();

        auto directory_listing = create_directory_listing(relative_path, file_set["new_file_check"]);

        for (auto &file_data : file_set["files"].GetArray())
        {
            auto filename = file_data["filename"].Get<std::string>();
            auto full_filename = relative_path + filename;

            emplace_back(FileInfo{error_fmt_str, full_filename, file_data["sha512"].Get<std::string>()});

            // This file has an entry in the json so remove it from the list
            directory_listing.erase(filename);
        }

        for (auto &new_file : directory_listing)
        {
            // All that should be left are new files without sha512's in the json config
            // so add a fake sha512 for them so the tests fail
            auto full_filename = relative_path + new_file;
            emplace_back(FileInfo{error_fmt_str, full_filename, "New file! Update sha512 in json"});
        }
    }

    std::cout << "Loaded " << size() << " checksums from " << json_filename << " to check\n";
}

/*
 * This is what the json looks like rapidjson::Value
 *
 * {
 * "files_to_skip": [],
 * "file_extensions": [
 *   ".data",
 *   ".reset"
 * ]
 * },
 */
std::set<std::string> checksum_dataset::create_directory_listing(const std::string &relative_path, const rapidjson::Value &value)
{
    std::set<std::string> skip;
    for (auto &filename : value["files_to_skip"].GetArray())
    {
        skip.insert(filename.Get<std::string>());
    }

    std::set<std::string> file_extensions;
    for (auto &ext : value["file_extensions"].GetArray())
    {
        file_extensions.insert(ext.Get<std::string>());
    }

    std::set<std::string> result;
    for (const auto &dir_entry : std::filesystem::directory_iterator{relative_path})
    {
        // Is it a file
        if (!dir_entry.is_regular_file())
        {
            continue;
        }

        auto filename = dir_entry.path().filename();

        // Does the file extension match
        if (file_extensions.find(filename.extension()) == file_extensions.end())
        {
            continue;
        }

        // Should it be skipped
        if (skip.find(filename) != skip.end())
        {
            continue;
        }

        result.insert(filename);
    }
    return result;
}

////////////////////////// Helper Printer for Boost ///////////////////////////////////
std::ostream &operator<<(std::ostream &os, const FileInfo &fi)
{
    os << fi.full_filename;
    return os;
}
