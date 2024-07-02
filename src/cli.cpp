#include <vector>
#include <string>
#include <format>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "argparse.hpp"
#include "enums.hpp"

const std::string ROGD_CONNECT_ERROR_MESSAGE = "unable to connect to rog-daemon, is it running?";

bool execute(std::vector<unsigned char> command, std::vector<unsigned char> &result)
{
    int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if(server_socket == -1) {
        return false;
    }

    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, ROGD_SOCKET_PATH.c_str());

    if(connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        return false;
    }

    timeval read_timeout;
    read_timeout.tv_sec = 3;
    read_timeout.tv_usec = 0;
    setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*) &read_timeout, sizeof(read_timeout));

    unsigned char command_arr[ROGD_COMMAND_SIZE];
    for(int i = 0; i < ROGD_COMMAND_SIZE; i ++) {
        command_arr[i] = command[i];
    }
    
    if(write(server_socket, &command_arr, ROGD_COMMAND_SIZE) != ROGD_COMMAND_SIZE) {
        return false;
    }

    unsigned char result_arr[ROGD_COMMAND_SIZE];
    if(read(server_socket, &result_arr, ROGD_COMMAND_SIZE) != ROGD_COMMAND_SIZE) {
        return false;
    }
    
    close(server_socket);

    result = std::vector<unsigned char>(ROGD_COMMAND_SIZE);
    for(int i = 0; i < ROGD_COMMAND_SIZE; i ++) {
        result[i] = result_arr[i];
    }

    return true;
}

#ifdef ROGD_BUILD_PROFILE
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

int rog_profile(int argc, char** argv)
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

    std::vector<unsigned char> command(ROGD_COMMAND_SIZE), result;

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
#endif

