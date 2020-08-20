# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
#this one not so much
SET(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
SET(CMAKE_C_COMPILER   /usr/bin/aarch64-linux-gnu-gcc-10)
SET(CMAKE_CXX_COMPILER /usr/bin/aarch64-linux-gnu-cpp-10)
option(ARM64 "Option description" ON)
option(EMULATE_SBRK "Option description" ON)
# where is the target environment
SET(CMAKE_FIND_ROOT_PATH /usr/aarch64-linux-gnu)
# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
# end of the file
