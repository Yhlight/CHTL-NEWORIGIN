cmake_minimum_required(VERSION 3.10)
project(CHTL_Project CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing for the project
enable_testing()

# Add subdirectories for different components
add_subdirectory(CHTL)
add_subdirectory(Test)
# The following directories will be added later as their components are developed
# add_subdirectory(CHTL_JS)
# add_subdirectory(SharedCore)
# add_subdirectory(Util)