#!/bin/sh

# Stop immediately when configuration, compilation, or installation fails.
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
BUILD_DIR="$SCRIPT_DIR/build"

if [ -z "${HOME:-}" ]; then
    echo "install.sh: HOME is not set; cannot determine the user install prefix." >&2
    exit 1
fi

if ! command -v cmake >/dev/null 2>&1; then
    echo "install.sh: cmake is required but was not found in PATH." >&2
    exit 1
fi

# ECM is required by the top-level CMakeLists.txt. On Arch, install its package
# here so a fresh system can proceed without failing during CMake configuration.
if command -v pacman >/dev/null 2>&1 && ! pacman -Qq extra-cmake-modules >/dev/null 2>&1; then
    echo "install.sh: installing the required Arch package extra-cmake-modules."
    sudo pacman -S --needed extra-cmake-modules
fi

# Configure and build both components before installing either one. This avoids
# leaving a partially updated installation when a later build step fails.
cmake -S "$SCRIPT_DIR" -B "$BUILD_DIR/script" \
    -DBUILD_PLUGIN=OFF \
    -DCMAKE_INSTALL_PREFIX="$HOME/.local"
cmake --build "$BUILD_DIR/script"

cmake -S "$SCRIPT_DIR" -B "$BUILD_DIR/plugin" \
    -DINSTALL_SCRIPT=OFF \
    -DCMAKE_INSTALL_PREFIX=/usr
cmake --build "$BUILD_DIR/plugin"

cmake --install "$BUILD_DIR/script"
sudo cmake --install "$BUILD_DIR/plugin"
