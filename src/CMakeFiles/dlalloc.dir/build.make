# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.11

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/iybang/dlmalloc

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/iybang/dlmalloc/src

# Include any dependencies generated for this target.
include CMakeFiles/dlalloc.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/dlalloc.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/dlalloc.dir/flags.make

CMakeFiles/dlalloc.dir/chunk.c.o: CMakeFiles/dlalloc.dir/flags.make
CMakeFiles/dlalloc.dir/chunk.c.o: chunk.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/iybang/dlmalloc/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/dlalloc.dir/chunk.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dlalloc.dir/chunk.c.o   -c /home/iybang/dlmalloc/src/chunk.c

CMakeFiles/dlalloc.dir/chunk.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dlalloc.dir/chunk.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/iybang/dlmalloc/src/chunk.c > CMakeFiles/dlalloc.dir/chunk.c.i

CMakeFiles/dlalloc.dir/chunk.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dlalloc.dir/chunk.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/iybang/dlmalloc/src/chunk.c -o CMakeFiles/dlalloc.dir/chunk.c.s

CMakeFiles/dlalloc.dir/debug.c.o: CMakeFiles/dlalloc.dir/flags.make
CMakeFiles/dlalloc.dir/debug.c.o: debug.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/iybang/dlmalloc/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/dlalloc.dir/debug.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dlalloc.dir/debug.c.o   -c /home/iybang/dlmalloc/src/debug.c

CMakeFiles/dlalloc.dir/debug.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dlalloc.dir/debug.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/iybang/dlmalloc/src/debug.c > CMakeFiles/dlalloc.dir/debug.c.i

CMakeFiles/dlalloc.dir/debug.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dlalloc.dir/debug.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/iybang/dlmalloc/src/debug.c -o CMakeFiles/dlalloc.dir/debug.c.s

CMakeFiles/dlalloc.dir/error.c.o: CMakeFiles/dlalloc.dir/flags.make
CMakeFiles/dlalloc.dir/error.c.o: error.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/iybang/dlmalloc/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/dlalloc.dir/error.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dlalloc.dir/error.c.o   -c /home/iybang/dlmalloc/src/error.c

CMakeFiles/dlalloc.dir/error.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dlalloc.dir/error.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/iybang/dlmalloc/src/error.c > CMakeFiles/dlalloc.dir/error.c.i

CMakeFiles/dlalloc.dir/error.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dlalloc.dir/error.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/iybang/dlmalloc/src/error.c -o CMakeFiles/dlalloc.dir/error.c.s

CMakeFiles/dlalloc.dir/heap-default.c.o: CMakeFiles/dlalloc.dir/flags.make
CMakeFiles/dlalloc.dir/heap-default.c.o: heap-default.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/iybang/dlmalloc/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/dlalloc.dir/heap-default.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dlalloc.dir/heap-default.c.o   -c /home/iybang/dlmalloc/src/heap-default.c

CMakeFiles/dlalloc.dir/heap-default.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dlalloc.dir/heap-default.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/iybang/dlmalloc/src/heap-default.c > CMakeFiles/dlalloc.dir/heap-default.c.i

CMakeFiles/dlalloc.dir/heap-default.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dlalloc.dir/heap-default.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/iybang/dlmalloc/src/heap-default.c -o CMakeFiles/dlalloc.dir/heap-default.c.s

CMakeFiles/dlalloc.dir/heap-user.c.o: CMakeFiles/dlalloc.dir/flags.make
CMakeFiles/dlalloc.dir/heap-user.c.o: heap-user.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/iybang/dlmalloc/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/dlalloc.dir/heap-user.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dlalloc.dir/heap-user.c.o   -c /home/iybang/dlmalloc/src/heap-user.c

CMakeFiles/dlalloc.dir/heap-user.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dlalloc.dir/heap-user.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/iybang/dlmalloc/src/heap-user.c > CMakeFiles/dlalloc.dir/heap-user.c.i

CMakeFiles/dlalloc.dir/heap-user.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dlalloc.dir/heap-user.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/iybang/dlmalloc/src/heap-user.c -o CMakeFiles/dlalloc.dir/heap-user.c.s

CMakeFiles/dlalloc.dir/heap.c.o: CMakeFiles/dlalloc.dir/flags.make
CMakeFiles/dlalloc.dir/heap.c.o: heap.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/iybang/dlmalloc/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/dlalloc.dir/heap.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dlalloc.dir/heap.c.o   -c /home/iybang/dlmalloc/src/heap.c

CMakeFiles/dlalloc.dir/heap.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dlalloc.dir/heap.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/iybang/dlmalloc/src/heap.c > CMakeFiles/dlalloc.dir/heap.c.i

CMakeFiles/dlalloc.dir/heap.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dlalloc.dir/heap.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/iybang/dlmalloc/src/heap.c -o CMakeFiles/dlalloc.dir/heap.c.s

