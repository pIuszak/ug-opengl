cmake_minimum_required(VERSION 3.15)
project(zombie)

set(CMAKE_CXX_STANDARD 14)

add_executable(zombie bones.cpp)
target_link_libraries(zombie -lSOIL -lfreeglut -lopengl32 -lglew32)