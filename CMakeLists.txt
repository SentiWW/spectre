cmake_minimum_required(VERSION 3.10)

project(spectre)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add source files
file(GLOB SOURCES "src/attacker.cpp")

# Add header files
include_directories(include)

# Create executable
add_executable(spectre ${SOURCES})