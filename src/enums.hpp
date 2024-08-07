#ifndef __ROGD_ENUMS_HPP
#define __ROGD_ENUMS_HPP

#include <vector>
#include <string>

const int ROGD_COMMAND_SIZE = 20;

const std::string ROGD_SOCKET_PATH = "/run/rog-daemon.socket";

enum {
    ROGD_PROFILE_BALANCED,
    ROGD_PROFILE_PERFORMANCE,
    ROGD_PROFILE_QUIET
};

enum {
    ROGD_COMMAND_PROFILE_GET,
    ROGD_COMMAND_PROFILE_SET,
    ROGD_COMMAND_PROFILE_NEXT,
    ROGD_COMMAND_FAN_CURVE_GET,
    ROGD_COMMAND_FAN_CURVE_ENABLE,
    ROGD_COMMAND_FAN_CURVE_DISABLE,
    ROGD_COMMAND_FAN_CURVE_SET,
    ROGD_COMMAND_FAN_CURVE_RESET,
    ROGD_COMMAND_MUX_GET,
    ROGD_COMMAND_MUX_SET,
    ROGD_COMMAND_PANEL_OD_GET,
    ROGD_COMMAND_PANEL_OD_SET
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
    ROGD_ERROR_FILE_WRITE_FAILED,
    ROGD_ERROR_MUX_UNSOPPORTED,
    ROGD_ERROR_PANEL_OD_UNSUPPORTED
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
    "file write failed",
    "your device does not support MUX switch",
    "your device does not support Panel Overdrive"
};

#endif
