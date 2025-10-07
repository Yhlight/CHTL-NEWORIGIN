cmake_minimum_required(VERSION 3.10)
project(CHTL)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Include third-party directory
include_directories(third-party)

# Add subdirectories
add_subdirectory(CHTL)
add_subdirectory(Test)