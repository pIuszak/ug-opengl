cmake_minimum_required(VERSION 3.15)
project(lab5ex2and3and4)

set(CMAKE_CXX_STANDARD 14)

add_executable(lab5ex2and3and4 simple_light.cpp shader_stuff.h Light.cpp Light.h)
target_link_libraries(lab5ex2and3and4 -lfreeglut -lopengl32 -lglew32)