cmake_minimum_required(VERSION 3.15)
project(uniform_block)

set(CMAKE_CXX_STANDARD 14)

add_executable(uniform_block uniform_block.cpp)
target_link_libraries(uniform_block -lSOIL -lfreeglut -lopengl32 -lglew32)