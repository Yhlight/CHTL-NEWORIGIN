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
    src/CHTL/CHTLGenerator/Generator.cpp
    src/CHTL/CHTLLexer/Lexer.cpp
    src/CHTL/CHTLLoader/Loader.cpp
    src/CHTL/CHTLManage/TemplateManager.cpp
    src/CHTL/CHTLNode/CommentNode.cpp
    src/CHTL/CHTLNode/ElementNode.cpp
    src/CHTL/CHTLNode/EnhancedSelectorNode.cpp
    src/CHTL/CHTLNode/AmpersandSelectorNode.cpp
    src/CHTL/CHTLNode/RawScriptNode.cpp
    src/CHTL/CHTLNode/ResponsiveValueNode.cpp
    src/CHTL/CHTLNode/ScriptNode.cpp
    src/CHTL/CHTLNode/TextNode.cpp
    src/CHTL/CHTLParser/Parser.cpp
    src/CHTL/CHTLState/ConfigurationState.cpp
    src/CHTL/CHTLState/StatementState.cpp
    src/CHTL/CHTLState/StyleBlockState.cpp
    src/CHTL/CHTLState/GlobalStyleBlockState.cpp
    src/CHTL/Util/ASTUtil.cpp
    src/CHTL/Util/NodeCloner.cpp
    src/CHTL/Util/StyleUtil.cpp
    src/CHTL/Util/StyleParsingUtil.cpp
)

# Add a library for the CHTL compiler components
add_library(chtl_lib STATIC ${CHTL_SOURCES})

# Add the src directory to the include path for the library
target_include_directories(chtl_lib PUBLIC src)


# --- Main Executable ---
# This would be the main compiler executable.
add_executable(chtl_compiler src/main.cpp)
target_link_libraries(chtl_compiler PRIVATE chtl_lib)


# --- Test Suite ---
# Enable testing with CTest
enable_testing()

# Add the test executable
add_executable(chtl_tests tests/main.cpp)

# Link the test executable against the CHTL library
target_link_libraries(chtl_tests PRIVATE chtl_lib)

# Add a test to CTest
add_test(NAME RunCHTLTests COMMAND chtl_tests)
