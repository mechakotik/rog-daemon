#include <iostream>
#include <vector>
#include <filesystem>
#include <thread>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <csignal>

#include "profile.hpp"
#include "fan_curve.hpp"
#include "mux.hpp"
#include "panel_od.hpp"
#include "enums.hpp"

std::vector<unsigned char> execute(std::vector<unsigned char> command)
{
    std::vector<unsigned char> result(ROGD_COMMAND_SIZE);

    switch(command[0]) {
        #if defined(ROGD_BUILD_PROFILE) || defined(ROGD_BUILD_FAN_CURVE)
        case ROGD_COMMAND_PROFILE_GET: {
            int id;
            result[0] = rogd::profile::get(id);
            result[1] = id;
            break;
        }
        #endif

        #ifdef ROGD_BUILD_PROFILE
        case ROGD_COMMAND_PROFILE_SET: {
            result[0] = rogd::profile::set(command[1]);
            break;
        }

        case ROGD_COMMAND_PROFILE_NEXT: {
            int id;
            result[0] = rogd::profile::next(id);
            result[1] = id;
            break;
        }
        #endif

        #ifdef ROGD_BUILD_FAN_CURVE
        case ROGD_COMMAND_FAN_CURVE_GET: {
            rogd::fan_curve::curve_t curve;
            result[0] = rogd::fan_curve::get(command[1], curve);
            result[1] = curve.enabled;
            for(int point = 0; point < rogd::fan_curve::NUM_CURVE_POINTS; point ++) {
                result[point + 2] = curve.temps[point];
            }
            for(int point = 0; point < rogd::fan_curve::NUM_CURVE_POINTS; point ++) {
                result[point + rogd::fan_curve::NUM_CURVE_POINTS + 2] = curve.pwms[point];
            }
            break;
        }

        case ROGD_COMMAND_FAN_CURVE_ENABLE: {
            rogd::fan_curve::curve_t curve;
            result[0] = rogd::fan_curve::get(command[1], curve);
            if(result[0]) {
                break;
            }
            curve.enabled = true;
            result[0] = rogd::fan_curve::set(command[1], curve);
            break;
        }

        case ROGD_COMMAND_FAN_CURVE_DISABLE: {
            rogd::fan_curve::curve_t curve;
            result[0] = rogd::fan_curve::get(command[1], curve);
            if(result[0]) {
                break;
            }
            curve.enabled = false;
            result[0] = rogd::fan_curve::set(command[1], curve);
            break;
        }

        case ROGD_COMMAND_FAN_CURVE_SET: {
            rogd::fan_curve::curve_t curve;
            curve.enabled = true;
            for(int point = 0; point < 8; point ++) {
                curve.temps[point] = command[point + 2];
                curve.pwms[point] = command[point + 10];
            }
            result[0] = rogd::fan_curve::set(command[1], curve);
            break;
        }

        case ROGD_COMMAND_FAN_CURVE_RESET: {
            result[0] = rogd::fan_curve::reset(command[1]);
            break;
        }
        #endif

        #ifdef ROGD_BUILD_MUX
        case ROGD_COMMAND_MUX_GET: {
            bool enabled;
            result[0] = rogd::mux::get(enabled);
            result[1] = enabled;
            break;
        }

        case ROGD_COMMAND_MUX_SET: {
            result[0] = rogd::mux::set(command[1]);
            break;
        }
        #endif

        #ifdef ROGD_BUILD_PANEL_OD
        case ROGD_COMMAND_PANEL_OD_GET: {
            bool enabled;
            result[0] = rogd::panel_od::get(enabled);
            result[1] = enabled;
            break;
        }

        case ROGD_COMMAND_PANEL_OD_SET: {
            result[0] = rogd::panel_od::set(command[1]);
            break;
        }
        #endif

        default: {
            result[0] = ROGD_ERROR_INVALID_COMMAND;
            break;
        }
    }

    return result;
}

int server_socket;

void interrupt(int signal)
{
    close(server_socket);
    std::filesystem::remove(ROGD_SOCKET_PATH);
    exit(signal);
}

int main()
{
    if(getuid() != 0) {
        std::cout << "ERROR: not running as superuser" << std::endl;
        return 1;
    }

    std::filesystem::remove(ROGD_SOCKET_PATH);
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if(server_socket == -1) {
        std::cout << "ERROR: failed to create socket" << std::endl;
        return 1;
    }
    
    sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, ROGD_SOCKET_PATH.c_str());

    if(bind(server_socket, (sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
        std::cout << "ERROR: failed to bind socket to " << ROGD_SOCKET_PATH << std::endl;
        return 1;
    }

    if(listen(server_socket, 5) == -1) {
        std::cout << "ERROR: failed to start listening socket" << std::endl;
        return 1;
    }

    std::filesystem::permissions(ROGD_SOCKET_PATH, std::filesystem::perms::all);
    signal(SIGINT, interrupt);
    signal(SIGABRT, interrupt);
    signal(SIGTERM, interrupt);

    #ifdef ROGD_BUILD_FAN_CURVE
        rogd::fan_curve::load();
        std::thread fan_curve_fix_thread(rogd::fan_curve::fix_loop);
    #endif

    #ifdef ROGD_BUILD_PANEL_OD
        rogd::panel_od::load();
    #endif

    while(true) {
        sockaddr_un client_addr;
        int client_addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (sockaddr*) &client_addr, (socklen_t*) &client_addr_len);

        unsigned char input[ROGD_COMMAND_SIZE];
        read(client_socket, &input, ROGD_COMMAND_SIZE);

        std::vector<unsigned char> input_vec(ROGD_COMMAND_SIZE);
        for(int i = 0; i < ROGD_COMMAND_SIZE; i ++) {
            input_vec[i] = input[i];
        }

        std::vector<unsigned char> output_vec = execute(input_vec);
        unsigned char output[ROGD_COMMAND_SIZE];
        for(int i = 0; i < ROGD_COMMAND_SIZE; i ++) {
            output[i] = output_vec[i];
        }

        write(client_socket, &output, ROGD_COMMAND_SIZE);
        close(client_socket);
    }

    #ifdef ROGD_BUILD_FAN_CURVE
        fan_curve_fix_thread.join();
    #endif
    
    return 0;
}
