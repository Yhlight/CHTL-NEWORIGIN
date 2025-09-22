cmake_minimum_required(VERSION 3.10)
project(CHTL_Compiler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the CHTL project subdirectory
add_subdirectory(CHTL)

# Define the main executable
add_executable(chtl_compiler main.cpp)

# Link the executable against the CHTL library
target_link_libraries(chtl_compiler PRIVATE CHTL)
