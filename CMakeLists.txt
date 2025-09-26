# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(CHTL_Compiler CXX)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Dependencies ---
add_subdirectory(third-party/libzippp)


# --- Define CHTL Library ---
# This makes it easy to link all CHTL source files to multiple executables.

# Define the source files for the CHTL library explicitly
set(CHTL_SOURCES
    src/CHTL/CHTLGenerator/Generator.cpp
    src/CHTL_JS/CHTLJSCompiler/CHTLJSCompiler.cpp
    src/CHTL/CHTLLexer/Lexer.cpp
    src/CHTL_JS/CHTLLexer/CHTLJSLexer.cpp
    src/CHTL_JS/CHTLJSParser/CHTLJSParser.cpp
    src/CHTL_JS/CHTLJSNode/CHTLJSEnhancedSelectorNode.cpp
    src/CHTL_JS/CHTLJSNode/RawJavaScriptNode.cpp
    src/CHTL_JS/CHTLJSNode/ListenNode.cpp
    src/CHTL_JS/CHTLJSNode/EventBindingNode.cpp
    src/CHTL_JS/CHTLJSNode/DelegateNode.cpp
    src/CHTL_JS/CHTLJSNode/AnimateNode.cpp
    src/CHTL_JS/CHTLJSNode/ScriptLoaderNode.cpp
    src/CHTL_JS/CHTLJSNode/VirtualObjectNode.cpp
    src/CHTL_JS/CHTLJSNode/VirtualObjectAccessNode.cpp
    src/CHTL_JS/CHTLJSGenerator/CHTLJSGenerator.cpp
    src/CHTL_JS/CHTLJSManage/VirtualObjectManager.cpp
    src/CHTL/CHTLLoader/Loader.cpp
    src/CHTL/CHTLManage/TemplateManager.cpp
    src/CHTL/CHTLNode/CommentNode.cpp
    src/CHTL/CHTLNode/ElementNode.cpp
    src/CHTL/CHTLNode/EnhancedSelectorNode.cpp
    src/CHTL/CHTLNode/InfoNode.cpp
    src/CHTL/CHTLNode/RawScriptNode.cpp
    src/CHTL/CHTLNode/ResponsiveValueNode.cpp
    src/CHTL/CHTLNode/ScriptNode.cpp
    src/CHTL/CHTLNode/TextNode.cpp
    src/CHTL/CHTLParser/Parser.cpp
    src/CHTL/CHTLState/ConfigurationState.cpp
    src/CHTL/CHTLState/StatementState.cpp
    src/CHTL/CHTLState/StyleBlockState.cpp
    src/CHTL/CHTLState/UseState.cpp
    src/CHTL/CHTLState/InfoState.cpp
    src/Dispatcher/CompilerDispatcher.cpp
    src/Scanner/UnifiedScanner.cpp
    src/CodeMerger/CodeMerger.cpp
    src/CHTL/Util/ASTUtil.cpp
    src/CHTL/Util/NodeCloner.cpp
    src/CHTL/Util/StyleUtil.cpp
)

# Add a library for the CHTL compiler components
add_library(chtl_lib STATIC ${CHTL_SOURCES})

# Add include directories for chtl_lib
target_include_directories(chtl_lib PUBLIC src)
target_include_directories(chtl_lib PUBLIC third-party/libzippp/src)

# Link chtl_lib against its dependencies
target_link_libraries(chtl_lib PUBLIC libzippp)


# --- Main Executable ---
# This would be the main compiler executable.
add_executable(chtl_compiler src/main.cpp)
target_link_libraries(chtl_compiler PRIVATE chtl_lib)


# --- CMOD Packer Utility ---
# Add the cmod_packer executable
add_executable(cmod_packer src/cmod_packer/main.cpp)

# Link the packer against libzippp
target_link_libraries(cmod_packer PRIVATE libzippp)

# Ensure the packer can find the libzippp headers
target_include_directories(cmod_packer PRIVATE third-party/libzippp/src)


# --- Test Suite ---
# Enable testing with CTest
enable_testing()

# Add the test executable
add_executable(chtl_tests tests/main.cpp)

# Link the test executable against the CHTL library
target_link_libraries(chtl_tests PRIVATE chtl_lib)

# Add a test to CTest
add_test(NAME RunCHTLTests COMMAND chtl_tests)
