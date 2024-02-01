#include "mux.hpp"
#include "sysfs.hpp"
#include "enums.hpp"

const fs_path MUX_PATH = ASUS_NB_WMI_PATH / "gpu_mux_mode";

int rogd::mux::get(bool &result)
{
    if(!sysfs::has(ASUS_NB_WMI_PATH)) {
        return ROGD_ERROR_NO_ASUS_NB_WMI;
    }
    if(!sysfs::has(MUX_PATH)) {
        return ROGD_ERROR_MUX_UNSOPPORTED;
    }

    int value;
    if(!sysfs::get(MUX_PATH, value)) {
        return ROGD_ERROR_SYSFS_READ_FAILED;
    }

    result = (value == 0);
    return ROGD_OK;
}

int rogd::mux::set(bool value)
{
    if(!sysfs::has(ASUS_NB_WMI_PATH)) {
        return ROGD_ERROR_NO_ASUS_NB_WMI;
    }
    if(!sysfs::has(MUX_PATH)) {
        return ROGD_ERROR_MUX_UNSOPPORTED;
    }
    if(!sysfs::set(MUX_PATH, (value ? 0 : 1))) {
        return ROGD_ERROR_SYSFS_WRITE_FAILED;
    }
    return ROGD_OK;
}