CMakeFiles/dlalloc.dir/init.c.o: CMakeFiles/dlalloc.dir/flags.make
CMakeFiles/dlalloc.dir/init.c.o: init.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/iybang/dlmalloc/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object CMakeFiles/dlalloc.dir/init.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dlalloc.dir/init.c.o   -c /home/iybang/dlmalloc/src/init.c

CMakeFiles/dlalloc.dir/init.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dlalloc.dir/init.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/iybang/dlmalloc/src/init.c > CMakeFiles/dlalloc.dir/init.c.i

CMakeFiles/dlalloc.dir/init.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dlalloc.dir/init.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/iybang/dlmalloc/src/init.c -o CMakeFiles/dlalloc.dir/init.c.s

CMakeFiles/dlalloc.dir/inspect.c.o: CMakeFiles/dlalloc.dir/flags.make
CMakeFiles/dlalloc.dir/inspect.c.o: inspect.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/iybang/dlmalloc/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object CMakeFiles/dlalloc.dir/inspect.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dlalloc.dir/inspect.c.o   -c /home/iybang/dlmalloc/src/inspect.c

CMakeFiles/dlalloc.dir/inspect.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dlalloc.dir/inspect.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/iybang/dlmalloc/src/inspect.c > CMakeFiles/dlalloc.dir/inspect.c.i

CMakeFiles/dlalloc.dir/inspect.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dlalloc.dir/inspect.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/iybang/dlmalloc/src/inspect.c -o CMakeFiles/dlalloc.dir/inspect.c.s

CMakeFiles/dlalloc.dir/lock.c.o: CMakeFiles/dlalloc.dir/flags.make
CMakeFiles/dlalloc.dir/lock.c.o: lock.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/iybang/dlmalloc/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building C object CMakeFiles/dlalloc.dir/lock.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dlalloc.dir/lock.c.o   -c /home/iybang/dlmalloc/src/lock.c

CMakeFiles/dlalloc.dir/lock.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dlalloc.dir/lock.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/iybang/dlmalloc/src/lock.c > CMakeFiles/dlalloc.dir/lock.c.i

CMakeFiles/dlalloc.dir/lock.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dlalloc.dir/lock.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/iybang/dlmalloc/src/lock.c -o CMakeFiles/dlalloc.dir/lock.c.s

CMakeFiles/dlalloc.dir/log.c.o: CMakeFiles/dlalloc.dir/flags.make
CMakeFiles/dlalloc.dir/log.c.o: log.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/iybang/dlmalloc/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building C object CMakeFiles/dlalloc.dir/log.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dlalloc.dir/log.c.o   -c /home/iybang/dlmalloc/src/log.c

CMakeFiles/dlalloc.dir/log.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dlalloc.dir/log.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/iybang/dlmalloc/src/log.c > CMakeFiles/dlalloc.dir/log.c.i

CMakeFiles/dlalloc.dir/log.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dlalloc.dir/log.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/iybang/dlmalloc/src/log.c -o CMakeFiles/dlalloc.dir/log.c.s

CMakeFiles/dlalloc.dir/os.c.o: CMakeFiles/dlalloc.dir/flags.make
CMakeFiles/dlalloc.dir/os.c.o: os.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/iybang/dlmalloc/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building C object CMakeFiles/dlalloc.dir/os.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dlalloc.dir/os.c.o   -c /home/iybang/dlmalloc/src/os.c

CMakeFiles/dlalloc.dir/os.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dlalloc.dir/os.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/iybang/dlmalloc/src/os.c > CMakeFiles/dlalloc.dir/os.c.i

CMakeFiles/dlalloc.dir/os.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dlalloc.dir/os.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/iybang/dlmalloc/src/os.c -o CMakeFiles/dlalloc.dir/os.c.s

CMakeFiles/dlalloc.dir/sbrk.c.o: CMakeFiles/dlalloc.dir/flags.make
CMakeFiles/dlalloc.dir/sbrk.c.o: sbrk.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/iybang/dlmalloc/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building C object CMakeFiles/dlalloc.dir/sbrk.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dlalloc.dir/sbrk.c.o   -c /home/iybang/dlmalloc/src/sbrk.c

CMakeFiles/dlalloc.dir/sbrk.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dlalloc.dir/sbrk.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/iybang/dlmalloc/src/sbrk.c > CMakeFiles/dlalloc.dir/sbrk.c.i

CMakeFiles/dlalloc.dir/sbrk.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dlalloc.dir/sbrk.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/iybang/dlmalloc/src/sbrk.c -o CMakeFiles/dlalloc.dir/sbrk.c.s

CMakeFiles/dlalloc.dir/segment.c.o: CMakeFiles/dlalloc.dir/flags.make
CMakeFiles/dlalloc.dir/segment.c.o: segment.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/iybang/dlmalloc/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building C object CMakeFiles/dlalloc.dir/segment.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dlalloc.dir/segment.c.o   -c /home/iybang/dlmalloc/src/segment.c

CMakeFiles/dlalloc.dir/segment.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dlalloc.dir/segment.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/iybang/dlmalloc/src/segment.c > CMakeFiles/dlalloc.dir/segment.c.i

