cmake_minimum_required(VERSION 3.15)
project(ex1)

set(CMAKE_CXX_STANDARD 14)

add_executable(ex1 objfile.cpp)
target_link_libraries(ex1 -lSOIL -lfreeglut -lopengl32 -lglew32)