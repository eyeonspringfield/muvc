#include "check.hpp"

void register_check_command(CLI::App &app) {
    auto snap_id = std::make_shared<std::string>("");

    auto *check_cmd = app.add_subcommand(
        "check", "Restore files from a snap into working directory, shelving current files");
    check_cmd->add_option("snap_id", *snap_id, "Snap ID to check");

    check_cmd->callback([snap_id]() {
        const std::filesystem::path repo = ".muvc";
        const std::filesystem::path snaps_dir = repo / "snaps";
        const std::filesystem::path shelf_dir = repo / "shelf";

        if (!exists(repo)) {
            std::cerr << "[muvc] Not a muvc repository.\n";
            return;
        }

        auto snap_to_use = *snap_id;
        if (snap_to_use.empty()) {
            std::ifstream head_file(repo / "HEAD");
            if (!head_file) {
                std::cerr << "[muvc] No HEAD file found.\n";
                return;
            }
            std::getline(head_file, snap_to_use);
        } else {
            std::vector<std::string> matches;
            for (const auto &entry: std::filesystem::directory_iterator(snaps_dir)) {
                if (!entry.is_directory()) continue;

                if (const auto &dir_name = entry.path().filename().string(); dir_name.starts_with(snap_to_use)) {
                    matches.push_back(dir_name);
                }
            }

            if (matches.empty()) {
                std::cerr << "[muvc] No snap found matching prefix: " << snap_to_use << "\n";
                return;
            }
            if (matches.size() > 1) {
                std::cerr << "[muvc] Ambiguous snap ID: " << snap_to_use << "\n";
                for (const auto &match: matches) {
                    std::cerr << "  - " << match << "\n";
                }
                return;
            }

            snap_to_use = matches.front();
        }

        std::ifstream head_file(repo / "HEAD");
        if (std::string current_head; head_file && std::getline(head_file, current_head)) {
            if (current_head == snap_to_use) {
                std::cout << "[muvc] Already on snap " << snap_to_use.substr(0, 8) << "\n";
                return;
            }
        }


        auto snap_path = snaps_dir / snap_to_use;
        if (!exists(snap_path) || !is_directory(snap_path)) {
            std::cerr << "[muvc] Snap " << snap_to_use << " does not exist.\n";
            return;
        }

        if (!exists(shelf_dir)) {
            create_directories(shelf_dir);
        } else {
            if (exists(shelf_dir / "restore_meta.txt")) {
                std::cerr << "[muvc] There are shelved files not restored yet. Please run 'restore' first.\n";
                return;
            }
        }

        std::ofstream meta(shelf_dir / "restore_meta.txt");
        if (!meta) {
            std::cerr << "[muvc] Could not open shelf meta file for writing.\n";
            return;
        }

        bool shelved_any = false;

        for (const auto &entry: std::filesystem::directory_iterator(snap_path)) {
            if (entry.path().filename() == "meta.txt") continue;

            auto filename = entry.path().filename();
            auto working_file = std::filesystem::current_path() / filename;

            if (exists(working_file)) {
                auto shelved_file = shelf_dir / filename;
                std::filesystem::rename(working_file, shelved_file);
                meta << filename.string() << "\n";
                shelved_any = true;
                std::cout << "[muvc] Shelved " << filename << "\n";
            }

            copy_file(entry.path(), working_file, std::filesystem::copy_options::overwrite_existing);
            std::cout << "[muvc] Checked out " << filename << "\n";
        }

        meta.close();

        if (!shelved_any) {
            std::filesystem::remove(shelf_dir / "restore_meta.txt");
        }
    });
}
