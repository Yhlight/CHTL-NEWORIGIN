# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(CHTL_Compiler CXX)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Dependencies ---
# Dependencies are now pre-built and stored in third-party/prebuilt
set(PREBUILT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third-party/prebuilt/include)
set(PREBUILT_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third-party/prebuilt/lib)

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
    src/CHTL/CHTLManage/CJMODManager.cpp
    src/CHTL/ExpressionParser/ExpressionLexer.cpp
    src/CHTL/ExpressionParser/ExpressionParser.cpp
    src/CHTL/ExpressionEvaluator/ExpressionEvaluator.cpp
    src/CJMOD/Syntax.cpp
    src/CJMOD/Arg.cpp
    src/CJMOD/AtomArg.cpp
    src/CJMOD/CJMODScanner.cpp
    src/CJMOD/CJMODGenerator.cpp
)

# Add a library for the CHTL compiler components
add_library(chtl_lib STATIC ${CHTL_SOURCES})

# Add include directories for chtl_lib
target_include_directories(chtl_lib PUBLIC src ${PREBUILT_INCLUDE_DIR})

# Link chtl_lib against its dependencies
target_link_libraries(chtl_lib PUBLIC
    ${PREBUILT_LIB_DIR}/libzippp_static.a
    ${PREBUILT_LIB_DIR}/libsass.a
    ${PREBUILT_LIB_DIR}/libqjs.a
    -lzip -lz -ldl -lm # System libraries required by dependencies
)


# --- Main Executable ---
# This would be the main compiler executable.
add_executable(chtl_compiler src/main.cpp)
target_link_libraries(chtl_compiler PRIVATE chtl_lib)

# Add the project root directory to the include path for the compiler executable
target_include_directories(chtl_compiler PRIVATE ${CMAKE_SOURCE_DIR})


# --- CMOD Packer Utility ---
# Add the cmod_packer executable
add_executable(cmod_packer src/cmod_packer/main.cpp)

# Link the packer against our CHTL library to get access to the parser
target_link_libraries(cmod_packer PRIVATE chtl_lib)

# Add the project root directory to the include path for the packer executable
target_include_directories(cmod_packer PRIVATE ${CMAKE_SOURCE_DIR})


# --- CJMOD Modules ---
# Compile the printMylove CJMOD as a shared library
add_library(printMylove_cjmod SHARED modules/Chtholly/cjmod/src/printMylove.cpp)

# Link the CJMOD against our CHTL library to get access to the API
target_link_libraries(printMylove_cjmod PRIVATE chtl_lib)

# Set a specific output directory for the CJMODs
set_target_properties(printMylove_cjmod PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/cjmods)


# --- Test Suite ---
# Enable testing with CTest
enable_testing()

# Add the test executable
add_executable(chtl_tests
    tests/main.cpp
    tests/ScannerTest.cpp
    tests/CHTLJSTest.cpp
    tests/CHTLParserTest.cpp
    tests/CoreFeaturesTest.cpp
    tests/LibSassIntegrationTest.cpp
    tests/QuickJSIntegrationTest.cpp
    tests/ExpressionEngineTest.cpp
    tests/CHTLJSGeneratorTest.cpp
    tests/ModuleSystemTest.cpp
    tests/CJMODAPITest.cpp
    tests/CLILogicTest.cpp
    # tests/OfficialModulesTest.cpp # Temporarily disabled due to multiple main() definitions
    tests/CJMODCodeGenTest.cpp
)

# Link the test executable against the CHTL library
target_link_libraries(chtl_tests PRIVATE chtl_lib)

# Make sure the test executable depends on the CJMOD being built first.
add_dependencies(chtl_tests printMylove_cjmod)

# Add the project root directory to the include path for tests
target_include_directories(chtl_tests PRIVATE ${CMAKE_SOURCE_DIR})

# Add a test to CTest
add_test(NAME RunCHTLTests COMMAND chtl_tests)