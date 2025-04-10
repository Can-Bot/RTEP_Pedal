#!/bin/bash

set -e

original_directory=$(pwd)
script_directory=$(cd "$(dirname "$0")"; pwd)

cd "$script_directory"

# Clean and recreate build directory
rm -rf build
mkdir -p build
cd build

# Update this path only if your vcpkg is not under lib/
VCPKG_PATH="$script_directory/lib/vcpkg"
VCPKG_TOOLCHAIN_FILE="$VCPKG_PATH/scripts/buildsystems/vcpkg.cmake"

if [ ! -f "$VCPKG_TOOLCHAIN_FILE" ]; then
    echo "ERROR: vcpkg.cmake not found at $VCPKG_TOOLCHAIN_FILE"
    echo "Make sure vcpkg is cloned into: $VCPKG_PATH"
    exit 1
fi

cmake .. -DCMAKE_TOOLCHAIN_FILE="$VCPKG_TOOLCHAIN_FILE" -G "Unix Makefiles" 2>&1 | tee cmake_errors.log
if [ $? -ne 0 ]; then
    echo "CMake configuration failed. Check cmake_errors.log"
    exit 1
fi

make 2>&1 | tee make_errors.log
if [ $? -ne 0 ]; then
    echo "Build failed. Check make_errors.log"
    exit 1
fi

cd "$original_directory"

if [ -f "$script_directory/build/src/shred_pedal" ]; then
    echo "Application built successfully: $script_directory/build/src/shred_pedal"
else
    echo "Build completed, but executable not found. Check make_errors.log"
    exit 1
fi
