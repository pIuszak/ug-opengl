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
CMAKE_SOURCE_DIR = C:\Dev\OpenGL\lab9\zombie

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Dev\OpenGL\lab9\zombie

# Include any dependencies generated for this target.
include CMakeFiles/zombie.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/zombie.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/zombie.dir/flags.make

CMakeFiles/zombie.dir/bones.cpp.obj: CMakeFiles/zombie.dir/flags.make
CMakeFiles/zombie.dir/bones.cpp.obj: bones.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Dev\OpenGL\lab9\zombie\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/zombie.dir/bones.cpp.obj"
	C:\mingw\mingw-w64\i686-8.1.0-posix-dwarf-rt_v6-rev0\mingw32\bin\g++.exe  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\zombie.dir\bones.cpp.obj -c C:\Dev\OpenGL\lab9\zombie\bones.cpp

CMakeFiles/zombie.dir/bones.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/zombie.dir/bones.cpp.i"
	C:\mingw\mingw-w64\i686-8.1.0-posix-dwarf-rt_v6-rev0\mingw32\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Dev\OpenGL\lab9\zombie\bones.cpp > CMakeFiles\zombie.dir\bones.cpp.i

CMakeFiles/zombie.dir/bones.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/zombie.dir/bones.cpp.s"
	C:\mingw\mingw-w64\i686-8.1.0-posix-dwarf-rt_v6-rev0\mingw32\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Dev\OpenGL\lab9\zombie\bones.cpp -o CMakeFiles\zombie.dir\bones.cpp.s

# Object files for target zombie
zombie_OBJECTS = \
"CMakeFiles/zombie.dir/bones.cpp.obj"

# External object files for target zombie
zombie_EXTERNAL_OBJECTS =

zombie.exe: CMakeFiles/zombie.dir/bones.cpp.obj
zombie.exe: CMakeFiles/zombie.dir/build.make
zombie.exe: CMakeFiles/zombie.dir/linklibs.rsp
zombie.exe: CMakeFiles/zombie.dir/objects1.rsp
zombie.exe: CMakeFiles/zombie.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Dev\OpenGL\lab9\zombie\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable zombie.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\zombie.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/zombie.dir/build: zombie.exe

.PHONY : CMakeFiles/zombie.dir/build

CMakeFiles/zombie.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\zombie.dir\cmake_clean.cmake
.PHONY : CMakeFiles/zombie.dir/clean

CMakeFiles/zombie.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Dev\OpenGL\lab9\zombie C:\Dev\OpenGL\lab9\zombie C:\Dev\OpenGL\lab9\zombie C:\Dev\OpenGL\lab9\zombie C:\Dev\OpenGL\lab9\zombie\CMakeFiles\zombie.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/zombie.dir/depend

