#pragma once

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#include <filesystem>
#include <fstream>
#include <vector>

#include <boost/test/data/monomorphic.hpp>

struct FileInfo
{
    std::string error_fmt_str;
    std::string full_filename;
    std::string sha512_checksum;
};

std::ostream &operator<<(std::ostream &os, const FileInfo &fi);

class checksum_dataset : public std::vector<FileInfo>
{
public:
    enum
    {
        arity = 1 // boost dataset requires this
    };

    checksum_dataset(const std::string &json_filename);

    ~checksum_dataset() = default;
};

namespace boost::unit_test::data::monomorphic
{
template<>
struct is_dataset<checksum_dataset> : boost::mpl::true_
{
};
} // namespace boost::unit_test::data::monomorphic
