#! /usr/bin/bash

if [! -f "CMakeLists.txt"]
then
    echo "No CMakeLists file. Terminating build ..."
    exit 1
fi 

if [ ! -d "build"]
then
    mkdir build
fi
cd build
cmake ..
make
./test
