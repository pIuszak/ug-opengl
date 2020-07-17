cmake_minimum_required(VERSION 3.15)
project(shadow_mapping)

set(CMAKE_CXX_STANDARD 14)

add_executable(shadow_mapping shadow_mapping.cpp shadow_mapping.cpp)
target_link_libraries(shadow_mapping -lSOIL -lfreeglut -lopengl32 -lglew32)