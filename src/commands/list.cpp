#include "list.hpp"
#include "CLI/App.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

void register_list_command(CLI::App& app) {
    auto* list_cmd = app.add_subcommand("list", "Display snap history");

    list_cmd->callback([]() {
        const std::filesystem::path repo = ".muvc";
        const std::filesystem::path snaps_dir = repo / "snaps";

        if (!exists(repo) || !exists(snaps_dir)) {
            std::cerr << "[muvc] No snaps found. Please initialize a repository and make some snaps.\n";
            return;
        }

        std::vector<std::filesystem::path> commit_dirs;
        for (const auto& entry : std::filesystem::directory_iterator(snaps_dir)) {
            if (entry.is_directory()) {
                commit_dirs.push_back(entry.path());
            }
        }

        if (commit_dirs.empty()) {
            std::cerr << "[muvc] No snaps found in the repository.\n";
            return;
        }

        std::sort(commit_dirs.rbegin(), commit_dirs.rend());

        std::cout << "[muvc] Snap history:\n";
        for (const auto& commit_dir : commit_dirs) {
            std::ifstream meta(commit_dir / "meta.txt");
            if (!meta) {
                std::cerr << "[muvc] Failed to read meta.txt for snap " << commit_dir.filename() << "\n";
                continue;
            }

            std::string line;
            std::string commit_message;
            std::string timestamp;

            while (std::getline(meta, line)) {
                if (line.find("Message:") == 0) {
                    commit_message = line.substr(8);
                } else if (line.find("Timestamp:") == 0) {
                    timestamp = line.substr(10);
                }
            }

            std::cout << "\nSnap ID: " << commit_dir.filename().string().substr(0, 8) << "\n";
            std::cout << "Message: " << commit_message << "\n";
            std::cout << "Timestamp: " << timestamp << "\n";
        }
    });
}
