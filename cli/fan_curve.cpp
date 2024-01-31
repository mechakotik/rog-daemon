#include "execute.hpp"
#include "argparse.hpp"
#include <format>

int main(int argc, char** argv)
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

    group.add_argument("-n", "--enable")
        .flag()
        .help("enable custom fan curve for current profile");
    
    group.add_argument("-n", "--disable")
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

    std::vector<char> command(ROGD_COMMAND_SIZE), result;
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
            pwm = pwm * 255 / 100;
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
            int temp = (unsigned int)result[point + 2];
            int pwm = (unsigned int)result[point + 10];
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
