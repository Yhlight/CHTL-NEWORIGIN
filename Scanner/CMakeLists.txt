# This will be a library for the Unified Scanner
add_library(Scanner STATIC
    Scanner.cpp
)

# Publicly expose the Scanner directory for includes
target_include_directories(Scanner PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
