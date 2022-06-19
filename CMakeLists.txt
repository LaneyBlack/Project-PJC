cmake_minimum_required(VERSION 3.21)
project(Project1)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-static")

add_executable(project main.cpp imageFile.h imageFile.cpp)