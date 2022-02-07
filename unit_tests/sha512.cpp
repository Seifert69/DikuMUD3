#include "sha512.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>

sha512::sha512()
{
    if (SHA512_Init(&context) == 0)
    {
        throw std::logic_error("SHA512_Init() failed");
    }
}

void sha512::generate(const std::string &filename)
{
    if (checksum_calculated)
    {
        throw std::logic_error("Checksum already calculated");
    }

    const size_t blocksize = 32768;
    std::array<unsigned char, blocksize> data_block{};
    auto filesize = std::filesystem::file_size(filename);
    std::ifstream in(filename, std::ios::binary);

    while (filesize >= blocksize)
    {
        in.read(reinterpret_cast<char *>(data_block.data()), blocksize);
        if (SHA512_Update(&context, data_block.data(), blocksize) == 0)
        {
            throw std::logic_error("SHA512_Update() failed");
        }
        filesize -= blocksize;
    }

    if (filesize > 0)
    {
        in.read(reinterpret_cast<char *>(data_block.data()), filesize);
        if (SHA512_Update(&context, data_block.data(), filesize) == 0)
        {
            throw std::logic_error("SHA512_Update() failed");
        }
    }

    if (SHA512_Final(checksum.data(), &context) == 0)
    {
        throw std::logic_error("SHA512_Final() failed");
    }

    std::ostringstream os;
    for (auto ch : checksum)
    {
        os << std::setw(2) << std::setfill('0') << std::hex << static_cast<unsigned int>(ch);
    }
    checksum_string = os.str();

    checksum_calculated = true;
}

std::array<unsigned char, SHA512_DIGEST_LENGTH> sha512::getChecksum() const
{
    if (!checksum_calculated)
    {
        throw std::logic_error("No checksum calculated yet!");
    }
    return checksum;
}

const std::string &sha512::getChecksumString() const
{
    if (!checksum_calculated)
    {
        throw std::logic_error("No checksum calculated yet!");
    }

    return checksum_string;
}
