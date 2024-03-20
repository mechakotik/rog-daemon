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

init = get_option('init')

if init == 'systemd'
    install_data('services/systemd',
        install_dir: '/usr/lib/systemd/system',
        rename: 'rog-daemon.service'
    )
endif

if init == 'dinit'
    install_data('services/dinit',
        install_dir: '/etc/dinit.d',
        rename: 'rog-daemon'
    )
endif
