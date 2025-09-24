# Defines the CHTL_Compiler library.
add_library(CHTL_Compiler STATIC
    CHTL.cpp
    CHTLState/State.h
    CHTLStrategy/Strategy.h
    CHTLContext/Context.h
    CHTLContext/Context.cpp
)

# Exposes the 'CHTL' directory for include lookups.
target_include_directories(CHTL_Compiler
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
