#include "hash.hpp"
#include <fstream>

constexpr uint64_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

std::string hash_file_contents(const std::filesystem::path &path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) return "ERROR";

    uint64_t hash = FNV_OFFSET_BASIS;
    char buffer[4096];

    while (in.read(buffer, sizeof(buffer)) || in.gcount() > 0) {
        for (std::streamsize i = 0; i < in.gcount(); ++i) {
            hash ^= static_cast<unsigned char>(buffer[i]);
            hash *= FNV_PRIME;
        }
    }

    std::stringstream<char> ss;
    ss << std::hex << hash;
    return ss.str();
}

std::string hash_string(const std::string& input) {
    uint64_t hash = FNV_OFFSET_BASIS;
    for (const char c : input) {
        hash ^= static_cast<unsigned char>(c);
        hash *= FNV_PRIME;
    }

    std::stringstream ss;
    ss << std::hex << hash;
    return ss.str();
}
