cmake_minimum_required(VERSION 3.8)
project(simple-kademlia)

set(CMAKE_CXX_STANDARD 11)

set(INCL ./include)
include_directories(${INCL})

file(GLOB SOURCE_FILES
        "include/*"
        "include/messages/*"
        "src/*"
        "src/messages/*"
        "test/*"
        )
add_library(simpleKademlia ${SOURCE_FILES})
add_executable(testSerialization test/serialization.cpp)
target_link_libraries(testSerialization simpleKademlia)

enable_testing()
