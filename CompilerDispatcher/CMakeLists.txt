add_library(chtl_dispatcher STATIC
  src/CompilerDispatcher.cpp
)

target_include_directories(chtl_dispatcher PUBLIC include)
target_link_libraries(chtl_dispatcher PRIVATE chtl_scanner chtl_compiler chtl_merger)
