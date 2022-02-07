#include "ChecksumDataset.h"

#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

#include <iostream>

// PROJECT_ROOT is a command line #define set in CMakeLists.txt
const std::filesystem::path ChecksumDataset::m_project_root{PROJECT_ROOT};

ChecksumDataset::ChecksumDataset(const std::string &json_filename)
    : m_json_filename(m_project_root / "unit_tests/json_configs" / json_filename)
{
    load_json();
    parser_header();
    parse_files_section();

    std::cout << "Loaded " << this->m_files.size() << " checksums from " << json_filename << " to check\n";
}

ChecksumDataset::~ChecksumDataset()
{
    if (m_changed)
    {
        write_json();
    }
}

void ChecksumDataset::write_json()
{
    rapidjson::StringBuffer s;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);

    writer.StartObject();

    writer.Key("name");
    writer.String(m_name);

    writer.Key("relative_path");
    writer.String(m_relative_path);

    writer.Key("new_file_check");
    writer.StartObject();
    {
        writer.Key("source_extension");
        writer.String(m_source_extension.string());

        writer.Key("files_to_skip");
        writer.StartArray();
        {
            for (const auto &skip : m_skip)
            {
                writer.String(StripFullPath(skip));
            }
        }
        writer.EndArray();

        writer.Key("output_extensions");
        writer.StartArray();
        {
            for (const auto &ext : m_output_extensions)
            {
                writer.String(ext.string());
            }
        }
        writer.EndArray();
    }
    writer.EndObject(); // new_file_check

    writer.Key("files");
    writer.StartArray();
    {
        for (const auto &file : m_files)
        {
            const auto &file_info = file.second;
            writer.StartObject();
            {
                writer.Key("source_file");
                writer.StartObject();
                {
                    writer.Key("filename");
                    writer.String(StripFullPath(file_info.source_file.filename));

                    writer.Key("sha512");
                    writer.String(file_info.source_file.sha512_checksum);
                }
                writer.EndObject();

                writer.Key("output");
                writer.StartArray();
                {
                    for (const auto &out : file_info.output_files)
                    {
                        writer.StartObject();
                        {
                            writer.Key("filename");
                            writer.String(StripFullPath(out.filename));

                            writer.Key("sha512");
                            writer.String(out.sha512_checksum);
                        }
                        writer.EndObject();
                    }
                }
                writer.EndArray();
            }
            writer.EndObject();
        }
    }
    writer.EndArray();
    writer.EndObject();

    auto new_filename = m_json_filename;
    //    new_filename.replace_filename(m_json_filename.stem().string() + "_temp.json");
    std::ofstream strm(new_filename.string());
    if (!strm.good())
    {
        throw std::logic_error("Cannot open file:" + new_filename.string());
    }
    strm << s.GetString() << std::endl;
}

/**
 * Creates a FilePair from a JSON object like
 * {
 * "filename": "abilities.dat",
 * "sha512":
 * "4f7a820fc8ea48780af1dbf5bca1f7a36382018175b559bf58322455bb3faf85764508ee03163d3d08cd4a64eeb0a84101a62b2df81f54ef28f45d6cd9469580"
 * }
 *
 * @param value JSON object
 * @return FilePair
 */
FilePair ChecksumDataset::extract_file_pair_from_json(const rapidjson::Value &value) const
{
    auto filename = m_project_root / m_relative_path / value["filename"].Get<std::string>();
    auto sha = value["sha512"].Get<std::string>();
    return {std::move(filename), std::move(sha)};
}

void ChecksumDataset::load_json()
{
    std::ifstream in(m_json_filename.string());
    if (!in.good())
    {
        throw std::logic_error("Failed to open file: " + m_json_filename.string());
    }
    rapidjson::IStreamWrapper in_w(in);
    m_json.ParseStream(in_w);
}

void ChecksumDataset::parser_header()
{
    m_name = m_json["name"].Get<std::string>();
    m_relative_path = m_json["relative_path"].Get<std::string>();

    parse_new_file_check_section();
}

/**
 *   "new_file_check": {
 *   "files_to_skip": [
 *     "color.dat"
 *   ],
 *   "source_extension": ".def",
 *   "output_extensions": [
 *     ".dat"
 *   ]
 * },
 */
