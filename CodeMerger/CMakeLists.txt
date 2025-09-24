add_library(chtl_merger STATIC
  src/CodeMerger.cpp
)

target_include_directories(chtl_merger PUBLIC include)
target_link_libraries(chtl_merger PRIVATE chtl_util)
