cmake_minimum_required(VERSION 3.8)
project(simple-kademlia)

set(CMAKE_CXX_STANDARD 11)

find_package(Boost REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS})

set(INCL ./include)
include_directories(${INCL})

file(GLOB SOURCE_FILES
        "src/*"
        "src/node/*"
        )

add_library(simpleKademlia ${SOURCE_FILES} )
add_executable(tests test/serialization.cpp test/testNetworking.cpp)
target_link_libraries(tests simpleKademlia ${Boost_LIBRARIES} -lpthread)

enable_testing()