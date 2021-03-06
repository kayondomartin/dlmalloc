cmake_minimum_required(VERSION 3.11)
project(dlalloc C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_BUILD_TYPE "Release")
#set(CMAKE_BUILD_TYPE "Debug")

option(LOCKS "LOCKS" OFF)
option(FOOTERS "FOOTERS" OFF)

option(DEFINE_MACRO "Option description" ON)



if (NOT CMAKE_BUILD_TYPE)
    if ("${CMAKE_BINARY_DIR}" MATCHES ".*(D|d)ebug$")
        message(STATUS "No build type selected, default to *** Debug ***")
        set(CMAKE_BUILD_TYPE "Debug")
    else ()
        message(STATUS "No build type selected, default to *** Release ***")
        set(CMAKE_BUILD_TYPE "Release")
    endif ()
else ()
    message(STATUS "Build type specified as *** ${CMAKE_BUILD_TYPE} ***")
endif ()

if (CMAKE_C_COMPILER_ID MATCHES "AppleClang|Clang|GNU")
  list(APPEND dlalloc_flags -Wall -Wextra -ldl)
endif ()

if (CMAKE_BUILD_TYPE MATCHES "(D|d)ebug")
  list(APPEND dlalloc_defines DEBUG=1)
  list(APPEND dlalloc_flags -Wall -Wextra -DDBG2 -gdwarf-2 -g3)
else()
  #list(APPEND dlalloc_flags -D INTEL)
endif ()

if (ARM64)
   message(STATUS "NOMAD : ARM64")
  list(APPEND dlalloc_flags -march=armv8.5-a+memtag)
  ADD_DEFINITIONS(-DAARCH64)
endif ()

#cmake -DEMULATE_SBRK=1 ..  
if(EMULATE_SBRK)
   message(STATUS "NOMAD : EMULATE_SBRK")
  ADD_DEFINITIONS(-DEMULATE_SBRK=1) 
else()
  #ADD_DEFINITIONS(-DEMULATE_SBRK=0) 
endif()

#cmake -DTEST_MEMORY=1 ..  
if(TEST_MEMORY)
   message(STATUS "NOMAD : TEST_MEMORY")
  ADD_DEFINITIONS(-DTEST_MEMORY=1) 
else()
  ADD_DEFINITIONS(-DTEST_MEMORY=0)
endif()

#cmake -DANALYZE_NOMAD=1 ..  
if(ANALYZE_NOMAD)
   message(STATUS "NOMAD : ANALYZE")
  ADD_DEFINITIONS(-DANALYZE_NOMAD=1) 
else()
  ADD_DEFINITIONS(-DANALYZE_NOMAD=0)
endif()

#cmake -DTEST_CONSOLIDATION=1 ..  
if(TEST_CONSOLIDATION)
   message(STATUS "NOMAD : TEST_CONSOLIDATION")
  ADD_DEFINITIONS(-DTEST_CONSOLIDATION=1) 
else()
  ADD_DEFINITIONS(-DTEST_CONSOLIDATION=0)
endif()


#cmake -DDISABLE_CONSOLIDATION=1 ..  
if(DISABLE_CONSOLIDATION)
   message(STATUS "NOMAD : DISABLE CONSOLIDATION")
  ADD_DEFINITIONS(-DDISABLE_CONSOLIDATION=1) 
else()
  ADD_DEFINITIONS(-DDISABLE_CONSOLIDATION=0)
endif()

#cmake -DDECOMPOSE_OVERHEAD=1 ..  
if(DECOMPOSE_OVERHEAD)
   message(STATUS "NOMAD : DECOMPOSE OVERHEAD")
  ADD_DEFINITIONS(-DDECOMPOSE_OVERHEAD=1) 
else()
  ADD_DEFINITIONS(-DDECOMPOSE_OVERHEAD=0)
endif()


if (LOCKS MATCHES "ON")
    list(APPEND dlalloc_defines USE_LOCKS=1)
endif ()

if (FOOTERS MATCHES "ON")
    list(APPEND dlalloc_defines USE_FOOTERS=1)
endif ()

set(dlalloc_sources
        src/chunk.c
        src/debug.c
        src/error.c
        src/heap-default.c
        src/heap-user.c
        src/heap.c
        src/init.c
        src/inspect.c
        src/lock.c
        src/log.c
        src/os.c
        src/sbrk.c
        src/segment.c
        src/redblack.c
        src/state.c)
add_library(dlalloc-static STATIC ${dlalloc_sources})
target_compile_options(dlalloc-static PRIVATE ${dlalloc_flags})
target_compile_definitions(dlalloc-static PRIVATE ${dlalloc_defines})
target_link_libraries(dlalloc-static PRIVATE pthread)
target_include_directories(dlalloc-static PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

add_library(dlalloc SHARED ${dlalloc_sources})
target_compile_options(dlalloc PRIVATE ${dlalloc_flags})
target_compile_definitions(dlalloc PRIVATE ${dlalloc_defines})
target_link_libraries(dlalloc PRIVATE pthread)
target_include_directories(dlalloc PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

add_executable(test src/test.c src/log.c)
target_link_libraries(test PRIVATE dlalloc)
target_include_directories(test PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
