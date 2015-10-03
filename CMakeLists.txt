cmake_minimum_required(VERSION 3.2)
project(boost_optional)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(boost_optional ${SOURCE_FILES})

add_executable(main optional.h main.cpp)
