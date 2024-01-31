#include <iostream>
#include <vector>
#include <filesystem>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <csignal>

#include "profile.hpp"
#include "enums.hpp"

std::vector<char> execute(std::vector<char> command)
{
    std::vector<char> result(ROGD_COMMAND_SIZE);

    switch(command[0]) {
        case ROGD_COMMAND_PROFILE_GET: {
            int id;
            result[0] = rogd::profile::get(id);
            result[1] = id;
            break;
        }

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
    }

    return result;
}

int server_socket;

void interrupt(int signal)
{
    close(server_socket);
    std::filesystem::remove("/tmp/rog-daemon-socket");
    exit(signal);
}

int main()
{
    if(getuid() != 0) {
        std::cout << "ERROR: not running as superuser" << std::endl;
        return 1;
    }

    std::filesystem::remove("/tmp/rog-daemon-socket");
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if(server_socket == -1) {
        std::cout << "ERROR: failed to create socket" << std::endl;
        return 1;
    }
    
    sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, "/tmp/rog-daemon-socket");

    if(bind(server_socket, (sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
        std::cout << "ERROR: failed to bind socket to /tmp/rog-daemon-socket" << std::endl;
        return 1;
    }

    if(listen(server_socket, 5) == -1) {
        std::cout << "ERROR: failed to start listening socket" << std::endl;
        return 1;
    }

    std::filesystem::permissions("/tmp/rog-daemon-socket", std::filesystem::perms::all);
    signal(SIGINT, interrupt);
    signal(SIGABRT, interrupt);
    signal(SIGTERM, interrupt);

    while(true) {
        sockaddr_un client_addr;
        int client_addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (sockaddr*) &client_addr, (socklen_t*) &client_addr_len);

        char input[ROGD_COMMAND_SIZE];
        read(client_socket, &input, ROGD_COMMAND_SIZE);

        std::vector<char> input_vec(ROGD_COMMAND_SIZE);
        for(int i = 0; i < ROGD_COMMAND_SIZE; i ++) {
            input_vec[i] = input[i];
        }

        std::vector<char> output_vec = execute(input_vec);
        char output[ROGD_COMMAND_SIZE];
        for(int i = 0; i < ROGD_COMMAND_SIZE; i ++) {
            output[i] = output_vec[i];
        }

        write(client_socket, &output, ROGD_COMMAND_SIZE);
        close(client_socket);
    }

    return 0;
}
