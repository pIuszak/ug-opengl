cmake_minimum_required(VERSION 3.15)
project(ex5)

set(CMAKE_CXX_STANDARD 14)

add_executable(ex5 collision.cpp)
target_link_libraries(ex5 -lSOIL -lfreeglut -lopengl32 -lglew32)