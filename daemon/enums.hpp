#ifndef __ROGD_ENUMS_HPP
#define __ROGD_ENUMS_HPP

#include <vector>
#include <string>

const int ROGD_COMMAND_SIZE = 20;

enum {
    ROGD_COMMAND_PROFILE_GET,
    ROGD_COMMAND_PROFILE_SET,
    ROGD_COMMAND_PROFILE_NEXT,
    ROGD_COMMAND_FAN_CURVE_GET,
    ROGD_COMMAND_FAN_CURVE_ENABLE,
    ROGD_COMMAND_FAN_CURVE_DISABLE,
    ROGD_COMMAND_FAN_CURVE_SET,
    ROGD_COMMAND_FAN_CURVE_RESET
};

enum {
    ROGD_OK,
    ROGD_ERROR_INVALID_COMMAND,
    ROGD_ERROR_NO_ASUS_NB_WMI,
    ROGD_ERROR_NO_TTP,
    ROGD_ERROR_SYSFS_READ_FAILED,
    ROGD_ERROR_SYSFS_WRITE_FAILED,
    ROGD_ERROR_FAN_CURVES_UNSUPPORTED,
    ROGD_ERROR_NO_SUCH_FAN,
    ROGD_ERROR_FILE_READ_FAILED,
    ROGD_ERROR_FILE_WRITE_FAILED
};

const std::vector<std::string> ROGD_ERROR_MESSAGES
{
    "ok",
    "invalid command",
    "could not find asus-nb-wmi driver",
    "could not find throttle_thermal_policy option",
    "sysfs read failed",
    "sysfs write failed",
    "your device does not support custom fan curves",
    "no such fan on your device",
    "file read failed",
    "file write failed"
};

#endif
