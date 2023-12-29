# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.27

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/nate/Documents/repos/broccoli/vendor/spirv_cross-1.1.5

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/nate/Documents/repos/broccoli/vendor/spirv_cross-1.1.5/build

# Include any dependencies generated for this target.
include CMakeFiles/spirv-cross-hlsl.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/spirv-cross-hlsl.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/spirv-cross-hlsl.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/spirv-cross-hlsl.dir/flags.make

CMakeFiles/spirv-cross-hlsl.dir/spirv_hlsl.cpp.o: CMakeFiles/spirv-cross-hlsl.dir/flags.make
CMakeFiles/spirv-cross-hlsl.dir/spirv_hlsl.cpp.o: /home/nate/Documents/repos/broccoli/vendor/spirv_cross-1.1.5/spirv_hlsl.cpp
CMakeFiles/spirv-cross-hlsl.dir/spirv_hlsl.cpp.o: CMakeFiles/spirv-cross-hlsl.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/nate/Documents/repos/broccoli/vendor/spirv_cross-1.1.5/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/spirv-cross-hlsl.dir/spirv_hlsl.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/spirv-cross-hlsl.dir/spirv_hlsl.cpp.o -MF CMakeFiles/spirv-cross-hlsl.dir/spirv_hlsl.cpp.o.d -o CMakeFiles/spirv-cross-hlsl.dir/spirv_hlsl.cpp.o -c /home/nate/Documents/repos/broccoli/vendor/spirv_cross-1.1.5/spirv_hlsl.cpp

CMakeFiles/spirv-cross-hlsl.dir/spirv_hlsl.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/spirv-cross-hlsl.dir/spirv_hlsl.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nate/Documents/repos/broccoli/vendor/spirv_cross-1.1.5/spirv_hlsl.cpp > CMakeFiles/spirv-cross-hlsl.dir/spirv_hlsl.cpp.i

CMakeFiles/spirv-cross-hlsl.dir/spirv_hlsl.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/spirv-cross-hlsl.dir/spirv_hlsl.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nate/Documents/repos/broccoli/vendor/spirv_cross-1.1.5/spirv_hlsl.cpp -o CMakeFiles/spirv-cross-hlsl.dir/spirv_hlsl.cpp.s

# Object files for target spirv-cross-hlsl
spirv__cross__hlsl_OBJECTS = \
"CMakeFiles/spirv-cross-hlsl.dir/spirv_hlsl.cpp.o"

# External object files for target spirv-cross-hlsl
spirv__cross__hlsl_EXTERNAL_OBJECTS =

libspirv-cross-hlsl.a: CMakeFiles/spirv-cross-hlsl.dir/spirv_hlsl.cpp.o
libspirv-cross-hlsl.a: CMakeFiles/spirv-cross-hlsl.dir/build.make
libspirv-cross-hlsl.a: CMakeFiles/spirv-cross-hlsl.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/nate/Documents/repos/broccoli/vendor/spirv_cross-1.1.5/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libspirv-cross-hlsl.a"
	$(CMAKE_COMMAND) -P CMakeFiles/spirv-cross-hlsl.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/spirv-cross-hlsl.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/spirv-cross-hlsl.dir/build: libspirv-cross-hlsl.a
.PHONY : CMakeFiles/spirv-cross-hlsl.dir/build

CMakeFiles/spirv-cross-hlsl.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/spirv-cross-hlsl.dir/cmake_clean.cmake
.PHONY : CMakeFiles/spirv-cross-hlsl.dir/clean

CMakeFiles/spirv-cross-hlsl.dir/depend:
	cd /home/nate/Documents/repos/broccoli/vendor/spirv_cross-1.1.5/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/nate/Documents/repos/broccoli/vendor/spirv_cross-1.1.5 /home/nate/Documents/repos/broccoli/vendor/spirv_cross-1.1.5 /home/nate/Documents/repos/broccoli/vendor/spirv_cross-1.1.5/build /home/nate/Documents/repos/broccoli/vendor/spirv_cross-1.1.5/build /home/nate/Documents/repos/broccoli/vendor/spirv_cross-1.1.5/build/CMakeFiles/spirv-cross-hlsl.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/spirv-cross-hlsl.dir/depend

