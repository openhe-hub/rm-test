cmake_minimum_required(VERSION 3.10)
project(rm_test)

set(CMAKE_CXX_STANDARD 17)

add_executable(rm_test main.cpp)

add_executable(ex1 ex1/ex1.cpp)

add_executable(ex2 ex2/ex2.cpp ex2/Pattern.cpp ex2/Group.cpp)
include_directories(ex2)
