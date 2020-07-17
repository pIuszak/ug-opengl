cmake_minimum_required(VERSION 3.15)
project(transparency)

set(CMAKE_CXX_STANDARD 14)

add_executable(transparency transparency.cpp)
target_link_libraries(transparency -lSOIL -lfreeglut -lopengl32 -lglew32)