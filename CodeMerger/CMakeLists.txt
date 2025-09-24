# This will be a library for the CodeMerger
add_library(CodeMerger STATIC
    CodeMerger.cpp
)

# Publicly expose the CodeMerger directory for includes
target_include_directories(CodeMerger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
