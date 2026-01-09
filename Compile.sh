#!/bin/bash

# This script expects to be run in the project root directory
# For Windows environments, a MINGW32 shell is expected

set -e
OS=$(uname -s)


BUILD_DIR="build"

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"


if [ ! -f "CMakeCache.txt" ]; then
    echo "Running CMake configuration.."

    if [["$OS" == "MINGW"*]] || [["$OS" == "MSYS"*]] || [["$OS" == "Windows_NT"*]]; then
        cmake -G "MinGW Makefiles" ..
    else
        cmake ..
    fi
else
    echo "build directory already configured."
fi

cmake --build .
echo # line break
echo == Running Tests ==
cd ..
./bin/RunTests.exe
