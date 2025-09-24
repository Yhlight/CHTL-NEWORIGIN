add_library(chtl_scanner STATIC
  src/UnifiedScanner.cpp
)

target_include_directories(chtl_scanner PUBLIC include)
target_link_libraries(chtl_scanner PRIVATE chtl_util)
