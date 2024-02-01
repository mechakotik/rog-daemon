#include "execute.hpp"
#include "argparse.hpp"

std::string get_profile_name(int id)
{
    switch(id) {
        case ROGD_PROFILE_BALANCED:
            return "Balanced";
        case ROGD_PROFILE_PERFORMANCE:
            return "Performance";
        case ROGD_PROFILE_QUIET:
            return "Quiet";
        default:
            return "Unknown (" + std::to_string(id) + ")";
    }
}

int main(int argc, char** argv)
{
    argparse::ArgumentParser program("rog-profile", "1.0", argparse::default_arguments::none);
    auto &group = program.add_mutually_exclusive_group(true);
    
    group.add_argument("-g", "--get")
        .flag()
        .help("get current profile");

    group.add_argument("-s", "--set")
        .choices("balanced", "performance", "quiet")
        .metavar("PROFILE")
        .help("set profile to balanced/performance/quiet");

    group.add_argument("-n", "--next")
        .flag()
        .help("switch to next profile");

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
        command[0] = ROGD_COMMAND_PROFILE_GET;
    }
    else if(program["--next"] == true) {
        command[0] = ROGD_COMMAND_PROFILE_NEXT;
    }
    else {
        command[0] = ROGD_COMMAND_PROFILE_SET;
        if(program.get("--set") == "balanced") {
            command[1] = ROGD_PROFILE_BALANCED;
        }
        else if(program.get("--set") == "performance") {
            command[1] = ROGD_PROFILE_PERFORMANCE;
        }
        else {
            command[1] = ROGD_PROFILE_QUIET;
        }
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
        std::cout << "Current profile is " << get_profile_name(result[1]) << std::endl;
    }
    else if(program["--next"] == true) {
        std::cout << "Profile was set to " << get_profile_name(result[1]) << std::endl;
    }
    else {
        std::cout << "Profile was set to " << get_profile_name(command[1]) << std::endl;
    }

    return 0;
}
