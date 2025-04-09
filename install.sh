#!/bin/sh

if [ -d "build" ]; then
    rm -rf build
fi

# Install script for current user
cmake -B build/script -S . -DBUILD_PLUGIN=OFF -DCMAKE_INSTALL_PREFIX=$HOME/.local
cmake --build build/script
cmake --install build/script

# Install plugin system-wide
cmake -B build/plugin -S . -DINSTALL_SCRIPT=OFF -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build/plugin
sudo cmake --install build/plugin
