cmake_minimum_required(VERSION 3.10)
project(CHTL_Compiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(CHTL)
add_subdirectory(CHTLJS)
add_subdirectory(third-party/libzippp)
# add_subdirectory(third-party/antlr4/runtime/Cpp) # Not needed yet
# add_subdirectory(third-party/libsass) # libsass does not use CMake, will be added later
# add_subdirectory(third-party/quickjs) # Not needed yet

# Add an executable for the compiler
add_executable(chtl-compiler CHTL/main.cpp)
target_include_directories(chtl-compiler PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(chtl-compiler PRIVATE CHTL CHTLJS)

# Add the test subdirectory
add_subdirectory(Test)