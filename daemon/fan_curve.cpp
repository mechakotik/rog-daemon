#include "fan_curve.hpp"
#include "enums.hpp"
#include "profile.hpp"
#include <fstream>
#include <format>
#include <thread>

namespace rogd::fan_curve {
    std::array<std::array<curve_t, NUM_FANS>, NUM_PROFILES> curves;

    fs_path get_file_path(int profile, int fan);
    int read_from_file(int profile, int fan);
    int write_to_file(int profile, int fan);

    int read_from_sysfs(int profile, int fan);
    int write_to_sysfs(int profile, int fan);

    int get_available_fans_mask();
    void normalize(curve_t &curve);

    fs_path hwmon_path;
    int fans_mask = 0;
    bool loaded = false;
}

fs_path rogd::fan_curve::get_file_path(int profile, int fan)
{
    std::string name = "fan_curve_" + std::to_string(profile) + "_" + std::to_string(fan);
    fs_path path = "/etc/rog-daemon";
    std::filesystem::create_directories(path);
    path /= name;
    return path;
}

int rogd::fan_curve::read_from_file(int profile, int fan)
{
    curves[profile][fan] = curve_t();
    fs_path path = get_file_path(profile, fan);

    std::ifstream fin(path);
    if(!fin.is_open()) {
        return ROGD_ERROR_FILE_READ_FAILED;
    }

    curve_t curve;
    if(!(fin >> curve.enabled)) {
        return ROGD_ERROR_FILE_READ_FAILED;
    }
    for(int point = 0; point < NUM_CURVE_POINTS; point ++) {
        if(!(fin >> curve.temps[point])) {
            return ROGD_ERROR_FILE_READ_FAILED;
        }
    }
    for(int point = 0; point < NUM_CURVE_POINTS; point ++) {
        if(!(fin >> curve.pwms[point])) {
            return ROGD_ERROR_FILE_READ_FAILED;
        }
    }

    curves[profile][fan] = curve;
    return ROGD_OK;
}

int rogd::fan_curve::write_to_file(int profile, int fan)
{
    fs_path path = get_file_path(profile, fan);
    std::ofstream fout(path);

    if(!fout.is_open()) {
        return ROGD_ERROR_FILE_WRITE_FAILED;
    }

    curve_t curve = curves[profile][fan];
    if(!(fout << curve.enabled << std::endl)) {
        return ROGD_ERROR_FILE_WRITE_FAILED;
    }
    
    for(int point = 0; point < NUM_CURVE_POINTS; point ++) {
        fout << curve.temps[point] << ' ';
    }
    fout << std::endl;

    for(int point = 0; point < NUM_CURVE_POINTS; point ++) {
        fout << curve.pwms[point] << ' ';
    }
    fout << std::endl;

    fout.close();
    return ROGD_OK;
}

int rogd::fan_curve::read_from_sysfs(int profile, int fan)
{
    curve_t curve;

    for(int point = 0; point < NUM_CURVE_POINTS; point ++) {
        std::string name = std::format("pwm{}_auto_point{}_temp", fan + 1, point + 1);
        if(!sysfs::get(hwmon_path / name, curve.temps[point])) {
            return ROGD_ERROR_SYSFS_READ_FAILED;
        }
    }

    for(int point = 0; point < NUM_CURVE_POINTS; point ++) {
        std::string name = std::format("pwm{}_auto_point{}_pwm", fan + 1, point + 1);
        if(!sysfs::get(hwmon_path / name, curve.pwms[point])) {
            return ROGD_ERROR_SYSFS_READ_FAILED;
        }
    }

    int enabled;
    if(!sysfs::get(hwmon_path / std::format("pwm{}_enable", fan + 1), enabled)) {
        return ROGD_ERROR_SYSFS_READ_FAILED;
    }
    curve.enabled = (enabled == 1);

    curves[profile][fan] = curve;
    return ROGD_OK;
}

int rogd::fan_curve::write_to_sysfs(int profile, int fan)
{
    curve_t curve = curves[profile][fan];

    for(int point = 0; point < NUM_CURVE_POINTS; point ++) {
        std::string name = std::format("pwm{}_auto_point{}_temp", fan + 1, point + 1);
        if(!sysfs::set(hwmon_path / name, curve.temps[point])) {
            return ROGD_ERROR_SYSFS_READ_FAILED;
        }
    }

    for(int point = 0; point < NUM_CURVE_POINTS; point ++) {
        std::string name = std::format("pwm{}_auto_point{}_pwm", fan + 1, point + 1);
        if(!sysfs::set(hwmon_path / name, curve.pwms[point])) {
            return ROGD_ERROR_SYSFS_READ_FAILED;
        }
    }

    if(!sysfs::set(hwmon_path / std::format("pwm{}_enable", fan + 1), (curve.enabled ? 1 : 2))) {
        return ROGD_ERROR_SYSFS_READ_FAILED;
    }

    return ROGD_OK;
}

