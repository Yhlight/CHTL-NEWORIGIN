cmake_minimum_required(VERSION 3.10)
project(CHTL_Compiler CXX C)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json for tooling support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add subdirectories that contain build targets
add_subdirectory(ThirdParty)
add_subdirectory(Util)
add_subdirectory(CHTL)
add_subdirectory(Scanner)
add_subdirectory(CompilerDispatcher)
