#include "snap.hpp"
#include <memory>
#include "CLI/App.hpp"
#include "../utils/hash.hpp"
#include <chrono>

void register_snap_command(CLI::App& app) {
    auto snap_message = std::make_shared<std::string>();

    auto* snap_cmd = app.add_subcommand("snap", "Create a snapshot of staged files");
    snap_cmd->add_option("-m,--message", *snap_message, "Snap message")->required();

    snap_cmd->callback([snap_message]() {
        const std::filesystem::path repo = ".muvc";
        const std::filesystem::path index_file = repo / "index";
        const std::filesystem::path tracks_dir = repo / "tracks";
        const std::filesystem::path snaps_dir = repo / "snaps";

        if (!exists(repo) || !exists(index_file) || is_empty(index_file)) {
            std::cerr << "[muvc] No staged files or not a muvc repository.\n";
            return;
        }

        std::vector<std::pair<std::string, std::string>> files;
        std::ifstream in(index_file);
        std::ostringstream full_contents;
        std::string line;
        while (std::getline(in, line)) {
            full_contents << line << "\n";
            if (auto pos = line.find(" HASH:"); pos != std::string::npos) {
                std::string name = line.substr(0, pos);
                std::string hash = line.substr(pos + 6);
                files.emplace_back(name, hash);
            }
        }

        std::string snap_id = hash_string(full_contents.str() + *snap_message);

        std::filesystem::path snap_path = snaps_dir / snap_id;
        create_directories(snap_path);

        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::stringstream timestamp_stream;
        timestamp_stream << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S");
        std::string timestamp = timestamp_stream.str();

        for (auto& [name, hash] : files) {
            auto src = tracks_dir / (hash + "_" + name);
            auto dest = snap_path / name;
            copy_file(src, dest, std::filesystem::copy_options::overwrite_existing);
        }

        std::ofstream meta(snap_path / "meta.txt");
        meta << "Message: " << *snap_message << "\n";
        meta << "Timestamp: " << timestamp << "\n";
        meta << "Files:\n";
        for (auto& [name, hash] : files)
            meta << "- " << name << " (hash: " << hash << ")\n";

        std::ofstream head(repo / "HEAD");
        head << snap_id;

        std::ofstream clear(index_file, std::ios::trunc);
        clear.close();

        std::cout << "[muvc] Snap " << snap_id.substr(0, 8) << " created.\n";
    });
}
