project(
    'rog-daemon', 'cpp',
    default_options: 'cpp_std=c++20'
)

if get_option('profile')
    add_project_arguments('-DROGD_BUILD_PROFILE', language: 'cpp')
endif
if get_option('fan_curve')
    add_project_arguments('-DROGD_BUILD_FAN_CURVE', language: 'cpp')
endif
if get_option('mux')
    add_project_arguments('-DROGD_BUILD_MUX', language: 'cpp')
endif
if get_option('panel_od')
    add_project_arguments('-DROGD_BUILD_PANEL_OD', language: 'cpp')
endif

executable(
    'rog-daemon',
    files(
        'src/main.cpp',
        'src/sysfs.cpp',
        'src/profile.cpp',
        'src/fan_curve.cpp',
        'src/mux.cpp',
        'src/panel_od.cpp'
    ),
    install: true,
    install_dir: '/usr/bin'
)

executable(
    'rog-cli',
    files('src/cli.cpp'),
    install: true,
    install_dir: '/usr/bin'
)

install_data('scripts/rog-profile', install_dir: '/usr/bin', install_mode: 'rwxr-xr-x')
install_data('scripts/rog-fan-curve', install_dir: '/usr/bin', install_mode: 'rwxr-xr-x')
install_data('scripts/rog-mux', install_dir: '/usr/bin', install_mode: 'rwxr-xr-x')
install_data('scripts/rog-panel-od', install_dir: '/usr/bin', install_mode: 'rwxr-xr-x')

init = get_option('init')

if init == 'systemd'
    install_data('scripts/systemd',
        install_dir: '/usr/lib/systemd/system',
        rename: 'rog-daemon.service'
    )
elif init == 'openrc'
    install_data('scripts/openrc',
        install_dir: '/etc/init.d',
        install_mode: 'rwxr-xr-x',
        rename: 'rog-daemon'
    )
elif init == 'dinit'
    install_data('scripts/dinit',
        install_dir: '/etc/dinit.d',
        rename: 'rog-daemon'
    )
endif
