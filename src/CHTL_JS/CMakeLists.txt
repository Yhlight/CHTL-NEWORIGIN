# Defines the CHTL_JS_Compiler library.
add_library(CHTL_JS_Compiler STATIC
    CHTL_JS.h
    CHTL_JS.cpp
)

# Exposes the 'CHTL_JS' directory for include lookups.
target_include_directories(CHTL_JS_Compiler
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
