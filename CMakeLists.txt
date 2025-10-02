cmake_minimum_required(VERSION 3.10)
project(CHTL_Compiler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(CHTL)
add_subdirectory("CHTL JS")
add_subdirectory(SharedCore)
add_subdirectory(Util)
add_subdirectory(Test)