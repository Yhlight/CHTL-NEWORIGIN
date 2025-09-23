cmake_minimum_required(VERSION 3.16)

project(CHTL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(CHTL_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)

if(MSVC)
  add_compile_options(/W4)
  if(CHTL_WARNINGS_AS_ERRORS)
    add_compile_options(/WX)
  endif()
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
  if(CHTL_WARNINGS_AS_ERRORS)
    add_compile_options(-Werror)
  endif()
endif()

add_library(chtl_core
  src/lexer/Lexer.cpp
  src/parser/Parser.cpp
)

target_include_directories(chtl_core PUBLIC include)

add_executable(chtl
  src/main.cpp
)

target_link_libraries(chtl PRIVATE chtl_core)

