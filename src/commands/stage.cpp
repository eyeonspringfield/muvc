#include <filesystem>
#include <fstream>
#include <unordered_set>
#include "stage.hpp"
#include "CLI/App.hpp"
#include "../utils/hash.hpp"

const std::unordered_set<std::string> valid_extensions = {
    ".wav", ".mp3", ".flac", ".ogg", ".m4a", ".aiff", ".alac", ".opus"
};

void register_stage_command(CLI::App &app) {
    auto file_to_stage = std::make_shared<std::string>();

    auto *stage_cmd = app.add_subcommand("stage", "Stage a music file for versioning");
    stage_cmd->add_option("file", *file_to_stage, "Path to music file")->required();

    stage_cmd->callback([file_to_stage]() {
        const std::filesystem::path repository_path = ".muvc";
        const std::filesystem::path tracks_dir = repository_path / "tracks";
        const std::filesystem::path index_path = repository_path / "index";

        const auto &source_path = static_cast<std::filesystem::path>(*file_to_stage);

        if (!exists(repository_path)) {
            std::cerr << "[muvc] Not a muvc repository (run muvc init to initialize an empty repository)" << std::endl;
            return;
        }

        if (!exists(source_path)) {
            std::cerr << "[muvc] File not found: " << source_path << std::endl;
            return;
        }

        std::string ext = source_path.extension().string();
        std::ranges::transform(ext, ext.begin(), ::tolower);
        if (!valid_extensions.contains(ext)) {
            std::cerr << "[muvc] Unsupported file type: '" << ext << "' . Only audio files are supported" << std::endl;
            return;
        }

        std::filesystem::path absolute_path;
        try {
            absolute_path = absolute(source_path);
        } catch (const std::filesystem::filesystem_error &e) {
            std::cerr << "[muvc] Error resolving path: " << e.what() << std::endl;
            return;
        }

        std::string hash = hash_file_contents(absolute_path);
        if (hash == "ERROR") {
            std::cerr << "[muvc] Failed to read file content for hashing.\n";
            return;
        }

        std::filesystem::path dest_path = tracks_dir / (hash + "_" + source_path.filename().string());

        if (exists(dest_path)) {
            std::cout << "[muvc] Note: File contents already exist in repository as "
                    << dest_path.filename() << "\n";
        } else {
            try {
                copy_file(source_path, dest_path,
                          std::filesystem::copy_options::overwrite_existing);
                std::cout << "[muvc] Stored new object: " << dest_path.filename() << std::endl;
            } catch (const std::exception &e) {
                std::cerr << "[muvc] Failed to copy file: " << e.what() << std::endl;;
                return;
            }
        }

        if (std::ifstream in(index_path, std::ios::binary); in) {
            in.seekg(0, std::ios::end);
            if (auto size = in.tellg(); size > 0) {
                in.seekg(0);
                std::string content((std::istreambuf_iterator(in)),
                                    std::istreambuf_iterator<char>());

                auto first_char = content.front();
                bool allowed_char = (std::isalnum(static_cast<unsigned char>(first_char)) ||
                                     first_char == '.' || first_char == '_' || first_char == '-' ||
                                     first_char == ' ' || first_char == '(' || first_char == ')');

                if (!allowed_char) {
                    content = content.substr(1);
                    std::ofstream out(index_path, std::ios::binary | std::ios::trunc);
                    out << content;
                }
            }
        }

        bool already_staged = false;
        if (std::ifstream index_file(index_path); index_file) {
            std::string line;
            while (std::getline(index_file, line)) {
                if (line == (source_path.filename().string() + " HASH:" + hash)) {
                    already_staged = true;
                    break;
                }
            }
        }

        if (already_staged) {
            std::cout << "[muvc] Warning: File " << source_path.filename()
                    << " with identical content is already staged.\n";
        } else {
            if (std::ofstream index_file(index_path, std::ios::app | std::ios::binary); index_file) {
                index_file << source_path.filename().string() << " HASH:" << hash << "\n";
            } else {
                std::cerr << "[muvc] Failed to open index file for appending.\n";
                return;
            }
        }

        std::cout << "[muvc] Staged: " << source_path.filename()
                << " as " << dest_path.filename()
                << " [hash: " << hash << "]\n";
    });
}
