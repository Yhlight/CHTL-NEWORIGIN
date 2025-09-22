# Set the minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Project definition
project(CHTL_Compiler CXX)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing with CTest
enable_testing()

# Add the main source directory which contains its own CMakeLists.txt
add_subdirectory(CHTL)
