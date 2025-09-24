cmake_minimum_required(VERSION 3.20)
project(CHTL LANGUAGES CXX)

# Project options
option(CHTL_BUILD_CLI "Build CLI tools" ON)
option(CHTL_BUILD_TESTS "Build tests" ON)
option(CHTL_ENABLE_SANITIZERS "Enable sanitizers" OFF)
option(CHTL_STRICT_WARNINGS "Enable strict warnings" ON)
option(CHTL_WITH_ANTLR "Enable ANTLR-based CSS/JS compilers" OFF)
option(CHTL_DEBUG_MODE "Enable debug mode" OFF)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Global compile definitions
if(CHTL_DEBUG_MODE)
    add_compile_definitions(CHTL_DEBUG_MODE=1)
endif()

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    if(CHTL_STRICT_WARNINGS)
        add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion)
    endif()
    
    if(CHTL_ENABLE_SANITIZERS)
        add_compile_options(-fsanitize=address -fsanitize=undefined)
        add_link_options(-fsanitize=address -fsanitize=undefined)
    endif()
endif()

# Platform-specific settings
if(UNIX AND NOT APPLE)
    add_compile_definitions(_FILE_OFFSET_BITS=64)
endif()

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(${CMAKE_SOURCE_DIR})

# Third-party dependencies
add_subdirectory(ThirdParty)

# Core utility libraries
add_subdirectory(Util)

# Scanner and dispatcher
add_subdirectory(Scanner)
add_subdirectory(CompilerDispatcher)
add_subdirectory(CodeMerger)

# Core compilers
add_subdirectory(CHTL)

# External compiler adapters
add_subdirectory(CSS)
add_subdirectory(JS)

# CLI tools
if(CHTL_BUILD_CLI)
    add_subdirectory(CLI)
endif()

# Test suite
if(CHTL_BUILD_TESTS)
    enable_testing()
    add_subdirectory(Test)
endif()

# Module system (placeholder)
# add_subdirectory(Module)

# Installation
install(TARGETS CHTL Scanner
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

if(CHTL_BUILD_CLI)
    install(TARGETS chtl
            RUNTIME DESTINATION bin)
endif()

# CPack configuration
set(CPACK_PACKAGE_NAME "CHTL")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CHTL - C++ HyperText Language")
set(CPACK_PACKAGE_VENDOR "CHTL Team")
set(CPACK_PACKAGE_LICENSE "MIT")

include(CPack)