cmake_minimum_required(VERSION 3.15)
project(rendering_pozaekranowy)

set(CMAKE_CXX_STANDARD 14)

add_executable(rendering_pozaekranowy rendering_pozaekranowy.cpp)
target_link_libraries(rendering_pozaekranowy -lSOIL -lfreeglut -lopengl32 -lglew32)