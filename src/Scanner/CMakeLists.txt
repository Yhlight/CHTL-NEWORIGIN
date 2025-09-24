# Defines the CHTL_Scanner library.
add_library(CHTL_Scanner STATIC
    Scanner.h
    Scanner.cpp
)

# Exposes the 'Scanner' directory for include lookups.
target_include_directories(CHTL_Scanner
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
