cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES C CXX)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Third-party libraries
add_library(miniz STATIC miniz.c)
set_target_properties(miniz PROPERTIES LINKER_LANGUAGE C)

# Main CHTL library
set(CHTL_SOURCES
    src/CHTL/CHTLContext.cpp
    src/CHTL/CHTLGenerator.cpp
    src/CHTL/CHTLLoader.cpp
    src/CHTL/CHTLManage.cpp
    src/CHTL/CHTLParser.cpp
    src/CHTL/CHTLState.cpp
    src/CHTL/CHTLStrategy.cpp
    src/CHTL/CHTLIOStream.cpp
    src/CHTL/CMODSystem.cpp
    src/CHTL/CHTLNode/BaseNode.cpp
    src/CHTL/CHTLNode/ElementNode.cpp
    src/CHTL/CHTLLexer/Lexer.cpp
    src/CHTL/CHTLLexer/GlobalMap.cpp
    src/CHTL/CHTLLexer/Token.cpp
)

set(CHTL_HEADERS
    include/CHTL/CHTLContext.h
    include/CHTL/CHTLGenerator.h
    include/CHTL/CHTLLexer.h
    include/CHTL/CHTLLoader.h
    include/CHTL/CHTLManage.h
    include/CHTL/CHTLParser.h
    include/CHTL/CHTLState.h
    include/CHTL/CHTLStrategy.h
    include/CHTL/CHTLIOStream.h
    include/CHTL/CMODSystem.h
    include/CHTL/CHTLNode/BaseNode.h
    include/CHTL/CHTLNode/ElementNode.h
    include/CHTL/CHTLLexer/Lexer.h
    include/CHTL/CHTLLexer/GlobalMap.h
    include/CHTL/CHTLLexer/Token.h
)

# CHTL JS library (stub for now)
set(CHTLJS_SOURCES
)

set(CHTLJS_HEADERS
)

# Shared core library (stub for now)
set(SHARED_SOURCES
)

set(SHARED_HEADERS
)

# Utility libraries (stub for now)
set(UTIL_SOURCES
)

set(UTIL_HEADERS
)

# Create libraries
add_library(chtl_core ${CHTL_SOURCES} ${CHTL_HEADERS})
target_link_libraries(chtl_core miniz)

# Main executable
add_executable(chtl src/main.cpp)
target_link_libraries(chtl chtl_core miniz)

# Testing
enable_testing()
add_subdirectory(tests)

# Installation
install(TARGETS chtl DESTINATION bin)
install(TARGETS chtl_core DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)