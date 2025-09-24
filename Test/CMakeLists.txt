add_executable(token_print
  TokenTest/token_print.cpp
)

target_include_directories(token_print PRIVATE
  ${CMAKE_SOURCE_DIR}/CHTL
)
target_link_libraries(token_print PRIVATE chtl_core)

