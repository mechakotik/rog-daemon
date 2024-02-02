executable(
    'rog-daemon',
    files(
        'main.cpp',
        'sysfs.cpp',
        'profile.cpp',
        'fan_curve.cpp',
        'mux.cpp',
        'panel_od.cpp'
    ),
    install: true,
    install_dir: '/usr/bin'
)

install_data('rog-daemon.service',
    install_dir: '/usr/lib/systemd/system'
)
