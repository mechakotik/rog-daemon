#ifndef __ROGD_PANEL_OD_HPP
#define __ROGD_PANEL_OD_HPP

namespace rogd::panel_od {
    int load();
    int get(bool &result);
    int set(bool value);
}

#endif
