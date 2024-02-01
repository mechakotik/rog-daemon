<h1 align="center">rog-daemon</h1>

rog-daemon is Linux damon and CLI to control ASUS ROG/TUF laptops specific features. It is still under development and some key features are missing. Currently it supports following features:

- Profile (throttle thermal policy) control
- Custom fan curves
- MUX switch control

## Build and install

To build this project, you need to install C++ comiler and Meson build system. The installation process varies through different distros and package managers, here is the example for Arch Linux:

```
sudo pacman -S base-devel meson
```

After installing dependencies, clone the repository and run these commands in the repository folder to build and install the daemon:

```
meson setup build && cd build
meson compile
sudo meson install
```

Then to start the daemon and enable autostart, run these commands:

```
sudo systemctl start rog-daemon
sudo systemctl enable rog-daemon
```

## Using the CLI

### `rog-profile`

Controls profile, which affects fan curves and thermal throttling. Three profiles are available: Balanced, Performance and Quiet.

```bash
# Get current profile
rog-profile --get

# Set current profile to balanced
rog-profile --set=balanced

# Switch to next profile
rog-profile --next
```

### `rog-fan-curve`

Allows to override default fan curves for each fan (cpu, gpu, mid) for current profile.

```bash
# Get current fan curve for CPU fan
rog-fan-curve --fan=cpu --get

# Set custom fan curve for CPU fan
rog-fan-curve --fan=cpu --set=0c:0%,65c:0%,67c:30%,70c:40%,75c:50%,80c:60%,85c:80%,90c:90%

# Reset to factory default fan curve for CPU fan:
rog-fan-curve --fan=cpu --reset
```

### `rog-mux`

Controls MUX switch mode, may be either Optimus (iGPU is used for rendering, dGPU is available through PRIME render offload) or Ultimate (dGPU is used for rendering).

```bash
# Get current MUX switch mode
rog-mux --get

# Set MUX switch mode to Optimus
rog-mux --set=optimus

# Set MUX switch mode to Ultimate
rog-mux --set=ultimate
```

## Difference from asusctl

A note on why this was created instead of using asusctl. The projects are similar, but asusctl uses power-profiles-daemon to control `platform_profile`. PPD is power management tool which,
alongside with controlling `platform_profile`, manages lots of power related kernel options. It conflicts with other power saving tools like TLP or TuneD, so asusctl forces user to have PPD as
power saving tool.

rog-daemon doesn't use an upstream tool to manage `platform_profile`, so user is free to use the power management tool he wants or not use it at all. It has rog-profile tool which does the
job of controlling `platform_profile`. However, if this option is changed directly by user or another program (for example, TLP has option for this), the daemon will work fine with it and
apply corresponding custom fan curve if enabled.
