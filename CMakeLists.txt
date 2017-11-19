cmake_minimum_required(VERSION 3.8)
project(244b_final)

set(CMAKE_CXX_STANDARD 11)

set(INCL ./include)
include_directories(${INCL})

file(GLOB SOURCE_FILES
        "include/*"
        "*.h"
        "*.cpp"
        )
add_library(244b_final ${SOURCE_FILES} main.cpp)
add_executable(testKademlia main.cpp)