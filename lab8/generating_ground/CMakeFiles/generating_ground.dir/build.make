# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.16

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2020.1.1\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2020.1.1\bin\cmake\win\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Dev\OpenGL\lab8\generating_ground

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Dev\OpenGL\lab8\generating_ground

# Include any dependencies generated for this target.
include CMakeFiles/generating_ground.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/generating_ground.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/generating_ground.dir/flags.make

CMakeFiles/generating_ground.dir/ground.cpp.obj: CMakeFiles/generating_ground.dir/flags.make
CMakeFiles/generating_ground.dir/ground.cpp.obj: ground.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Dev\OpenGL\lab8\generating_ground\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/generating_ground.dir/ground.cpp.obj"
	C:\mingw\mingw-w64\i686-8.1.0-posix-dwarf-rt_v6-rev0\mingw32\bin\g++.exe  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\generating_ground.dir\ground.cpp.obj -c C:\Dev\OpenGL\lab8\generating_ground\ground.cpp

CMakeFiles/generating_ground.dir/ground.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/generating_ground.dir/ground.cpp.i"
	C:\mingw\mingw-w64\i686-8.1.0-posix-dwarf-rt_v6-rev0\mingw32\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Dev\OpenGL\lab8\generating_ground\ground.cpp > CMakeFiles\generating_ground.dir\ground.cpp.i

CMakeFiles/generating_ground.dir/ground.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/generating_ground.dir/ground.cpp.s"
	C:\mingw\mingw-w64\i686-8.1.0-posix-dwarf-rt_v6-rev0\mingw32\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Dev\OpenGL\lab8\generating_ground\ground.cpp -o CMakeFiles\generating_ground.dir\ground.cpp.s

# Object files for target generating_ground
generating_ground_OBJECTS = \
"CMakeFiles/generating_ground.dir/ground.cpp.obj"

# External object files for target generating_ground
generating_ground_EXTERNAL_OBJECTS =

generating_ground.exe: CMakeFiles/generating_ground.dir/ground.cpp.obj
generating_ground.exe: CMakeFiles/generating_ground.dir/build.make
generating_ground.exe: CMakeFiles/generating_ground.dir/linklibs.rsp
generating_ground.exe: CMakeFiles/generating_ground.dir/objects1.rsp
generating_ground.exe: CMakeFiles/generating_ground.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Dev\OpenGL\lab8\generating_ground\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable generating_ground.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\generating_ground.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/generating_ground.dir/build: generating_ground.exe

.PHONY : CMakeFiles/generating_ground.dir/build

CMakeFiles/generating_ground.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\generating_ground.dir\cmake_clean.cmake
.PHONY : CMakeFiles/generating_ground.dir/clean

CMakeFiles/generating_ground.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Dev\OpenGL\lab8\generating_ground C:\Dev\OpenGL\lab8\generating_ground C:\Dev\OpenGL\lab8\generating_ground C:\Dev\OpenGL\lab8\generating_ground C:\Dev\OpenGL\lab8\generating_ground\CMakeFiles\generating_ground.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/generating_ground.dir/depend
