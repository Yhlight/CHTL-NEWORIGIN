cmake_minimum_required(VERSION 3.20)
project(CHTL LANGUAGES CXX)

option(CHTL_DEBUG "Enable debug logging" ON)
option(CHTL_ENABLE_INLINE "Inline output mode: none/all/css/js" OFF)
option(CHTL_DEFAULT_STRUCT "Enable default HTML structure" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CHTL_DEBUG)
  add_definitions(-DCHTL_DEBUG)
endif()

add_subdirectory(Scanner)
add_subdirectory(CompilerDispatcher)
add_subdirectory(CodeMerger)
add_subdirectory(CHTL)
add_subdirectory(CHTL_JS)
add_subdirectory(Util)
add_subdirectory(Test)

add_executable(chtl_cli examples/chtl_cli.cpp)
target_include_directories(chtl_cli PRIVATE ${CMAKE_SOURCE_DIR}/CompilerDispatcher ${CMAKE_SOURCE_DIR}/CodeMerger)
target_link_libraries(chtl_cli PRIVATE dispatcher codemerger)
target_compile_features(chtl_cli PRIVATE cxx_std_20)

