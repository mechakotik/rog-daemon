#ifndef __ROGD_FAN_CURVE_HPP
#define __ROGD_FAN_CURVE_HPP

#ifdef ROGD_BUILD_FAN_CURVE

#include "sysfs.hpp"
#include <array>

namespace rogd::fan_curve {
    const int NUM_FANS = 3, NUM_PROFILES = 3, NUM_CURVE_POINTS = 8;

    struct curve_t {
        std::array<int, NUM_CURVE_POINTS> temps, pwms;
        bool enabled = false;

        curve_t() {
            for(int point = 0; point < NUM_CURVE_POINTS; point ++) {
                temps[point] = point * 5;
                pwms[point] = point * 10;
            }
        }
    };

    int load();
    int fix();
    int get(int fan, curve_t &result);
    int set(int fan, curve_t value);
    int reset(int fan);
}

#endif

#endif