#ifdef ROGD_BUILD_FAN_CURVE
int rog_fan_curve(int argc, char** argv)
{
    argparse::ArgumentParser program("rog-fan-curve", "1.0", argparse::default_arguments::none);

    program.add_argument("-f", "--fan")
        .choices("cpu", "gpu", "mid")
        .metavar("FAN")
        .required()
        .help("fan to work with, cpu/gpu/mid");

    auto &group = program.add_mutually_exclusive_group(true);
    
    group.add_argument("-g", "--get")
        .flag()
        .help("get current fan curve");

    group.add_argument("-e", "--enable")
        .flag()
        .help("enable custom fan curve for current profile");
    
    group.add_argument("-d", "--disable")
        .flag()
        .help("disable custom fan curve for current profile");
    
    group.add_argument("-s", "--set")
        .metavar("CURVE")
        .help("set fan curve for current profile, in format Xc:X%,Xc:X%,Xc:X%,Xc:X%,Xc:X%,Xc:X%,Xc:X%,Xc:X%");
    
    group.add_argument("-r", "--reset")
        .flag()
        .help("set stock fan curve for current profile");

    try {
        program.parse_args(argc, argv);
    }
    catch(const std::exception& error) {
        std::cout << error.what() << std::endl << std::endl;
        std::cout << program << std::endl;
        return 1;
    }

    int fan;
    if(program.get("--fan") == "cpu") {
        fan = 0;
    }
    else if(program.get("--fan") == "gpu") {
        fan = 1;
    }
    else {
        fan = 2;
    }

    std::vector<unsigned char> command(ROGD_COMMAND_SIZE), result;
    command[1] = fan;

    if(program["--get"] == true) {
        command[0] = ROGD_COMMAND_FAN_CURVE_GET;
    }
    else if(program["--enable"] == true) {
        command[0] = ROGD_COMMAND_FAN_CURVE_ENABLE;
    }
    else if(program["--disable"] == true) {
        command[0] = ROGD_COMMAND_FAN_CURVE_DISABLE;
    }
    else if(program["--reset"] == true) {
        command[0] = ROGD_COMMAND_FAN_CURVE_RESET;
    }
    else {
        command[0] = ROGD_COMMAND_FAN_CURVE_SET;
        std::stringstream curve;
        curve << program.get("--set");
        bool error = false;

        for(int point = 0; point < 8; point ++) {
            int temp, pwm;
            char sym;
            if(!(curve >> temp >> sym >> sym >> pwm >> sym)) {
                error = true;
                break;
            }
            if(point <= 6 && !(curve >> sym)) {
                error = true;
                break;
            }
            pwm = (pwm * 255 + 99) / 100;
            command[point + 2] = temp;
            command[point + 10] = pwm;
        }

        if(error) {
            std::cout << "ERROR: " << "wrong custom fan curve format, use Xc:X%,Xc:X%,Xc:X%,Xc:X%,Xc:X%,Xc:X%,Xc:X%,Xc:X%" << std::endl;
            return 1;
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
        std::cout << (result[1] ? "Enabled" : "Disabled") << std::endl;
        for(int point = 0; point < 8; point ++) {
            int temp = result[point + 2];
            int pwm = result[point + 10];
            pwm = pwm * 100 / 255;
            std::cout << std::format("{}c:{}%", temp, pwm);
            if(point <= 6) {
                std::cout << ",";
            }
        }
        std::cout << std::endl;
    }
    else if(program["--enable"] == true) {
        std::cout << "Custom fan curve was enabled for current profile" << std::endl;
    }
    else if(program["--disable"] == true) {
        std::cout << "Custom fan curve was disabled for current profile" << std::endl;
    }
    else if(program["--reset"] == true) {
        std::cout << "Fan curve was set to stock for current profile" << std::endl;
    }
    else {
        std::cout << "New custom fan curve was set successfully" << std::endl;
    }

    return 0;
}
#endif

#ifdef ROGD_BUILD_MUX
int rog_mux(int argc, char** argv)
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
#endif

#ifdef ROGD_BUILD_PANEL_OD
int rog_panel_od(int argc, char** argv)
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

    std::vector<unsigned char> command(ROGD_COMMAND_SIZE), result;

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
#endif

int main(int argc, char** argv)
{
    if(std::getenv("ROGD_EXTERNAL_CALL") == NULL || std::string(std::getenv("ROGD_EXTERNAL_CALL")) != "1") {
        std::cout << "ERROR: rog-cli is unified CLI executable for internal use only, please use rog-profile, rog-fan-curve, rog-mux and rog-panel-od instead" << std::endl;
        return 1;
    }

    if(argc <= 1) {
        std::cout << "ERROR: no subcommand specified" << std::endl;
        return 1;
    }

    std::string subcommand = argv[1];
    for(int i = 1; i < argc - 1; i ++) {
        argv[i] = argv[i + 1];
    }

    if(subcommand == "profile") {
        #ifdef ROGD_BUILD_PROFILE
            return rog_profile(argc - 1, argv);
        #else
            std::cout << "ERROR: rog-daemon was built without profile control support" << std::endl;
            return 1;
        #endif
    }
    if(subcommand == "fan-curve") {
        #ifdef ROGD_BUILD_FAN_CURVE
            return rog_fan_curve(argc - 1, argv);
        #else
            std::cout << "ERROR: rog-daemon was built without custom fan curves support" << std::endl;
            return 1;
        #endif
    }
    if(subcommand == "mux") {
        #ifdef ROGD_BUILD_MUX
            return rog_mux(argc - 1, argv);
        #else
            std::cout << "ERROR: rog-daemon was built without MUX switch support" << std::endl;
            return 1;
        #endif
    }
    if(subcommand == "panel-od") {
        #ifdef ROGD_BUILD_PANEL_OD
            return rog_panel_od(argc - 1, argv);
        #else
            std::cout << "ERROR: rog-daemon was built without Panel Overdrive support" << std::endl;
            return 1;
        #endif
    }
    
    std::cout << "ERROR: unknown subcommand" << std::endl;
    return 1;
}
