#ifndef __ROGD_SYSFS_HPP
#define __ROGD_SYSFS_HPP

#include <filesystem>

using fs_path = std::filesystem::path;

const fs_path ASUS_NB_WMI_PATH = "/sys/devices/platform/asus-nb-wmi";

namespace rogd::sysfs {
    bool has(fs_path path);
    bool get(fs_path path, int &result);
    bool set(fs_path path, int value);
}

#endif
