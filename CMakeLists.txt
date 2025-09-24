# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Project Name - Enable both CXX (C++) and C languages
project(CHTL_FINAL CXX C)

# Set a default build type to Debug if none was specified by the user.
# This enables debugging symbols and assertions, which helps catch bugs.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug, Release, RelWithDebInfo, MinSizeRel)." FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add subdirectories that contain their own CMakeLists.txt files
add_subdirectory(ThirdParty)
add_subdirectory(Util)
add_subdirectory(CHTL)
add_subdirectory(CHTL_JS)
add_subdirectory(Scanner)
add_subdirectory(CodeMerger)
add_subdirectory(CompilerDispatcher)
add_subdirectory(Test)
