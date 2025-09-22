cmake_minimum_required(VERSION 3.10)
project(CHTL_Compiler)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/CHTL/CHTL
    ${PROJECT_SOURCE_DIR}/CHTL/ThirdParty/miniz
)

# Add source files
file(GLOB_RECURSE SOURCES "CHTL/*.cpp")

# Add the miniz library source
set(SOURCES ${SOURCES} CHTL/ThirdParty/miniz/miniz.c)

# Add the executable
add_executable(chtl_compiler ${SOURCES})

# Target-specific include directories
target_include_directories(chtl_compiler PUBLIC
    ${PROJECT_SOURCE_DIR}/CHTL/CHTL/CHTLLexer
    ${PROJECT_SOURCE_DIR}/CHTL/CHTL/CHTLNode
    ${PROJECT_SOURCE_DIR}/CHTL/CHTL/CHTLParser
    ${PROJECT_SOURCE_DIR}/CHTL/CHTL/CHTLGenerator
    ${PROJECT_SOURCE_DIR}/CHTL/CHTL/CHTLExpression
)
