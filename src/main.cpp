#include "CLI/CLI.hpp"
#include "commands/init.hpp"
#include "commands/about.hpp"
#include "commands/stage.hpp"
#include "commands/snap.hpp"
#include "commands/list.hpp"

int main(const int argc, char** argv) {
    CLI::App app{"muvc - Music Version Control System"};

    register_init_command(app);
    register_about_command(app);
    register_stage_command(app);
    register_snap_command(app);
    register_list_command(app);

    try {
        CLI11_PARSE(app, argc, argv);
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
