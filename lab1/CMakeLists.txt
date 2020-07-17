cmake_minimum_required(VERSION 3.15)
project(lab1)

set(CMAKE_CXX_STANDARD 14)

add_executable(lab1 main.cpp)
target_link_libraries(lab1 -lfreeglut -lopengl32 -lglew32)