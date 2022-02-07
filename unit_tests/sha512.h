#pragma once

#include <openssl/sha.h>

#include <array>
#include <string>

class sha512
{
public:
    sha512();

    void generate(const std::string &filename);

    [[nodiscard]] std::array<unsigned char, SHA512_DIGEST_LENGTH> getChecksum() const;
    [[nodiscard]] const std::string &getChecksumString() const;

private:
    bool checksum_calculated{false};
    SHA512_CTX context{};
    std::array<unsigned char, SHA512_DIGEST_LENGTH> checksum{};
    std::string checksum_string;
};
