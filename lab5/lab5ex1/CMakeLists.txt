cmake_minimum_required(VERSION 3.15)
project(lab5ex1)

set(CMAKE_CXX_STANDARD 14)

add_executable(lab5ex1 simple_texture.cpp stb_image.h shader_s.h)
target_link_libraries(lab5ex1 -lfreeglut -lopengl32 -lglew32)