add_library(chtl_cmod STATIC
  src/CMODSystem.cpp
)

target_include_directories(chtl_cmod PUBLIC include)
target_link_libraries(chtl_cmod PRIVATE chtl_util)
