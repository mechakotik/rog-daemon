<h1 align="center">rog-daemon</h1>

<div align="center">

![GitHub Tag](https://img.shields.io/github/v/tag/mechakotik/rog-daemon?label=latest&color=green)
![Endpoint Badge](https://img.shields.io/endpoint?url=https%3A%2F%2Fghloc.vercel.app%2Fapi%2Fmechakotik%2Frog-daemon%2Fbadge%3Ffilter%3D!argparse.hpp&label=lines%20of%20code&color=green)

</div>

rog-daemon is Linux daemon and CLI to control ASUS ROG/TUF laptops. It supports following features:

- Profile (throttle thermal policy)
- Custom fan curves
- MUX switch
- Panel Overdrive

## Build and install

To build this project, you need to install C++ compiler and Meson build system. The installation process varies through different distros and package managers, here is an example for Arch Linux:

```
sudo pacman -S gcc meson
```

After installing dependencies, run these commands to clone the repository, build and install the daemon:

```
git clone --recurse-submodules https://github.com/mechakotik/rog-daemon
cd rog-daemon
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

Note that all this tool does is modifying ``throttle_thermal_policy`` kernel option, which updates ``platform_profile`` accordingly. You can also control profile through another tool (e.g. TLP), and daemon will apply corresponding fan curve if necessary.

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

### `rog-panel-od`

Controls Panel Overdrive feature.

```bash
# Get current Panel Overdrive state
rog-panel-od --get

# Enable Panel Overdrive
rog-panel-od --enable

# Disable Panel Overdrive
rog-panel-od --disable
```

## Alternative init systems

Aside from systemd, rog-daemon supports OpenRC and dinit. To install service files for any of these init systems specify it as ``init`` Meson option when setting up build directory:

```bash
# OpenRC
meson setup build -Dinit=openrc

# dinit
meson setup build -Dinit=dinit
```

Feel free to contribute service scripts for other init systems as well.

## Disabling unneeded functionality

If your device does not support some of the daemon features, or you don't want daemon to control them, you may cut off unneeded functionality at build time using appropriate Meson options:

```bash
meson setup build -Dprofile=true -Dfan_curve=true -Dmux=true -Dpanel_od=true
```

Set unneeded features to false to disable them.
