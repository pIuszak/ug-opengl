cmake_minimum_required(VERSION 3.15)
project(AnotherWayToSurvive2)

set(CMAKE_CXX_STANDARD 14)

add_executable(AnotherWayToSurvive2 main.cpp)
target_link_libraries(AnotherWayToSurvive2 -lSOIL -lfreeglut -lopengl32 -lglew32)