# Gather all the source files for the compiler library
file(GLOB_RECURSE CHTL_LIB_SOURCES "*.cpp" "*.h")

# Add a static library from the specified source files
add_library(chtl_lib STATIC ${CHTL_LIB_SOURCES})

# Specify the include directories for the library
# This allows other targets linking against this library to find its headers
target_include_directories(chtl_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
