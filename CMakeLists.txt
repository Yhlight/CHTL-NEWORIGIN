cmake_minimum_required(VERSION 3.10)
project(CHTL_Project)

# Set C++ standard to 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the CHTL subdirectory
add_subdirectory(CHTL)

# Enable testing and add the Test subdirectory
enable_testing()
add_subdirectory(Test)