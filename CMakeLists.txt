# Sets the minimum required version of CMake
cmake_minimum_required(VERSION 3.15)

# Declares the project name and enables both C++ and C languages
project(CHTL LANGUAGES CXX C)

# Sets the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Dependency Management (Google Test) ---
# Includes the FetchContent module to manage dependencies
include(FetchContent)

# Declares the Google Test dependency
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

# Makes the dependency available to the project
# This will download and configure Google Test
FetchContent_MakeAvailable(googletest)

# Enables the testing framework for the project
enable_testing()

# Adds the main source directory to the build process
add_subdirectory(src)
