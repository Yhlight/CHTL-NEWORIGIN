add_library(chtl_compiler STATIC
  src/Token.cpp
  src/Lexer.cpp
  src/Parser.cpp
  src/Generator.cpp
)

target_include_directories(chtl_compiler PUBLIC include)
target_link_libraries(chtl_compiler PRIVATE chtl_util)
