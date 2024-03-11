#include "panel_od.hpp"
#include "sysfs.hpp"
#include "enums.hpp"
#include <fstream>

const fs_path PANEL_OD_PATH = ASUS_NB_WMI_PATH / "panel_od";
const fs_path PANEL_OD_SAVE_PATH = "/etc/rog-daemon/panel_od";;

namespace rogd::panel_od {
    int read_from_file();
    int write_to_file();

    bool loaded = false, enabled = true;
}

int rogd::panel_od::read_from_file()
{
    if(!std::filesystem::exists(PANEL_OD_SAVE_PATH)) {
        enabled = true;
        return ROGD_OK;
    }

    std::ifstream fin(PANEL_OD_SAVE_PATH);
    if(!fin.is_open()) {
        return ROGD_ERROR_FILE_READ_FAILED;
    }
    if(!(fin >> enabled)) {
        return ROGD_ERROR_FILE_READ_FAILED;
    }

    fin.close();
    return ROGD_OK;
}

int rogd::panel_od::write_to_file()
{
    std::ofstream fout(PANEL_OD_SAVE_PATH);
    if(!fout.is_open()) {
        return ROGD_ERROR_FILE_WRITE_FAILED;
    }
    if(!(fout << enabled << std::endl)) {
        return ROGD_ERROR_FILE_WRITE_FAILED;
    }

    fout.close();
    return ROGD_OK;
}

int rogd::panel_od::load()
{
    if(loaded) {
        return ROGD_OK;
    }
    if(!sysfs::has(ASUS_NB_WMI_PATH)) {
        return ROGD_ERROR_NO_ASUS_NB_WMI;
    }
    if(!sysfs::has(PANEL_OD_PATH)) {
        return ROGD_ERROR_PANEL_OD_UNSUPPORTED;
    }

    int error = read_from_file();
    if(error) {
        return error;
    }

    if(!sysfs::set(PANEL_OD_PATH, enabled)) {
        return ROGD_ERROR_SYSFS_WRITE_FAILED;
    }

    error = write_to_file();
    if(error) {
        return error;
    }

    loaded = true;
    return ROGD_OK;
}

int rogd::panel_od::get(bool &result)
{
    int error = load();
    if(error) {
        return error;
    }

    result = enabled;
    return ROGD_OK;
}

int rogd::panel_od::set(bool value)
{
    int error = load();
    if(error) {
        return error;
    }
    if(enabled == value) {
        return ROGD_OK;
    }

    enabled = value;
    if(!sysfs::set(PANEL_OD_PATH, enabled)) {
        enabled = !enabled;
        return ROGD_ERROR_SYSFS_WRITE_FAILED;
    }

    error = write_to_file();
    if(error) {
        return error;
    }

    return ROGD_OK;
}
