#!/bin/sh

if [ -d "build" ]; then
    rm -rf build
fi

# Install widget for current user
cmake -B build/ -S . -DCMAKE_INSTALL_PREFIX=~/.local
cmake --build build/
cmake --install build/

# Install plugin system-wide (required for qml modules)
# cmake -B build/plugin -S . -DBUILD_PLUGIN=ON -DCMAKE_INSTALL_PREFIX=/usr
# cmake --build build/plugin
# sudo cmake --install build/plugin
