#include "execute.hpp"
#include "argparse.hpp"

int main(int argc, char** argv)
{
    argparse::ArgumentParser program("rog-mux", "1.0", argparse::default_arguments::none);
    auto &group = program.add_mutually_exclusive_group(true);
    
    group.add_argument("-g", "--get")
        .flag()
        .help("get current MUX switch mode");

    group.add_argument("-s", "--set")
        .metavar("MODE")
        .choices("optimus", "ultimate")
        .help("set MUX switch mode to optimus/ultimate");

    try {
        program.parse_args(argc, argv);
    }
    catch(const std::exception& error) {
        std::cout << error.what() << std::endl << std::endl;
        std::cout << program << std::endl;
        return 1;
    }

    std::vector<unsigned char> command(ROGD_COMMAND_SIZE), result;

    if(program["--get"] == true) {
        command[0] = ROGD_COMMAND_MUX_GET;
    }
    else {
        command[0] = ROGD_COMMAND_MUX_SET;
        command[1] = (program.get("--set") == "ultimate");
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
        std::cout << "Current MUX switch mode is " << (result[1] ? "Ultimate" : "Optimus") << std::endl;
    }
    else {
        std::cout << "MUX switch mode will be set to " << (command[1] ? "Ultimate" : "Optimus") << " after reboot" << std::endl;
    }

    return 0;
}
