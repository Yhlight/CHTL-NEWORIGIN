cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler-specific options
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Third-party libraries
include_directories(${CMAKE_SOURCE_DIR}/third-party)

# ANTLR4 setup (placeholder - will be configured when ANTLR4 is available)
# find_package(antlr4-runtime REQUIRED)

# Main source directories - only add existing ones
if(EXISTS ${CMAKE_SOURCE_DIR}/src/CHTL)
    add_subdirectory(src/CHTL)
endif()

# Other directories will be added as they are implemented
# if(EXISTS ${CMAKE_SOURCE_DIR}/src/CHTL_JS)
#     add_subdirectory(src/CHTL_JS)
# endif()
# if(EXISTS ${CMAKE_SOURCE_DIR}/src/SharedCore)
#     add_subdirectory(src/SharedCore)
# endif()
# if(EXISTS ${CMAKE_SOURCE_DIR}/src/CSS)
#     add_subdirectory(src/CSS)
# endif()
# if(EXISTS ${CMAKE_SOURCE_DIR}/src/JS)
#     add_subdirectory(src/JS)
# endif()
# if(EXISTS ${CMAKE_SOURCE_DIR}/src/Util)
#     add_subdirectory(src/Util)
# endif()
# if(EXISTS ${CMAKE_SOURCE_DIR}/src/Test)
#     add_subdirectory(src/Test)
# endif()

# Main executable
add_executable(chtl_compiler
    src/main.cpp
)

# Include directories for main executable
target_include_directories(chtl_compiler PRIVATE src/CHTL/include)

# Link libraries
target_link_libraries(chtl_compiler
    chtl_core
)

# Test configuration
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install configuration
install(TARGETS chtl_compiler
    RUNTIME DESTINATION bin
)

# Git hooks and version control
find_package(Git QUIET)
if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    add_definitions(-DGIT_HASH="${GIT_HASH}")
endif()

# Version information
configure_file(
    ${CMAKE_SOURCE_DIR}/src/version.h.in
    ${CMAKE_BINARY_DIR}/src/version.h
    @ONLY
)