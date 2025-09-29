# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(CHTL_Compiler CXX)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Dependencies ---
include(ExternalProject)

# LibSass (does not have CMake support, so we build it manually)
ExternalProject_Add(
    libsass_project
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third-party/libsass
    # No configure step needed for the default Makefile
    CONFIGURE_COMMAND ""
    # The command to build the library
    BUILD_COMMAND ${MAKE}
    # No install step needed, we will use it from the build tree
    INSTALL_COMMAND ""
    # Ensure the build happens in the source directory of libsass
    BUILD_IN_SOURCE 1
)

# Define the location for the compiled libsass library and its includes
set(LIBSASS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third-party/libsass/include)
set(LIBSASS_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/third-party/libsass/lib/libsass.a)

# Add the libzippp dependency
add_subdirectory(third-party/libzippp)

# Add the quickjs dependency
add_subdirectory(third-party/quickjs)


# --- Define CHTL Library ---
# This makes it easy to link all CHTL source files to multiple executables.

# Define the source files for the CHTL library explicitly
set(CHTL_SOURCES
    src/CHTL/CHTLGenerator/Generator.cpp
    src/CHTL_JS/CHTLJSCompiler/CHTLJSCompiler.cpp
    src/CHTL/CHTLLexer/Lexer.cpp
    src/CHTL_JS/CHTLLexer/CHTLJSLexer.cpp
    src/CHTL_JS/CHTLJSParser/CHTLJSParser.cpp
    src/CHTL_JS/CHTLJSNode/RawJavaScriptNode.cpp
    src/CHTL_JS/CHTLJSNode/ScriptLoaderNode.cpp
    src/CHTL_JS/CHTLJSNode/VirtualObjectNode.cpp
    src/CHTL_JS/CHTLJSNode/VirtualObjectAccessNode.cpp
    src/CHTL_JS/CHTLJSNode/RouterNode.cpp
    src/CHTL_JS/CHTLJSGenerator/CHTLJSGenerator.cpp
    src/CHTL_JS/CHTLJSManage/VirtualObjectManager.cpp
    src/CHTL/CHTLLoader/Loader.cpp
    src/CHTL/CHTLLoader/ModuleResolver.cpp
    src/CHTL/CHTLManage/TemplateManager.cpp
    src/CHTL/CHTLNode/CommentNode.cpp
    src/CHTL/CHTLNode/ElementNode.cpp
    src/CHTL/CHTLNode/EnhancedSelectorNode.cpp
    src/CHTL/CHTLNode/InfoNode.cpp
    src/CHTL/CHTLNode/RawScriptNode.cpp
    src/CHTL/CHTLNode/ScriptNode.cpp
    src/CHTL/CHTLNode/TextNode.cpp
    src/CHTL/CHTLNode/NamespaceNode.cpp
    src/CHTL/CHTLParser/Parser.cpp
    src/CHTL/CHTLState/ConfigurationState.cpp
    src/CHTL/CHTLState/StatementState.cpp
    src/CHTL/CHTLState/StyleBlockState.cpp
    src/CHTL/CHTLState/UseState.cpp
    src/CHTL/CHTLState/InfoState.cpp
    src/CHTL/CHTLState/NamespaceState.cpp
    src/CHTL/CHTLStrategy/ElementParsingStrategy.cpp
    src/Dispatcher/CompilerDispatcher.cpp
    src/Scanner/UnifiedScanner.cpp
    src/CodeMerger/CodeMerger.cpp
    src/CHTL/Util/ASTUtil.cpp
    src/CHTL/Util/NodeCloner.cpp
)

# Add a library for the CHTL compiler components
add_library(chtl_lib STATIC ${CHTL_SOURCES})

# Make sure chtl_lib depends on the libsass project being built
add_dependencies(chtl_lib libsass_project)

# Add include directories for chtl_lib
target_include_directories(chtl_lib PUBLIC src)
target_include_directories(chtl_lib PUBLIC third-party/libzippp/src)
target_include_directories(chtl_lib PUBLIC ${LIBSASS_INCLUDE_DIR})

# Link chtl_lib against its dependencies
target_link_libraries(chtl_lib PUBLIC libzippp ${LIBSASS_LIBRARY} qjs)


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
add_executable(chtl_tests tests/main.cpp tests/ScannerTest.cpp tests/CHTLJSTest.cpp tests/CHTLParserTest.cpp tests/CoreFeaturesTest.cpp tests/LibSassIntegrationTest.cpp tests/QuickJSIntegrationTest.cpp)

# Link the test executable against the CHTL library
target_link_libraries(chtl_tests PRIVATE chtl_lib)

# Add the project root directory to the include path for tests
target_include_directories(chtl_tests PRIVATE ${CMAKE_SOURCE_DIR})

# Add a test to CTest
add_test(NAME RunCHTLTests COMMAND chtl_tests)
