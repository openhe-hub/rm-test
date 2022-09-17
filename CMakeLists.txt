cmake_minimum_required(VERSION 3.10)
project(rm_test)

set(CMAKE_CXX_STANDARD 17)

add_executable(ex1 ex1/ex1.cpp)

add_executable(ex2 ex2/ex2.cpp ex2/Pattern.cpp ex2/Group.cpp)
include_directories(ex2)

add_executable(ex3 ex3/ex3.cpp)

set(OpenCV_DIR D:\\software\\opencv\\OpenCV-MinGW-Build-OpenCV-4.5.5-x64)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(ex4 ex4/ex4.cpp)
target_link_libraries(ex4 ${OpenCV_LIBS})