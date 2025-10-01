# Automatically find all source files in the current directory and subdirectories
file(GLOB_RECURSE CHTL_SOURCES CONFIGURE_DEPENDS "*.cpp")

# Create the CHTL library
add_library(CHTL ${CHTL_SOURCES})

# Publicly expose the include directory for this library
target_include_directories(CHTL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})