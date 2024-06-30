#ifndef __ROGD_MUX_HPP
#define __ROGD_MUX_HPP

#ifdef ROGD_BUILD_MUX

namespace rogd::mux {
    int get(bool &result);
    int set(bool value);
}

#endif

#endif
