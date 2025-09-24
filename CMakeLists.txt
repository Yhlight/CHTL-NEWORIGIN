cmake_minimum_required(VERSION 3.15)
project(CHTL_Project VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard to C++17 or higher for modern features
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set the output directory for binaries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable testing with CTest
enable_testing()

# Add subdirectories for each module/component of the project.
# We will create CMakeLists.txt files in these directories later.
add_subdirectory(Util)
add_subdirectory(CHTL)
# add_subdirectory(Scanner)
# add_subdirectory(CompilerDispatcher)
# add_subdirectory(CodeMerger)
# add_subdirectory(Test)

# Main executable for the CHTL compiler
add_executable(chtl main.cpp)

# Link libraries to the main executable.
# This will be populated as we build the components.
target_link_libraries(chtl
    PRIVATE
    Util
    CHTL_Compiler
)

# Add tests to be run by CTest
# add_test(NAME MyTest COMMAND MyTest)
