# to build test code use: 
#   cmake .. -DBUILD_TESTS=ON

cmake_minimum_required(VERSION 3.10)
project(qoraal C)

# Set the C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

# Add subdirectories
add_subdirectory(src)

# Option to build tests
option(BUILD_TESTS "Build the test directory" OFF)

if(BUILD_TESTS)
    add_subdirectory(test)
endif()
