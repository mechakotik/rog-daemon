#ifndef __ROGD_PROFILE_HPP
#define __ROGD_PROFILE_HPP

namespace rogd::profile {
    #if defined(ROGD_BUILD_PROFILE) || defined(ROGD_BUILD_FAN_CURVE)
        int get(int &result);
    #endif
    
    #if defined(ROGD_BUILD_PROFILE)
        int set(int value);
        int next(int &result);
    #endif
}

#endif
