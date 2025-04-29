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
        const auto &source_path = static_cast<std::filesystem::path>(*file_to_stage);

        const std::filesystem::path repository_path = ".muvc";
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
            std::cerr << "[muvc] Failed to read file content for hashing." << std::endl;
            return;
        }

        const std::filesystem::path index_path = repository_path / "index";
        if (std::ifstream index_file(index_path); index_file) {
            std::string line;
            while (std::getline(index_file, line)) {
                if (line.find("HASH:" + hash) != std::string::npos) {
                    std::cout << "[muvc] File already staged: " << source_path.filename() << std::endl;
                    return;
                }
            }
        }

        std::filesystem::path tracks_dir = repository_path / "tracks";
        std::filesystem::path dest_path = tracks_dir / (hash + "_" + source_path.filename().string());

        try {
            copy_file(source_path, dest_path, std::filesystem::copy_options::overwrite_existing);
        } catch (const std::exception &e) {
            std::cerr << "[muvc] Failed to copy file: " << e.what() << std::endl;
            return;
        }


        if (std::ifstream in(index_path, std::ios::binary); in) {
            in.seekg(0, std::ios::end);
            if (auto size = in.tellg(); size > 0) {
                in.seekg(0);
                std::string content((std::istreambuf_iterator(in)),
                                     std::istreambuf_iterator<char>());
                content = content.substr(1);
                std::ofstream out(index_path, std::ios::binary | std::ios::trunc);
                out << content;
                std::cout << "[muvc] Stripped first character from index file.\n";
            }
        }

        if (std::ofstream index_file(repository_path / "index", std::ios::app | std::ios::binary); index_file) {
            index_file << source_path.filename().string() << " HASH:" << hash << "\n";
        } else {
            std::cerr << "[muvc] Failed to open index file for appending." << std::endl;
        }

        std::cout << "[muvc] Staged: " << source_path.filename()
          << " as " << dest_path.filename()
          << " [hash: " << hash << "]" << std::endl;
    });
}
