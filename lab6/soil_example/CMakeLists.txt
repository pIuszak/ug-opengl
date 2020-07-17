cmake_minimum_required(VERSION 3.15)
project(soil_example)

set(CMAKE_CXX_STANDARD 14)

add_executable(soil_example soil_example.cpp)
target_link_libraries(soil_example -lSOIL -lfreeglut -lopengl32 -lglew32)