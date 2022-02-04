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
        for (auto &file_data : file_set["files"].GetArray())
        {
            auto full_filename = file_set["relative_path"].Get<std::string>();
            full_filename += file_data["filename"].Get<std::string>();

            emplace_back(FileInfo{file_set["error_fmt_str"].Get<std::string>(), full_filename, file_data["sha512"].Get<std::string>()});
        }
    }
    std::cout << "Loaded " << size() << " checksums from " << json_filename << " to check\n";
}

std::ostream &operator<<(std::ostream &os, const FileInfo &fi)
{
    os << fi.full_filename;
    return os;
}
