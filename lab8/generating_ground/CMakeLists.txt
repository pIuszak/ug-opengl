cmake_minimum_required(VERSION 3.15)
project(generating_ground)

set(CMAKE_CXX_STANDARD 14)

add_executable(generating_ground ground.cpp)
target_link_libraries(generating_ground -lSOIL -lfreeglut -lopengl32 -lglew32)