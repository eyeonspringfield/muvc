#include "init.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <ctime>

void register_init_command(CLI::App& app) {
    auto* init_cmd = app.add_subcommand("init", "Initialize a new muvc repository");
    init_cmd->callback([]() {
        const std::filesystem::path repository_path = ".muvc";

        if ( exists(repository_path)) {
            std::cout << "[muvc] Repository already initialized here." << std::endl;
            return;
        }

        try {
            create_directories(repository_path / "tracks");
            create_directories(repository_path / "tracks");
            std::ofstream(repository_path / "index").put(*"\0");
            if (std::ofstream config(repository_path / "config"); config) {
                const auto now = std::chrono::system_clock::now();
                const std::time_t now_time = std::chrono::system_clock::to_time_t(now);
                config << "project name = untitled\n";
                config << "created = " << std::ctime(&now_time);
            }
            std::cout << "[muvc] Initialized empty muvc repository in " << std::filesystem::current_path() << "/.muvc/" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[muvc] Error initializing repository: " << e.what() << std::endl;
        }


    });
}
