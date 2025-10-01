cmake_minimum_required(VERSION 3.10)
project(CHTL)

# Set C++ standard to 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the source directory
add_subdirectory(src)

# Enable testing
enable_testing()
add_subdirectory(Test)