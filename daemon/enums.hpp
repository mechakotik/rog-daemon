#ifndef __ROGD_ENUMS_HPP
#define __ROGD_ENUMS_HPP

#include <vector>
#include <string>

const int ROGD_COMMAND_SIZE = 20;

enum {
    ROGD_COMMAND_PROFILE_GET,
    ROGD_COMMAND_PROFILE_SET,
    ROGD_COMMAND_PROFILE_NEXT
};

enum {
    ROGD_OK,
    ROGD_ERROR_INVALID_COMMAND,
    ROGD_ERROR_NO_ASUS_NB_WMI,
    ROGD_ERROR_NO_TTP,
    ROGD_ERROR_READ_FAILED,
    ROGD_ERROR_WRITE_FAILED,
};

const std::vector<std::string> ROGD_ERROR_MESSAGES
{
    "ok",
    "invalid command",
    "could not find asus-nb-wmi driver",
    "could not find throttle_thermal_policy option",
    "sysfs read failed",
    "sysfs write failed"
};

#endif
