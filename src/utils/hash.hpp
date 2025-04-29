#pragma once
#include <string>
#include <filesystem>

std::string hash_file_contents(const std::filesystem::path& path);

std::string hash_string(const std::string& input);