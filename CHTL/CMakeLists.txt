# Find all source files for the CHTL compiler
file(GLOB_RECURSE CHTL_SOURCES "*.cpp")

# Separate the main executable source from the library sources
list(FILTER CHTL_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")
set(CHTL_MAIN_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

# Add the static library for the CHTL components
add_library(chtl_lib STATIC ${CHTL_SOURCES})

# Set include directories for the library
target_include_directories(chtl_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Add the executable for the CHTL compiler
add_executable(chtl_compiler ${CHTL_MAIN_SOURCE})

# Link the executable against the library
target_link_libraries(chtl_compiler PRIVATE chtl_lib)