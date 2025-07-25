#include "restore.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

void register_restore_command(CLI::App &app) {
    auto *restore_cmd = app.add_subcommand("restore", "Restore shelved files to working directory");

    auto force_yes = std::make_shared<bool>(false);
    restore_cmd->add_flag("-y,--yes", *force_yes, "Overwrite existing files without confirmation");

    restore_cmd->callback([force_yes] {
        const std::filesystem::path repo = ".muvc";
        const std::filesystem::path shelf_dir = repo / "shelf";
        const auto meta_file = shelf_dir / "restore_meta.txt";

        if (!exists(meta_file)) {
            std::cerr << "[muvc] No shelved files to restore.\n";
            return;
        }

        std::ifstream meta(meta_file);
        if (!meta) {
            std::cerr << "[muvc] Could not open shelved files metadata.\n";
            return;
        }

        bool restored_any = false;
        std::string filename;
        while (std::getline(meta, filename)) {
            auto shelved_file = shelf_dir / filename;
            auto working_file = std::filesystem::current_path() / filename;

            if (!exists(shelved_file)) {
                std::cerr << "[muvc] Shelved file " << filename << " missing!\n";
                continue;
            }

            if (exists(working_file)) {
                if (*force_yes) {
                    std::cout << "[muvc] Working directory file \"" << filename << "\" will be overwritten.\n";
                } else {
                    std::cerr << "[muvc] Warning: Working directory file \"" << filename <<
                            "\" exists and will be overwritten.\n";
                    std::cerr << "Proceed? [y/n]: ";

                    std::string response;
                    std::getline(std::cin, response);

                    if (response != "y" && response != "Y") {
                        std::cout << "[muvc] Skipped restoring \"" << filename << "\"\n";
                        continue;
                    }
                }

                std::filesystem::remove(working_file);
            }

            std::filesystem::rename(shelved_file, working_file);
            std::cout << "[muvc] Restored shelved file " << filename << "\n";
            restored_any = true;
        }

        meta.close();
        std::filesystem::remove(meta_file);

        if (!restored_any) {
            std::cout << "[muvc] No shelved files were restored.\n";
        }
    });
}
