# Defines the CHTL_CompilerDispatcher library.
add_library(CHTL_CompilerDispatcher STATIC
    CompilerDispatcher.h
    CompilerDispatcher.cpp
)

# Exposes the 'CompilerDispatcher' directory for include lookups.
target_include_directories(CHTL_CompilerDispatcher
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
