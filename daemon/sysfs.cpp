#include "sysfs.hpp"
#include <fstream>

bool rogd::sysfs::has(fs_path path)
{
    return std::filesystem::exists(path);
}

bool rogd::sysfs::get(fs_path path, int &result)
{
    std::ifstream fin(path);
    if(!fin.is_open()) {
        return false;
    }
    if(!(fin >> result)) {
        return false;
    }
    return true;
}

bool rogd::sysfs::get(fs_path path, std::string &result)
{
    std::ifstream fin(path);
    if(!fin.is_open()) {
        return false;
    }
    if(!(fin >> result)) {
        return false;
    }
    return true;
}

bool rogd::sysfs::set(fs_path path, int value)
{
    std::ofstream fout(path);
    if(!fout.is_open()) {
        return false;
    }
    if(!(fout << value << std::endl)) {
        return false;
    }
    return true;
}

bool rogd::sysfs::find_hwmon(fs_path path, std::string name, fs_path &result)
{
    path /= "hwmon";
    if(!has(path)) {
        return false;
    }

    for(fs_path subdir : std::filesystem::directory_iterator(path)) {
        if(!has(subdir / "name")) {
            continue;
        }
        std::string current_name;
        if(!get(subdir / "name", current_name)) {
            continue;
        }
        if(current_name == name) {
            result = subdir;
            return true;
        }
    }

    return false;
}
