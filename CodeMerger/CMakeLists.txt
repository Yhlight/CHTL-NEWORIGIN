# CodeMerger CMakeLists.txt
add_library(CodeMerger STATIC
    Merger.h
    Merger.cpp
)

target_include_directories(CodeMerger PUBLIC
    ${CMAKE_SOURCE_DIR}/CodeMerger
)

target_link_libraries(CodeMerger PUBLIC
    Scanner
    Util
)