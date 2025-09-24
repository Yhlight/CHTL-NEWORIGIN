add_library(chtljs_compiler STATIC
  src/Stub.cpp
)

target_include_directories(chtljs_compiler PUBLIC include)
target_link_libraries(chtljs_compiler PRIVATE chtl_util)
