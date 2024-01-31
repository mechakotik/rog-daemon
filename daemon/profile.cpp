#include "profile.hpp"
#include "sysfs.hpp"
#include "enums.hpp"

const fs_path TTP_PATH = ASUS_NB_WMI_PATH / "throttle_thermal_policy";

int rogd::profile::get(int &result)
{
    if(!sysfs::has(ASUS_NB_WMI_PATH)) {
        return ROGD_ERROR_NO_ASUS_NB_WMI;
    }
    if(!sysfs::has(TTP_PATH)) {
        return ROGD_ERROR_NO_TTP;
    }
    if(!sysfs::get(TTP_PATH, result)) {
        return ROGD_ERROR_SYSFS_READ_FAILED;
    }
    return ROGD_OK;
}

int rogd::profile::set(int value)
{
    if(!sysfs::has(ASUS_NB_WMI_PATH)) {
        return ROGD_ERROR_NO_ASUS_NB_WMI;
    }
    if(!sysfs::has(TTP_PATH)) {
        return ROGD_ERROR_NO_TTP;
    }
    if(!sysfs::set(TTP_PATH, value)) {
        return ROGD_ERROR_SYSFS_WRITE_FAILED;
    }
    return ROGD_OK;
}

int rogd::profile::next(int &value)
{
    int id, error = get(id);
    if(error) {
        return error;
    }

    id = (id + 1) % 3;
    error = set(id);
    if(error) {
        return error;
    }

    value = id;
    return ROGD_OK;
}
