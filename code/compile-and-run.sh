
#Copyright 2026 Alan Crispin <crispinalan@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

#!/usr/bin/env bash
set -e

# Path to where you extracted the CopperSpice Debian 13 binaries
CS_DIR="/home/yourusername/libs/copperspice-2.1.0-debian13-x64"

# 1. Recreate a clean build directory
if [ -d "build" ]; then
    echo "Removing existing build directory..."
    rm -rf build
fi
echo "Creating new build directory..."
mkdir build
cd build

# 2. Run CMake and compile
echo "Running CMake..."
cmake -DCMAKE_PREFIX_PATH="${CS_DIR}" -DCMAKE_BUILD_TYPE=Release ..
echo "Building project..."
make

# ====================================================================
# DIRECTLY POPULATE THE BUILD DIRECTORY FOR STANDALONE RUNNING
# ====================================================================
echo "Copying runtime libraries into the build directory..."
# Copy core libraries directly into the build root
cp "${CS_DIR}/lib/libCsCore"* .
cp "${CS_DIR}/lib/libCsGui"* .
# core SQL database library (needed)
cp "${CS_DIR}/lib/libCsSql"* .

cp "${CS_DIR}/lib/libCsXcbSupport"* . || true
cp "${CS_DIR}/lib/libCsWaylandClient"* . || true

echo "Setting up local platforms subdirectory..."
mkdir -p platforms
# Copy the window plugins into the required subfolder
cp "${CS_DIR}/lib/CsGuiXcb"* platforms/
cp "${CS_DIR}/lib/CsGuiWayland"* platforms/

# Execute the application natively from the build folder
echo "Executing standalone application under Wayland..."
export LD_LIBRARY_PATH=".:${LD_LIBRARY_PATH}"
export QT_QPA_PLATFORM="wayland"

./caspercalendar
