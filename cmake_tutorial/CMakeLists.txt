cmake_minimum_required(VERSION 4.3.2)
project(hello_world 
LANGUAGES CXX )
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)
add_executable(hello main.cpp) 