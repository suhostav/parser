cmake_minimum_required(VERSION 3.11)

project(parser CXX)
set(CMAKE_CXX_STANDARD 23)
add_executable(parser
    src/parser.h 
    src/parser.cpp 
    src/main.cpp 
    src/test.cpp
)
