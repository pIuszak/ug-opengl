cmake_minimum_required(VERSION 3.15)
project(light_material)

set(CMAKE_CXX_STANDARD 14)

add_executable(light_material light_material.cpp)
target_link_libraries(light_material -lSOIL -lfreeglut -lopengl32 -lglew32)