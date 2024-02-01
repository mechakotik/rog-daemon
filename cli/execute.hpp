#ifndef __ROGD_EXECUTE_HPP
#define __ROGD_EXECUTE_HPP

#include <vector>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "../daemon/enums.hpp"

const std::string ROGD_CONNECT_ERROR_MESSAGE = "unable to connect to rog-daemon, is it running?";

bool execute(std::vector<char> command, std::vector<char> &result)
{
    int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if(server_socket == -1) {
        return false;
    }

    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, "/tmp/rog-daemon-socket");

    if(connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        return false;
    }

    timeval read_timeout;
    read_timeout.tv_sec = 3;
    read_timeout.tv_usec = 0;
    setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*) &read_timeout, sizeof(read_timeout));

    char command_arr[ROGD_COMMAND_SIZE];
    for(int i = 0; i < ROGD_COMMAND_SIZE; i ++) {
        command_arr[i] = command[i];
    }
    
    if(write(server_socket, &command_arr, ROGD_COMMAND_SIZE) != ROGD_COMMAND_SIZE) {
        return false;
    }

    char result_arr[ROGD_COMMAND_SIZE];
    if(read(server_socket, &result_arr, ROGD_COMMAND_SIZE) != ROGD_COMMAND_SIZE) {
        return false;
    }
    
    close(server_socket);

    result = std::vector<char>(ROGD_COMMAND_SIZE);
    for(int i = 0; i < ROGD_COMMAND_SIZE; i ++) {
        result[i] = result_arr[i];
    }

    return true;
}

#endif
