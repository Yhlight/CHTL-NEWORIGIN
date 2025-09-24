add_library(chtl_cjmod STATIC
  src/CJMODScanner.cpp
)

target_include_directories(chtl_cjmod PUBLIC include)
target_link_libraries(chtl_cjmod PRIVATE chtl_util)