int rogd::fan_curve::load()
{
    if(loaded) {
        return ROGD_OK;
    }
    if(!sysfs::has(ASUS_NB_WMI_PATH)) {
        return ROGD_ERROR_NO_ASUS_NB_WMI;
    }
    if(!sysfs::find_hwmon(ASUS_NB_WMI_PATH, "asus_custom_fan_curve", hwmon_path)) {
        return ROGD_ERROR_FAN_CURVES_UNSUPPORTED;
    }

    fans_mask = get_available_fans_mask();
    for(int profile = 0; profile < NUM_PROFILES; profile ++) {
        for(int fan = 0; fan < NUM_FANS; fan ++) {
            if(fans_mask & (1 << fan)) {
                read_from_file(profile, fan);
            }
        }
    }

    loaded = true;
    return ROGD_OK;
}

int rogd::fan_curve::get_available_fans_mask()
{
    int mask = 0;
    for(int fan = 0; fan < NUM_FANS; fan ++) {
        std::string name = std::format("pwm{}_enable", fan + 1);
        if(sysfs::has(hwmon_path / name)) {
            mask |= (1 << fan);
        }
    }
    return mask;
}

int rogd::fan_curve::fix()
{
    if(!loaded) {
        return ROGD_OK;
    }

    int profile, error;
    error = profile::get(profile);
    if(error) {
        return error;
    }

    for(int fan = 0; fan < NUM_FANS; fan ++) {
        if((fans_mask & (1 << fan)) == 0) {
            continue;
        }
        if(!curves[profile][fan].enabled) {
            continue;
        }

        int enabled;
        std::string name = std::format("pwm{}_enable", fan + 1);
        if(!sysfs::get(hwmon_path / name, enabled)) {
            return ROGD_ERROR_SYSFS_READ_FAILED;
        }

        if(enabled != 1) {
            error = write_to_sysfs(profile, fan);
            if(error) {
                return error;
            }
        }
    }

    return ROGD_OK;
}

void rogd::fan_curve::fix_loop()
{
    while(1) {
        fix();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int rogd::fan_curve::get(int fan, curve_t &result)
{
    int error = load();
    if(error) {
        return error;
    }

    if((fans_mask & (1 << fan)) == 0) {
        return ROGD_ERROR_NO_SUCH_FAN;
    }

    int profile;
    error = profile::get(profile);
    if(error) {
        return error;
    }

    result = curves[profile][fan];
    return ROGD_OK;
}

int rogd::fan_curve::set(int fan, curve_t value)
{
    int error = load();
    if(error) {
        return error;
    }

    if((fans_mask & (1 << fan)) == 0) {
        return ROGD_ERROR_NO_SUCH_FAN;
    }

    int profile;
    error = profile::get(profile);
    if(error) {
        return error;
    }

    normalize(value);
    curves[profile][fan] = value;

    error = write_to_sysfs(profile, fan);
    if(error) {
        return error;
    }
    error = write_to_file(profile, fan);
    if(error) {
        return error;
    }

    return ROGD_OK;
}

int rogd::fan_curve::reset(int fan)
{
    int error = load();
    if(error) {
        return error;
    }

    if((fans_mask & (1 << fan)) == 0) {
        return ROGD_ERROR_NO_SUCH_FAN;
    }

    int profile;
    error = profile::get(profile);
    if(error) {
        return error;
    }

    std::string name = std::format("pwm{}_enable", fan + 1);
    if(!sysfs::set(hwmon_path / name, 3)) {
        return ROGD_ERROR_SYSFS_WRITE_FAILED;
    }

    error = read_from_sysfs(profile, fan);
    if(error) {
        return error;
    }
    error = write_to_file(profile, fan);
    if(error) {
        return error;
    }

    return ROGD_OK;
}

void rogd::fan_curve::normalize(curve_t &curve)
{
    for(int point = 0; point < NUM_CURVE_POINTS; point ++) {
        curve.temps[point] = std::max(curve.temps[point], 0);
        curve.temps[point] = std::min(curve.temps[point], 100);

        curve.pwms[point] = std::max(curve.pwms[point], 0);
        curve.pwms[point] = std::min(curve.pwms[point], 255);
    }
}
