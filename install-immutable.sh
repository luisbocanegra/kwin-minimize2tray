#!/bin/sh

if [ -d "build" ]; then
    rm -rf build
fi

# Install widget and C++ plugin for current user
#
# NOTE: For the C++ plugin to work adding `$HOME/.local/lib/qml` to `QML_IMPORT_PATH` is needed
# in `$HOME/.config/plasma-workspace/env/path.sh` add:
# export QML_IMPORT_PATH="$HOME/.local/lib/qml:$QML_IMPORT_PATH"
#
# For more information see https://userbase.kde.org/Session_Environment_Variables
cmake -B build/ -S . -DCMAKE_INSTALL_PREFIX="$HOME/.local"
cmake --build build/
cmake --install build/