void ChecksumDataset::parse_new_file_check_section()
{
    auto &nfc = m_json["new_file_check"];
    for (auto &filename : nfc["files_to_skip"].GetArray())
    {
        auto fname = std::filesystem::path() / m_project_root / m_relative_path / filename.Get<std::string>();
        m_skip.insert(fname);
    }

    m_source_extension = nfc["source_extension"].Get<std::string>();

    for (auto &file_ext : nfc["output_extensions"].GetArray())
    {
        auto ext = std::filesystem::path(file_ext.Get<std::string>());
        m_output_extensions.insert(ext);
    }
}

void ChecksumDataset::parse_files_section()
{
    for (auto &file_data : m_json["files"].GetArray())
    {
        FileInfo file_info{.name = m_name, .source_file = extract_file_pair_from_json(file_data["source_file"])};

        for (auto &out_file : file_data["output"].GetArray())
        {
            auto pair = extract_file_pair_from_json(out_file);
            file_info.output_files.push_back(std::move(pair));
        }
        m_files.insert(std::make_pair(file_info.source_file.filename, std::move(file_info)));
    }

    process_new_files();
}

void ChecksumDataset::process_new_files()
{
    auto new_files = create_list_new_source_files();

    // Add the output files to the source files
    for (const auto &dir_entry : std::filesystem::directory_iterator{m_project_root / m_relative_path})
    {
        // Is it a file
        if (!dir_entry.is_regular_file())
        {
            continue;
        }

        auto filename = m_project_root / m_relative_path / dir_entry.path().filename();

        // Does the file extension match
        if (m_output_extensions.find(filename.extension()) == m_output_extensions.end())
        {
            continue;
        }

        // Should it be skipped
        if (m_skip.find(filename) != m_skip.end())
        {
            continue;
        }

        auto source_file_name = filename.stem() / m_source_extension;
        // Ignore if there is not matching source file
        if (auto it = new_files.find(source_file_name); it != new_files.end())
        {
            it->second.output_files.push_back(FilePair{.filename = std::move(filename), .sha512_checksum = "empty hash"});
        }
    }

    // Add them to the dataset to be checked
    m_files.insert(new_files.begin(), new_files.end());
}

ChecksumDataset::Container ChecksumDataset::create_list_new_source_files()
{
    ChecksumDataset::Container result;
    for (const auto &dir_entry : std::filesystem::directory_iterator{m_project_root / m_relative_path})
    {
        // Is it a file
        if (!dir_entry.is_regular_file())
        {
            continue;
        }

        auto filename = m_project_root / m_relative_path / dir_entry.path().filename();

        // Does the file extension match
        if (filename.extension() != m_source_extension)
        {
            continue;
        }

        // Should it be skipped
        if (m_skip.find(filename) != m_skip.end())
        {
            continue;
        }

        // if files exists already in json config do not add
        if (m_files.find(filename) == m_files.end())
        {
            FileInfo new_file{.name = m_name, .source_file = FilePair{.filename = filename, .sha512_checksum = "empty hash"}};
            for (const auto &ext : m_output_extensions)
            {
                auto output_filename = filename;
                output_filename.replace_extension(ext);
                new_file.output_files.push_back(FilePair{.filename = output_filename, .sha512_checksum = "empty hash"});
            }
            result.insert(std::make_pair(filename, new_file));
        }
    }
    return result;
}

ChecksumDataset::iterator ChecksumDataset::RemoveSourceFile(iterator it)
{
    m_changed = true;
    return m_files.erase(it);
}

std::string ChecksumDataset::StripFullPath(const std::filesystem::path &filename)
{
    const std::filesystem::path to_erase(m_project_root / m_relative_path);
    std::string result{filename.string()};

    if (auto loc = result.find(to_erase.string()); loc != std::string::npos)
    {
        result.erase(0, to_erase.string().length());
    }
    return result;
}

void ChecksumDataset::SetChanged()
{
    m_changed = true;
}

void ChecksumDataset::CancelChanged()
{
    m_changed = false;
}
