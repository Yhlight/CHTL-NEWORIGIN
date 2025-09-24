# Defines the CHTL_CodeMerger library.
add_library(CHTL_CodeMerger STATIC
    CodeMerger.h
    CodeMerger.cpp
)

# Exposes the 'CodeMerger' directory for include lookups.
target_include_directories(CHTL_CodeMerger
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