CMakeFiles/dlalloc.dir/segment.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dlalloc.dir/segment.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/iybang/dlmalloc/src/segment.c -o CMakeFiles/dlalloc.dir/segment.c.s

CMakeFiles/dlalloc.dir/redblack.c.o: CMakeFiles/dlalloc.dir/flags.make
CMakeFiles/dlalloc.dir/redblack.c.o: redblack.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/iybang/dlmalloc/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Building C object CMakeFiles/dlalloc.dir/redblack.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dlalloc.dir/redblack.c.o   -c /home/iybang/dlmalloc/src/redblack.c

CMakeFiles/dlalloc.dir/redblack.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dlalloc.dir/redblack.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/iybang/dlmalloc/src/redblack.c > CMakeFiles/dlalloc.dir/redblack.c.i

CMakeFiles/dlalloc.dir/redblack.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dlalloc.dir/redblack.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/iybang/dlmalloc/src/redblack.c -o CMakeFiles/dlalloc.dir/redblack.c.s

CMakeFiles/dlalloc.dir/state.c.o: CMakeFiles/dlalloc.dir/flags.make
CMakeFiles/dlalloc.dir/state.c.o: state.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/iybang/dlmalloc/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_15) "Building C object CMakeFiles/dlalloc.dir/state.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dlalloc.dir/state.c.o   -c /home/iybang/dlmalloc/src/state.c

CMakeFiles/dlalloc.dir/state.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dlalloc.dir/state.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/iybang/dlmalloc/src/state.c > CMakeFiles/dlalloc.dir/state.c.i

CMakeFiles/dlalloc.dir/state.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dlalloc.dir/state.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/iybang/dlmalloc/src/state.c -o CMakeFiles/dlalloc.dir/state.c.s

# Object files for target dlalloc
dlalloc_OBJECTS = \
"CMakeFiles/dlalloc.dir/chunk.c.o" \
"CMakeFiles/dlalloc.dir/debug.c.o" \
"CMakeFiles/dlalloc.dir/error.c.o" \
"CMakeFiles/dlalloc.dir/heap-default.c.o" \
"CMakeFiles/dlalloc.dir/heap-user.c.o" \
"CMakeFiles/dlalloc.dir/heap.c.o" \
"CMakeFiles/dlalloc.dir/init.c.o" \
"CMakeFiles/dlalloc.dir/inspect.c.o" \
"CMakeFiles/dlalloc.dir/lock.c.o" \
"CMakeFiles/dlalloc.dir/log.c.o" \
"CMakeFiles/dlalloc.dir/os.c.o" \
"CMakeFiles/dlalloc.dir/sbrk.c.o" \
"CMakeFiles/dlalloc.dir/segment.c.o" \
"CMakeFiles/dlalloc.dir/redblack.c.o" \
"CMakeFiles/dlalloc.dir/state.c.o"

# External object files for target dlalloc
dlalloc_EXTERNAL_OBJECTS =

libdlalloc.so: CMakeFiles/dlalloc.dir/chunk.c.o
libdlalloc.so: CMakeFiles/dlalloc.dir/debug.c.o
libdlalloc.so: CMakeFiles/dlalloc.dir/error.c.o
libdlalloc.so: CMakeFiles/dlalloc.dir/heap-default.c.o
libdlalloc.so: CMakeFiles/dlalloc.dir/heap-user.c.o
libdlalloc.so: CMakeFiles/dlalloc.dir/heap.c.o
libdlalloc.so: CMakeFiles/dlalloc.dir/init.c.o
libdlalloc.so: CMakeFiles/dlalloc.dir/inspect.c.o
libdlalloc.so: CMakeFiles/dlalloc.dir/lock.c.o
libdlalloc.so: CMakeFiles/dlalloc.dir/log.c.o
libdlalloc.so: CMakeFiles/dlalloc.dir/os.c.o
libdlalloc.so: CMakeFiles/dlalloc.dir/sbrk.c.o
libdlalloc.so: CMakeFiles/dlalloc.dir/segment.c.o
libdlalloc.so: CMakeFiles/dlalloc.dir/redblack.c.o
libdlalloc.so: CMakeFiles/dlalloc.dir/state.c.o
libdlalloc.so: CMakeFiles/dlalloc.dir/build.make
libdlalloc.so: CMakeFiles/dlalloc.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/iybang/dlmalloc/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_16) "Linking C shared library libdlalloc.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/dlalloc.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/dlalloc.dir/build: libdlalloc.so

.PHONY : CMakeFiles/dlalloc.dir/build

CMakeFiles/dlalloc.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/dlalloc.dir/cmake_clean.cmake
.PHONY : CMakeFiles/dlalloc.dir/clean

CMakeFiles/dlalloc.dir/depend:
	cd /home/iybang/dlmalloc/src && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/iybang/dlmalloc /home/iybang/dlmalloc /home/iybang/dlmalloc/src /home/iybang/dlmalloc/src /home/iybang/dlmalloc/src/CMakeFiles/dlalloc.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/dlalloc.dir/depend

