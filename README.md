<h1 align="center">rog-daemon</h1>

rog-daemon is Linux damon and CLI to control ASUS ROG/TUF laptops. Currently it supports following features:

- Profile (throttle thermal policy)
- Custom fan curves
- MUX switch
- Panel Overdrive

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
