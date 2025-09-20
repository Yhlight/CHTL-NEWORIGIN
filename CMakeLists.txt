cmake_minimum_required(VERSION 3.20)
project(CHTL LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

# Source directories
set(CHTL_SRC_DIR ${CMAKE_SOURCE_DIR}/src/CHTL)
set(CHTLJS_SRC_DIR ${CMAKE_SOURCE_DIR}/src/CHTLJS)
set(SCANNER_SRC_DIR ${CMAKE_SOURCE_DIR}/src/Scanner)
set(UTIL_SRC_DIR ${CMAKE_SOURCE_DIR}/src/Util)
set(THIRD_PARTY_SRC_DIR ${CMAKE_SOURCE_DIR}/src/ThirdParty)

# CHTL Core Sources
file(GLOB_RECURSE CHTL_CORE_SOURCES
    ${CHTL_SRC_DIR}/CHTLContext/*.cpp
    ${CHTL_SRC_DIR}/CHTLLexer/*.cpp
    ${CHTL_SRC_DIR}/CHTLParser/*.cpp
    ${CHTL_SRC_DIR}/CHTLGenerator/*.cpp
    ${CHTL_SRC_DIR}/CHTLNode/*.cpp
    ${CHTL_SRC_DIR}/CHTLLoader/*.cpp
    ${CHTL_SRC_DIR}/CHTLState/*.cpp
    ${CHTL_SRC_DIR}/CMODSystem/*.cpp
)

# CHTL JS Sources
file(GLOB_RECURSE CHTLJS_SOURCES
    ${CHTLJS_SRC_DIR}/CHTLJSContext/*.cpp
    ${CHTLJS_SRC_DIR}/CHTLJSLexer/*.cpp
    ${CHTLJS_SRC_DIR}/CHTLJSParser/*.cpp
    ${CHTLJS_SRC_DIR}/CHTLJSGenerator/*.cpp
    ${CHTLJS_SRC_DIR}/CHTLJSNode/*.cpp
    ${CHTLJS_SRC_DIR}/CHTLJSLoader/*.cpp
    ${CHTLJS_SRC_DIR}/CHTLJSState/*.cpp
    ${CHTLJS_SRC_DIR}/CJMODSystem/*.cpp
)

# Scanner Sources
file(GLOB_RECURSE SCANNER_SOURCES
    ${SCANNER_SRC_DIR}/*.cpp
)

# Util Sources
file(GLOB_RECURSE UTIL_SOURCES
    ${UTIL_SRC_DIR}/FileSystem/*.cpp
    ${UTIL_SRC_DIR}/StringUtil/*.cpp
    ${UTIL_SRC_DIR}/ZipUtil/*.cpp
)

# Third Party Sources
file(GLOB_RECURSE THIRD_PARTY_SOURCES
    ${THIRD_PARTY_SRC_DIR}/*.cpp
)

# Main executable sources
file(GLOB_RECURSE MAIN_SOURCES
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/CompilerDispatcher.cpp
    ${CMAKE_SOURCE_DIR}/src/CodeMerger.cpp
)

# Combine all sources
set(ALL_SOURCES
    ${CHTL_CORE_SOURCES}
    ${CHTLJS_SOURCES}
    ${SCANNER_SOURCES}
    ${UTIL_SOURCES}
    ${THIRD_PARTY_SOURCES}
    ${MAIN_SOURCES}
)

# Create executable
add_executable(chtl ${ALL_SOURCES})

# Link libraries
find_package(Threads REQUIRED)
target_link_libraries(chtl Threads::Threads)

# Simple test target
add_executable(simple_test tests/simple_test.cpp)
target_link_libraries(simple_test Threads::Threads)
target_include_directories(simple_test PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)

# Test targets
enable_testing()
add_test(NAME SimpleTests COMMAND simple_test)

# Installation
install(TARGETS chtl DESTINATION bin)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/module DESTINATION share/chtl)