cmake_minimum_required(VERSION 3.10)
project(recognition)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE RELEASE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -O3 -g")

include_directories(${CMAKE_SOURCE_DIR}/include/)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

file(GLOB_RECURSE sources 
    ${CMAKE_SOURCE_DIR}/src/*.cc
    ${CMAKE_SOURCE_DIR}/main.cc
)
add_executable(recognition ${sources})

target_link_libraries(recognition ${OpenCV_LIBS})
   