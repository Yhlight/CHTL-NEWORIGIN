# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(CHTL_Compiler CXX)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Define CHTL Library ---
# This makes it easy to link all CHTL source files to multiple executables.

# Define the source files for the CHTL library explicitly
set(CHTL_SOURCES
    ${CMAKE_SOURCE_DIR}/src/CHTL/CHTLGenerator/Generator.cpp
    ${CMAKE_SOURCE_DIR}/src/CHTL/CHTLLexer/Lexer.cpp
    ${CMAKE_SOURCE_DIR}/src/CHTL/CHTLLoader/Loader.cpp
    ${CMAKE_SOURCE_DIR}/src/CHTL/CHTLManage/TemplateManager.cpp
    ${CMAKE_SOURCE_DIR}/src/CHTL/CHTLNode/CommentNode.cpp
    ${CMAKE_SOURCE_DIR}/src/CHTL/CHTLNode/ElementNode.cpp
    ${CMAKE_SOURCE_DIR}/src/CHTL/CHTLNode/TextNode.cpp
    ${CMAKE_SOURCE_DIR}/src/CHTL/CHTLParser/Parser.cpp
    ${CMAKE_SOURCE_DIR}/src/CHTL/CHTLState/ConfigurationState.cpp
    ${CMAKE_SOURCE_DIR}/src/CHTL/CHTLState/StatementState.cpp
    ${CMAKE_SOURCE_DIR}/src/CHTL/CHTLState/StyleBlockState.cpp
    ${CMAKE_SOURCE_DIR}/src/CHTL/Util/ASTUtil.cpp
    ${CMAKE_SOURCE_DIR}/src/CHTL/Util/NodeCloner.cpp
    ${CMAKE_SOURCE_DIR}/src/CHTL/Util/StyleUtil.cpp
)

# Add a library for the CHTL compiler components
add_library(chtl_lib STATIC ${CHTL_SOURCES})

# Add the src directory to the include path for the library
target_include_directories(chtl_lib PUBLIC ${CMAKE_SOURCE_DIR}/src)


# --- Main Executable ---
# This would be the main compiler executable.
add_executable(chtl_compiler ${CMAKE_SOURCE_DIR}/src/main.cpp)
target_link_libraries(chtl_compiler PRIVATE chtl_lib)


# --- Test Suite ---
# Enable testing with CTest
enable_testing()

# Add the test executable
add_executable(chtl_tests ${CMAKE_SOURCE_DIR}/tests/main.cpp)

# Link the test executable against the CHTL library
target_link_libraries(chtl_tests PRIVATE chtl_lib)

# Add a test to CTest
add_test(NAME RunCHTLTests COMMAND chtl_tests)
