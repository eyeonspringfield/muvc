#include "about.hpp"
#include <iostream>

void register_about_command(CLI::App& app) {
    auto* about_cmd = app.add_subcommand("about", "Show info about app");
    about_cmd->callback([]() {
        std::cout << "[muvc] muvc version 0.0.1, copyright 2025 Mark Csorgo" << std::endl;
    });
}