cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

# Third-party libraries
set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/third_party)

# Add miniz for compression (disabled for now)
# add_library(miniz STATIC ${CMAKE_SOURCE_DIR}/miniz.c)
# set_target_properties(miniz PROPERTIES LINKER_LANGUAGE C)

# Find required packages
find_package(Threads REQUIRED)

# Source files for library
set(CHTL_LIB_SOURCES
    src/CHTL/CHTLContext.cpp
    src/CHTL/CHTLGenerator.cpp
    src/CHTL/CHTLLexer.cpp
    src/CHTL/CHTLLoader.cpp
    src/CHTL/CHTLParser.cpp
    src/CHTL/CHTLState.cpp
    src/CHTL/CHTLStrategy.cpp
    src/CHTL/CHTLNode/BaseNode.cpp
    src/CHTL/CHTLNode/ElementNode.cpp
    src/CHTL/CHTLNode/TextNode.cpp
    src/CHTL/CHTLNode/CommentNode.cpp
    src/CHTL/CHTLNode/TemplateNode.cpp
    src/CHTL/CHTLNode/CustomNode.cpp
    src/CHTL/CHTLNode/StyleNode.cpp
    src/CHTL/CHTLNode/ScriptNode.cpp
    src/CHTL/CHTLNode/OriginNode.cpp
    src/CHTL/CHTLNode/ImportNode.cpp
    src/CHTL/CHTLNode/ConfigNode.cpp
    src/CHTL/CHTLNode/NamespaceNode.cpp
    src/CHTL/CHTLNode/OperatorNode.cpp
    src/CHTL/CMODSystem.cpp
    src/CHTL_JS/CHTLJSContext.cpp
    src/CHTL_JS/CHTLJSGenerator.cpp
    src/CHTL_JS/CHTLJSLexer.cpp
    src/CHTL_JS/CHTLJSLoader.cpp
    src/CHTL_JS/CHTLJSParser.cpp
    src/CHTL_JS/CHTLJSState.cpp
    src/CHTL_JS/CHTLJSStrategy.cpp
    src/CHTL_JS/CHTLJSNode/CHTLJSBaseNode.cpp
    src/CHTL_JS/CJMODSystem.cpp
    src/SharedCore/SharedContext.cpp
    src/Util/FileSystem/FileSystem.cpp
    src/Util/ZipUtil/ZipUtil.cpp
    src/Util/StringUtil/StringUtil.cpp
)

# Header files
set(CHTL_HEADERS
    include/CHTL/CHTLContext.h
    include/CHTL/CHTLGenerator.h
    include/CHTL/CHTLLexer.h
    include/CHTL/CHTLLoader.h
    include/CHTL/CHTLParser.h
    include/CHTL/CHTLState.h
    include/CHTL/CHTLStrategy.h
    include/CHTL/CHTLNode/BaseNode.h
    include/CHTL/CHTLNode/ElementNode.h
    include/CHTL/CHTLNode/TextNode.h
    include/CHTL/CHTLNode/CommentNode.h
    include/CHTL/CHTLNode/TemplateNode.h
    include/CHTL/CHTLNode/CustomNode.h
    include/CHTL/CHTLNode/StyleNode.h
    include/CHTL/CHTLNode/ScriptNode.h
    include/CHTL/CHTLNode/OriginNode.h
    include/CHTL/CHTLNode/ImportNode.h
    include/CHTL/CHTLNode/ConfigNode.h
    include/CHTL/CHTLNode/NamespaceNode.h
    include/CHTL/CHTLNode/OperatorNode.h
    include/CHTL/CMODSystem.h
    include/CHTL_JS/CHTLJSContext.h
    include/CHTL_JS/CHTLJSGenerator.h
    include/CHTL_JS/CHTLJSLexer.h
    include/CHTL_JS/CHTLJSLoader.h
    include/CHTL_JS/CHTLJSParser.h
    include/CHTL_JS/CHTLJSState.h
    include/CHTL_JS/CHTLJSStrategy.h
    include/CHTL_JS/CHTLJSNode/CHTLJSBaseNode.h
    include/CHTL_JS/CJMODSystem.h
    include/SharedCore/SharedContext.h
    include/Util/FileSystem/FileSystem.h
    include/Util/ZipUtil/ZipUtil.h
    include/Util/StringUtil/StringUtil.h
)

# Create the library
add_library(chtl_lib STATIC ${CHTL_LIB_SOURCES} ${CHTL_HEADERS})

# Create the main executable
add_executable(chtl src/main.cpp)

# Link libraries
target_link_libraries(chtl chtl_lib Threads::Threads)

# Test configuration
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation
install(TARGETS chtl DESTINATION bin)
install(FILES ${CHTL_HEADERS} DESTINATION include/CHTL)

# Create build directory structure
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/build)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/module)