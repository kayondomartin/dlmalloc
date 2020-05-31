#! /usr/bin/bash

CMAKELISTS_FILE = "CMakeLists.txt"
BUILD_FOLDER = "build"

if [! -f "$CMAKELISTS_FILE"]
then
    echo "No CMakeLists file. Terminating build ..."
    exit 1
fi 

if [! -d "$BUILD_FOLDER"]
then
    mkdir build
fi
cd build
cmake ..
make
./test
