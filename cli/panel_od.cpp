#include "execute.hpp"
#include "argparse.hpp"

int main(int argc, char** argv)
{
    argparse::ArgumentParser program("rog-panel-od", "1.0", argparse::default_arguments::none);
    auto &group = program.add_mutually_exclusive_group(true);
    
    group.add_argument("-g", "--get")
        .flag()
        .help("get current Panel Overdrive state");

    group.add_argument("-e", "--enable")
        .flag()
        .help("enable Panel Overdrive");
    
    group.add_argument("-d", "--disable")
        .flag()
        .help("disable Panel Overdrive");

    try {
        program.parse_args(argc, argv);
    }
    catch(const std::exception& error) {
        std::cout << error.what() << std::endl << std::endl;
        std::cout << program << std::endl;
        return 1;
    }

    std::vector<char> command(ROGD_COMMAND_SIZE), result;

    if(program["--get"] == true) {
        command[0] = ROGD_COMMAND_PANEL_OD_GET;
    }
    else {
        command[0] = ROGD_COMMAND_PANEL_OD_SET;
        command[1] = program.is_used("--enable");
    }

    if(!execute(command, result)) {
        std::cout << "ERROR: " << ROGD_CONNECT_ERROR_MESSAGE << std::endl;
        return 1;
    }
    if(result[0] != ROGD_OK) {
        std::cout << "ERROR: " << ROGD_ERROR_MESSAGES[result[0]] << std::endl;
        return 1;
    }

    if(program["--get"] == true) {
        std::cout << "Panel Overdrive is currently " << (result[1] ? "enabled" : "disabled") << std::endl;
    }
    else {
        std::cout << "Panel Overdrive was " << (command[1] ? "enabled" : "disabled") << std::endl;
    }

    return 0;
}
